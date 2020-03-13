/**
 * Addle source code
 * @file servicelocator.h
 * @copyright Copyright 2019 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef SERVICELOCATOR_HPP
#define SERVICELOCATOR_HPP

#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <QHash>
#include <QMultiHash>
#include <QMutex>
#include <QSharedPointer>

#include "interfaces/servicelocator/ifactory.hpp"
#include "interfaces/servicelocator/iservicelocator.hpp"
#include "interfaces/services/iservice.hpp"

#include "exceptions/servicelocatorexceptions.hpp"

#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "utilities/qtextensions/qhash.hpp"

/**
 * @class ServiceLocator
 * @brief Use the public static members of this class to access services and to
 * use object factories.
 * 
 * The ServiceLocator serves as a mediator between other classes in the
 * application, providing them access to each other without entangling them in
 * a dependency web. The Service Locator is itself essentially only an interface
 * relying on an external configuration to define its full behavior.
 * 
 * @note
 * Accessing ServiceLocator uses runtime type checking and QHash accessors. It 
 * is expected to be used infrequently by persistent objects. Be mindful of
 * situations where this may be too expensive.
 * 
 * @todo
 * ServiceLocator is closely tied to initialization, but I've also exposed some
 * options to willfully skip initialization. This feels necessary since I want
 * to model initialization in Addle as a convention and not a specialization of
 * C++, but I don't actually know under what circumstances an uninitialized
 * object would be useful -- and they could represent a major threading hazard.
 */
class ServiceLocator : public IServiceLocator
{
public:
    /**
     * @brief Get a service.
     * 
     * Provides a reference to the ServiceLocator's owned instance of the
     * service specified by `Interface`. If no such instance exists, then one is
     * created.
     * 
     * @tparam Interface
     * The interface of the desired service
     * 
     * @note
     * Interface must inherit IService.
     */
    template<class Interface>
    static Interface& get()
    {
        if (!_instance)
        {
            ADDLE_THROW(ServiceLocatorNotInitializedException());
        }

        return *_instance->get_p<Interface>();
    }

    /**
     * @brief Make an object.
     * 
     * Creates a new object that implements the interface specified by
     * Interface, and returns a pointer to this new object. If Interface
     * expects to be initialized, and it exposes an `initialize()` method
     * with no required parameters, it will be automatically initialized.
     * 
     * @tparam Interface
     * The interface of the desired object
     * 
     * @note
     * Interface must be make-able, i.e., must have trait is_makeable
     * 
     * @note
     * The caller is responsible to delete this object.
     * 
     * @sa
     * expects_initialize, is_makeable
     */
    template<class Interface>
    static Interface* make()
    {
        if (!_instance)
        {
            ADDLE_THROW(ServiceLocatorNotInitializedException());
        }

        Interface* result = _instance->make_p<Interface>();

        noParameterInitializeHelper(result, expects_initialize<Interface>());

        return result;
    }

    /**
     * @brief Make an object.
     * 
     * Creates a new object that implements the interface specified by
     * Interface, then calls the initialize function on that object with the
     * arguments given.
     * 
     * @tparam Interface
     * The interface of the desired object
     * 
     * @param args
     * The arguments of this function must be the same as if calling
     * `Interface::initialize()`
     * 
     * @note
     * Interface must be make-able, i.e., must have trait is_makeable
     * 
     * @note
     * `Interface` must expect initialization. To make interfaces that don't
     * expect initialization, use make() instead.
     * 
     * @note
     * The caller is responsible to delete this object.
     * 
     * @sa
     * expects_initialize, is_makeable
     */
    template<class Interface, typename... ArgTypes>
    static Interface* make(ArgTypes... args)
    {
        static_assert(
            expects_initialize<Interface>::value,
            "Calling ServiceLocator::make(...) requires Interface to expect initialization."
        );

        if (!_instance)
        {
            ADDLE_THROW(ServiceLocatorNotInitializedException());
        }

        Interface* result = _instance->make_p<Interface>();

        static_assert(
            std::is_same<void, decltype(result->initialize(args...))>::value,
            "Interface::initialize(...) must be return type void"
        );
        
        result->initialize(args...);

        return result;
    }

    /**
     * @brief Make an uninitialized object.
     * 
     * Creates a new object that implements the interface specified by 
     * `Interface`, and returns a pointer to this new object. Specifically does
     * not initialize the object.
     *  
     * @note
     * Interface must be make-able, i.e., must have trait is_makeable
     * 
     * @note
     * `Interface` must expect initialization. To make interfaces that don't
     * expect initialization, use make() instead.
     * 
     * @note
     * The caller is responsible to delete this object.
     * 
     * @sa
     * expects_initialize, is_makeable
     */
    template<class Interface>
    static Interface* make_uninitialized()
    {
        static_assert(
            expects_initialize<Interface>::value,
            "Calling ServiceLocator::make_uninitialized() requires Interface to expect initialization."
        );

        if (!_instance)
        {
            ADDLE_THROW(ServiceLocatorNotInitializedException());
        }

        return _instance->make_p<Interface>();;
    }

    template<class Interface>
    static QSharedPointer<Interface> makeShared()
    {
        return QSharedPointer<Interface>(make<Interface>());
    }

    template<class Interface, typename... ArgTypes>
    static QSharedPointer<Interface> makeShared(ArgTypes... args)
    {
        return QSharedPointer<Interface>(make<Interface>(args...));
    }

private:

    ServiceLocator() = default;
    virtual ~ServiceLocator()
    {
        for (IFactory* factory : _factoryRegistry) delete factory;
        for (IService* service : _serviceRegistry) delete service;
    }

    static ServiceLocator* _instance;
    
    QHash<std::type_index, IFactory*> _factoryRegistry;
    QHash<std::type_index, IService*> _serviceRegistry;
    QHash<std::type_index, QSharedPointer<QMutex>> _serviceInitMutexes;
    
    // Private internals of getting a service
    template<class Interface>
    Interface* get_p()
    {
        //todo: thread safety
        static_assert(
            std::is_base_of<IService, Interface>::value,
            "Interface must inherit IService"
        );
        static_assert(
            !std::is_same<IService, Interface>::value,
            "IService is the common base interface of services. It cannot be gotten directly."
        );
        
        std::type_index interfaceIndex(typeid(Interface));
        
        if (_serviceInitMutexes.contains(interfaceIndex))
        {
            // An initialization mutex exists for this service, i.e., it is
            // currently being constructed in another thread. We will wait
            // until it is finished.

            auto mutex = _serviceInitMutexes.value(interfaceIndex);
            mutex->lock();
            mutex->unlock();
        }

        if (_serviceRegistry.contains(interfaceIndex))
        {   
            // This service has already been made. Return the locator's instance.

            IService* service = _serviceRegistry[interfaceIndex];
            return dynamic_cast<Interface*>(service);
        }
        else
        {
            // This service has not yet been made. We must make it now.

            if (!_factoryRegistry.contains(interfaceIndex))
            {
#ifdef ADDLE_DEBUG
                ServiceNotFoundException ex(
                        typeid(Interface).name(),
                        _serviceRegistry.size(),
                        _factoryRegistry.size()
                );
#else
                ServiceNotFoundException ex;
#endif
                ADDLE_THROW(ex);
            }
            
            auto mutex = QSharedPointer<QMutex>(new QMutex);
            _serviceInitMutexes.insert(interfaceIndex, mutex);
            mutex->lock();

            Interface* service = make_p<Interface>();
            //service->setServiceLocator(this);
            _serviceRegistry[interfaceIndex] = service;

            _serviceInitMutexes.remove(interfaceIndex);
            mutex->unlock();

            return service;
        }
    }

    // Private internals of making an object
    template<class Interface>
    Interface* make_p()
    {
        static_assert(
            is_makeable<Interface>::value,
            "Interface must be makeable"
        );

        std::type_index interfaceIndex(typeid(Interface));
        
        if (!_factoryRegistry.contains(interfaceIndex))
        {
#ifdef ADDLE_DEBUG
            FactoryNotFoundException ex(
                    typeid(Interface).name(),
                    _factoryRegistry.size()
            );
#else
            FactoryNotFoundException ex;
#endif
            ADDLE_THROW(ex);
        }
        
        IFactory* factory = _factoryRegistry[interfaceIndex];
        Interface* product = reinterpret_cast<Interface*>(factory->make());

        return product;
    }

    // Helper functions to conditionally call no-parameter initialize() on
    // interfaces than expect it.

    template<class Interface>
    static void noParameterInitializeHelper(Interface* object, std::true_type)
    {
        static_assert(
            std::is_same<void, decltype(object->initialize())>::value,
            "Interface::initialize() must be return type void"
        );

        object->initialize();
    }
    
    template<class Interface>
    static void noParameterInitializeHelper(Interface* object, std::false_type)
    {
        Q_UNUSED(object)
    }

    friend class BaseServiceConfiguration;
};

#endif // SERVICELOCATOR_HPP
