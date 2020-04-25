/**
 * Addle source code
 * @file servicelocator.h
 * @copyright Copyright 2019 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef SERVICELOCATOR_HPP
#define SERVICELOCATOR_HPP

#include "compat.hpp"

#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <memory>
#include <QHash>
#include <QMultiHash>
#include <QMutex>
#include <QSharedPointer>

#include "interfaces/servicelocator/ifactory.hpp"
#include "interfaces/servicelocator/iservicelocator.hpp"
#include "interfaces/services/iservice.hpp"

#include "exceptions/servicelocatorexceptions.hpp"

#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/by_id_traits.hpp"
#include "interfaces/traits/initialize_trait.hpp"
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
 */
class ADDLE_COMMON_EXPORT ServiceLocator : public IServiceLocator
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
     * @brief Get a persistent object.
     * 
     * Provides a reference to the ServiceLocator's owned instance of the object
     * specified by `Interface` and `id`. If no such instance exists, then one
     * is created.
     * 
     * @param id
     * The persistent ID identifying the desired object. (Must be derived type
     * of PersistentId)
     * 
     * @tparam Interface
     * The interface of the desired object.
     * 
     * @note
     * Interface must specify it is gettable by ID (e.g. with the
     * `DECL_PERSISTENT_OBJECT_TYPE` macro)
     */
    template<class Interface, class TypeId>
    static Interface& get(TypeId id)
    {
        if (!_instance)
        {
            ADDLE_THROW(ServiceLocatorNotInitializedException());
        }

        return *_instance->get_p<Interface>(id);
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

        try
        {
            noParameterInitializeHelper(result);
        }
        catch(...)
        {
            delete result;
            throw;
        }

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
    template<class Interface, typename... ArgTypes,
        typename = typename std::enable_if<
            !is_makeable_by_id<Interface>::value
        >::type
    >
    static Interface* make(ArgTypes... args)
    {
        static_assert(
            expects_initialize<Interface>::value,
            "Interface did not expect initialization and is not makeable by ID. "
            "at least one of these must be true to use `ServiceLocator::make(...)` "
            "with arguments."
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
        
        try
        {
            result->initialize(args...);
        }
        catch(...)
        {
            delete result;
            throw;
        }

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
     * @param id
     * The persistent ID characterizing the desired object. This may be used by
     * ServiceLocator to select an implementation.
     * 
     * @note
     * Interface must be makeable by ID
     * 
     * @sa
     * is_makeable_by_id
     */
    template<class Interface, class TypeId,
        typename = typename std::enable_if<
            is_makeable_by_id<Interface>::value
        >::type
    >
    static Interface* make(TypeId id)
    {
        if (!_instance)
        {
            ADDLE_THROW(ServiceLocatorNotInitializedException());
        }

        Interface* result = _instance->make_p<Interface>(id);

        try
        {
            noParameterInitializeHelper(result);
        }
        catch(...)
        {
            delete result;
            throw;
        }

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
     * @param id
     * The persistent ID characterizing the desired object. This may be used by
     * ServiceLocator to select an implementation.
     * 
     * @param args
     * The arguments of this function must be the same as if calling
     * `Interface::initialize()`
     * 
     * @note
     * Interface must be makeable by ID
     * 
     * @sa
     * is_makeable_by_id
     */
    template<class Interface, class TypeId, typename... ArgTypes,
        typename = typename std::enable_if<
            is_makeable_by_id<Interface>::value
        >::type
    >
    static Interface* make(TypeId id, ArgTypes... args)
    {
        static_assert(
            expects_initialize<Interface>::value,
            "Interface must expect initialization to use "
            "`ServiceLocator::make(...)` with arguments after the id."
        );

        if (!_instance)
        {
            ADDLE_THROW(ServiceLocatorNotInitializedException());
        }

        Interface* result = _instance->make_p<Interface>(id);

        static_assert(
            std::is_same<void, decltype(result->initialize(args...))>::value,
            "Interface::initialize(...) must be return type void"
        );

        try
        {
            result->initialize(args...);
        }
        catch(...)
        {
            delete result;
            throw;
        }

        return result;
    }

    /**
     * @brief Convenience wrapper for ServiceLocator::make()
     * 
     * Wraps the made object in a QSharedPointer
     */
    template<class Interface, typename... ArgTypes>
    static QSharedPointer<Interface> makeShared(ArgTypes... args)
    {
        return QSharedPointer<Interface>(make<Interface>(args...));
    }

    /**
     * @brief Convenience wrapper for ServiceLocator::make()
     * 
     * Wraps the made object in a std::unique_ptr
     */
    template<class Interface, typename... ArgTypes>
    static std::unique_ptr<Interface> makeUnique(ArgTypes... args)
    {
        return std::unique_ptr<Interface>(make<Interface>(args...));
    }

private:

    ServiceLocator() = default;
    virtual ~ServiceLocator()
    {
        //for (IFactory* factory : _factoryRegistry) delete factory;
        for (IService* service : _services) delete service;
    }

    static ServiceLocator* _instance;
    
    QHash<std::type_index, IService*> _services;
    QHash<std::type_index, QSharedPointer<QMutex>> _serviceInitMutexes;
    QHash<std::type_index, const IFactory*> _factoriesByType;

    QHash<QPair<PersistentId, std::type_index>, const IFactory*> _factoriesById;
    QHash<QPair<PersistentId, std::type_index>, void*> _persistentObjectsById;
    
    // internal get by interface
    template<class Interface>
    Interface* get_p()
    {
        //todo: thread safety
        static_assert(
            std::is_base_of<IService, Interface>::value,
            "Interface must derive from IService (in order to be gotten without "
            "PersistentId)."
        );
        static_assert(
            !std::is_same<IService, Interface>::value,
            "IService is the common base interface of services. It cannot be "
            "gotten directly."
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

        if (_services.contains(interfaceIndex))
        {   
            // This service has already been made. Return the locator's instance.

            IService* service = _services[interfaceIndex];
            return dynamic_cast<Interface*>(service);
        }
        else
        {
            auto mutex = QSharedPointer<QMutex>(new QMutex);
            _serviceInitMutexes.insert(interfaceIndex, mutex);
            mutex->lock();

            Interface* service = make_p<Interface>();
            //service->setServiceLocator(this);
            _services[interfaceIndex] = service;

            _serviceInitMutexes.remove(interfaceIndex);
            mutex->unlock();

            return service;
        }
    }

    // internal get by interface and id
    template<class Interface, class IdType>
    Interface* get_p(IdType id)
    {
        static_assert(
            is_gettable_by_id<Interface>::value,
            "Interface must be gettable by id."
        );
        static_assert(
            std::is_same<typename is_makeable_by_id<Interface>::IdType, IdType>::value,
            "Interface must be makeable by id of type IdType"
        );
        static_assert(
            std::is_base_of<PersistentId, IdType>::value,
            "IdType must be PersistentId"
        );

        //mutex
    
        auto index = qMakePair(id, std::type_index(typeid(Interface)));

        if (_persistentObjectsById.contains(index))
        {
            return reinterpret_cast<Interface*>(_persistentObjectsById[index]);
        }
        else 
        {
            Interface* object = make_p<Interface>(id);
            _persistentObjectsById[index] = object;

            return object;
        }
    }

    // internal make object by interface (using static factory)
    template<class Interface>
    Interface* make_p()
    {
        static_assert(
            is_makeable<Interface>::value,
            "Interface must be makeable"
        );


        auto index = std::type_index(typeid(Interface));

        if (!_factoriesByType.contains(index))
        {
            
#ifdef ADDLE_DEBUG
            FactoryNotFoundException ex(
                typeid(Interface).name(),
                _factoriesByType.count()
            );
#else
            FactoryNotFoundException ex;
#endif
            ADDLE_THROW(ex);
        }

        const IFactory* factory = _factoriesByType[index];
        Interface* product = reinterpret_cast<Interface*>(factory->make());

        return product;
    }

    // internal make object by interface and id
    template<class Interface, class IdType>
    Interface* make_p(IdType id)
    {
        static_assert(
            is_makeable_by_id<Interface>::value,
            "Interface must be makeable by id"
        );
        static_assert(
            std::is_same<typename is_makeable_by_id<Interface>::IdType, IdType>::value,
            "Interface must be makeable by id of type IdType"
        );
        static_assert(
            std::is_base_of<PersistentId, IdType>::value,
            "IdType must be PersistentId"
        );

        auto index = qMakePair(id, std::type_index(typeid(Interface)));

        if (!_factoriesById.contains(index))
        {
            
#ifdef ADDLE_DEBUG
            FactoryNotFoundException ex(
                typeid(Interface).name(),
                _factoriesById.count()
            );
#else
            FactoryNotFoundException ex;
#endif
            ADDLE_THROW(ex);
        }

        const IFactory* factory = _factoriesById[index];
        Interface* product = reinterpret_cast<Interface*>(factory->make());

        return product;
    }

    // Helper functions to call no-parameter initialize() only on interfaces
    // that expect it

    template<class Interface>
    static inline typename std::enable_if<
        expects_initialize<Interface>::value
    >::type noParameterInitializeHelper(Interface* object)
    {
        static_assert(
            std::is_same<void, decltype(object->initialize())>::value,
            "Interface::initialize() must be return type void"
        );

        object->initialize();
    }
    
    template<class Interface>
    static inline typename std::enable_if<
        !expects_initialize<Interface>::value
    >::type noParameterInitializeHelper(Interface* object) noexcept
    {
        Q_UNUSED(object)
    }

    friend class BaseServiceConfiguration;
};

#endif // SERVICELOCATOR_HPP
