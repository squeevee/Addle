/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
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
#include <functional>

#include <QSet>
#include <QHash>
#include <QMultiHash>
#include <QMutex>
#include <QSharedPointer>

#include "interfaces/ifactory.hpp"

#include "exceptions/servicelocatorexceptions.hpp"

#include "interfaces/traits.hpp"

#include "utilities/collections.hpp"
#include "utilities/hashfunctions.hpp"
namespace Addle {

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
class ADDLE_COMMON_EXPORT ServiceLocator
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
            ADDLE_THROW(ServiceLocatorNotInitializedException());

        return *_instance->get_p<
            typename std::remove_const<Interface>::type
        >();
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
     * of AddleId)
     * 
     * @tparam Interface
     * The interface of the desired object.
     * 
     * @note
     * Interface must specify it is gettable by ID (e.g. with the
     * `DECL_PERSISTENT_OBJECT_TYPE` macro)
     */
    template<class Interface, class IdType>
    static Interface& get(IdType id)
    {
        if (!_instance)
            ADDLE_THROW(ServiceLocatorNotInitializedException());

        return *_instance->get_p<
            typename std::remove_const<Interface>::type
        >(id);
    }

    template<class Interface, class IdType>
    static bool has(IdType id)
    {
        static_assert(
            Traits::is_gettable_by_id<
                typename std::remove_const<Interface>::type
            >::value,
            "Interface is not gettable by Id"
        );
        static_assert(
            std::is_convertible<IdType,
                typename Traits::id_type<
                    typename std::remove_const<Interface>::type
                >::type
            >::value,
            "Interface is not gettable by IdType"
        );

        if (!_instance)
            ADDLE_THROW(ServiceLocatorNotInitializedException());

        auto index = qMakePair(
            id,
            std::type_index(
                typeid(typename std::remove_const<Interface>::type)
            )
        );

        return _instance->_factoriesById.contains(index) ||
            _instance->_persistentObjectsById.contains(index);
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
     * is_makeable_by_id
     */
    template<class Interface, typename... ArgTypes>
    static typename std::enable_if<
        !Traits::is_public_makeable_by_id<
            typename std::remove_const<Interface>::type
        >::value,
        Interface*
    >::type make(ArgTypes... args)
    {
        static_assert(
            Traits::is_public_makeable_by_type<Interface>::value,
            "Interface must be publicly makeable."
        );

        if (!_instance)
            ADDLE_THROW(ServiceLocatorNotInitializedException());

        auto result = _instance->make_p<
            typename std::remove_const<Interface>::type
        >();
        
        try
        {
            initialize_if_needed(*result, args...);
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
    template<class Interface, class IdType, typename... ArgTypes>
    static typename std::enable_if<
        Traits::is_public_makeable_by_id<
            typename std::remove_const<Interface>::type
        >::value
        && std::is_convertible<
            IdType,
            typename Traits::id_type<
                typename std::remove_const<Interface>::type
            >::type
        >::value,
        Interface*
    >::type make(IdType id, ArgTypes... args)
    {
        if (!_instance)
            ADDLE_THROW(ServiceLocatorNotInitializedException());

        auto result = _instance->make_p<
            typename std::remove_const<Interface>::type
        >(id);
        
        try
        {
            initialize_if_needed(*result, args...);
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

    template<class Interface>
    static QSet<typename Traits::id_type<Interface>::type> getIds()
    {
        static_assert(
            Traits::is_gettable_by_id<
                typename std::remove_const<Interface>::type
            >::value,
            "Interface must be gettable by Id"
        );

        QSet<
            typename Traits::id_type<
                typename std::remove_const<Interface>::type
            >::type
        > result;

        std::type_index interfaceIndex(
            typeid(typename std::remove_const<Interface>::type)
        );

        {
            auto&& begin = noDetach(_instance->_factoriesById).keyBegin();
            auto&& end = noDetach(_instance->_factoriesById).keyEnd();
            for(auto i = begin; i != end; ++i)
            {
                auto id = (*i).first;
                auto entryInterfaceIndex = (*i).second;

                if (entryInterfaceIndex == interfaceIndex
                    && !result.contains(id) )
                {
                    result.insert(id);
                }
            }
        }

        {
            auto&& begin = noDetach(_instance->_persistentObjectsById).keyBegin();
            auto&& end = noDetach(_instance->_persistentObjectsById).keyEnd();
            for(auto i = begin; i != end; ++i)
            {
                auto id = (*i).first;
                auto entryInterfaceIndex = (*i).second;

                if (entryInterfaceIndex == interfaceIndex
                    && !result.contains(id) )
                {
                    result.insert(id);
                }
            }
        }

        return result;
    }

private:
    // Trait-like helper functions that determine whether the given type has an
    // initialize function.
    //
    // (The `int`/`long` dummy arguments are a SFINAE trick. Calling this
    // function with 0 as the argument will favor the `int` overload if it
    // exists, i.e., there is an initialize function. But if there is no
    // initialize function only the `long` specialization exists.)

    template<typename T, typename... ArgTypes>
    static constexpr decltype(
        // Note: If a call is made to ServiceLocator::make() with an invalid
        // set of arguments then it should produce an error here.
        std::declval<T&>().initialize(std::declval<ArgTypes>()...), 
    bool()) needs_init(int) { return true; }

    // Note: this specialization accepting only one template parameter will
    // cause the build to fail if arguments are passed into make() for an
    // interface that doesn't have an initialize funciton. This is desired
    // behavior but may be unclear from error messages if it happens.
    template<typename T>
    static constexpr bool needs_init(long) { return false; }

    template<typename T>
    static constexpr decltype(
        std::declval<T>().initialize(), 
    bool()) needs_no_param_init(int) { return true; }

    template<typename T>
    constexpr bool needs_no_param_init(long) { return false; }

    // For interfaces that provide an initialize function, this will call it
    // with 0 or more given arguments.
    template<typename T, typename... ArgTypes>
    static inline typename std::enable_if<
        needs_init<T, ArgTypes...>(0)
    >::type initialize_if_needed(T& obj, ArgTypes... args)
    {
        static_assert(
            std::is_void<decltype(obj.initialize(args...))>::value,
            "initialize function should have return type void"
        );
        obj.initialize(args...);
    }

    // For interfaces that do not provide an initialize function, this will do
    // nothing.
    template<typename T>
    static inline typename std::enable_if<
        !needs_init<T>(0) && !needs_no_param_init<T>(0)
    >::type initialize_if_needed(T& obj)
    {
        Q_UNUSED(obj);
    }

    ServiceLocator() = default;
    virtual ~ServiceLocator()
    {
        for (auto i = _services.keyValueBegin(); i != _services.keyValueEnd(); ++i)
        {
            std::type_index type = (*i).first;
            void* service = (*i).second;
            
            _factoriesByType[type]->delete_(service);
        }
        for (const IFactory* factory : _factoriesByType) delete factory;
    }

    static ServiceLocator* _instance;
    
    QHash<std::type_index, void*> _services;
    QHash<std::type_index, QSharedPointer<QMutex>> _serviceInitMutexes;
    QHash<std::type_index, const IFactory*> _factoriesByType;

    QHash<QPair<AddleId, std::type_index>, const IFactory*> _factoriesById;
    QHash<QPair<AddleId, std::type_index>, void*> _persistentObjectsById;
    
    // internal get by interface
    template<class Interface>
    Interface* get_p()
    {
        //todo: thread safety
        
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

            void* service = _services[interfaceIndex];
            return reinterpret_cast<Interface*>(service);
        }
        else
        {
            auto mutex = QSharedPointer<QMutex>(new QMutex);
            _serviceInitMutexes.insert(interfaceIndex, mutex);
            mutex->lock();

            if (!_factoriesByType.contains(interfaceIndex))
            {         
#ifdef ADDLE_DEBUG
                ServiceNotFoundException ex(typeid(Interface).name());
#else
                ServiceNotFoundException ex;
#endif 
                ADDLE_THROW(ex);
            }

            static_assert(
                !needs_init<Interface>(0),
                "Service types must not expect initialization"
            );

            Interface* service = make_p<Interface>();
            //service->setServiceLocator(this);
            _services[interfaceIndex] = reinterpret_cast<void*>(service);

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
            Traits::is_gettable_by_id<Interface>::value,
            "Interface must be gettable by id."
        );
        static_assert(
            std::is_base_of<AddleId, IdType>::value,
            "IdType must be AddleId"
        );

        //mutex
    
        auto index = qMakePair(id, std::type_index(typeid(Interface)));

        if (_persistentObjectsById.contains(index))
        {
            return reinterpret_cast<Interface*>(_persistentObjectsById[index]);
        }
        else 
        {
            if (!_factoriesById.contains(index))
            {
#ifdef ADDLE_DEBUG
                PersistentObjectNotFoundException ex(typeid(Interface).name(), id);
#else
                PersistentObjectNotFoundException ex;
#endif 
                ADDLE_THROW(ex);
            }

            static_assert(
                !needs_init<Interface>(0),
                "Persistent object types must not expect initialization"
            );

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
            Traits::is_makeable<Interface>::value,
            "Interface must be makeable"
        );


        auto index = std::type_index(typeid(Interface));

        if (!_factoriesByType.contains(index))
        {
#ifdef ADDLE_DEBUG
            FactoryNotFoundException ex(typeid(Interface).name());
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
            Traits::is_makeable_by_id<Interface>::value,
            "Interface must be makeable by id"
        );
        static_assert(
            std::is_same<typename Traits::id_type<Interface>::type, IdType>::value,
            "Interface must be makeable by id of type IdType"
        );
        static_assert(
            std::is_base_of<AddleId, IdType>::value,
            "IdType must be AddleId"
        );

        auto index = qMakePair(id, std::type_index(typeid(Interface)));

        if (!_factoriesById.contains(index))
        {
#ifdef ADDLE_DEBUG
            FactoryNotFoundException ex(typeid(Interface).name(), id);
#else
            FactoryNotFoundException ex;
#endif
            ADDLE_THROW(ex);
        }

        const IFactory* factory = _factoriesById[index];
        Interface* product = reinterpret_cast<Interface*>(factory->make());

        return product;
    }

    friend class ServiceConfigurationBase;
};

} // namespace Addle
#endif // SERVICELOCATOR_HPP
