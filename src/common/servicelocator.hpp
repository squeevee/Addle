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

#include <QtDebug>
#include <QSet>
#include <QHash>
#include <QMultiHash>
#include <QMutex>
#include <QSharedPointer>

#include "exceptions/servicelocatorexceptions.hpp"

#include "interfaces/traits.hpp"

#include "utilities/collections.hpp"
#include "utilities/hashfunctions.hpp"
#include "utilities/errors.hpp"

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
//         static_assert(
//             Traits::is_gettable_by_type<
//                 typename std::remove_const<Interface>::type
//             >::value,
//             "Interface is not gettable by type"
//         );
//         
//         if (!_instance)
//             ADDLE_THROW(ServiceLocatorNotInitializedException());
// 
//         return _instance->get_p<typename std::remove_const<Interface>::type>();
        std::abort();
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
     * `DECL_GLOBAL_REPO_MEMBER` macro)
     */
    template<class Interface, class IdType>
    static Interface& get(IdType id)
    {
//         static_assert(
//             Traits::is_gettable_by_id<
//                 typename std::remove_const<Interface>::type
//             >::value,
//             "Interface is not gettable by id"
//         );
//         static_assert(
//             std::is_convertible<IdType,
//                 typename Traits::id_type<
//                     typename std::remove_const<Interface>::type
//                 >::type
//             >::value,
//             "Interface is not gettable by IdType"
//         );
//         
//         if (!_instance)
//             ADDLE_THROW(ServiceLocatorNotInitializedException());
// 
//         ADDLE_ASSERT(id);
//         
//         return _instance->get_p<typename std::remove_const<Interface>::type>(id);
        std::abort();
    }

    template<class Interface, class IdType>
    static bool has(IdType id)
    {
//         static_assert(
//             Traits::is_gettable_by_id<
//                 typename std::remove_const<Interface>::type
//             >::value,
//             "Interface is not gettable by Id"
//         );
//         static_assert(
//             std::is_convertible<IdType,
//                 typename Traits::id_type<
//                     typename std::remove_const<Interface>::type
//                 >::type
//             >::value,
//             "Interface is not gettable by IdType"
//         );
// 
//         if (!_instance)
//             ADDLE_THROW(ServiceLocatorNotInitializedException());
//         
//         ADDLE_ASSERT(id);
// 
//         return _instance->_objects.contains(
//             qMakePair(std::type_index(typeid(Interface)), id)
//         );
        std::abort();
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
     * The caller is responsible to delete this object.
     */
    template<class Interface, typename... ArgTypes>
    static Interface* make(ArgTypes... args)
    {
//         static_assert(
//             Traits::is_public_makeable<Interface>::value,
//             "Interface must be publicly makeable."
//         );
// 
//         if (!_instance)
//             ADDLE_THROW(ServiceLocatorNotInitializedException());
// 
//         IFactory* factory = nullptr;
//         auto simpleIndex = qMakePair(std::type_index(typeid(Interface)), AddleId());
//         
//         factory = _instance->getFactory(FactorySelector::fromArgs<Interface>(args...));
//         if (!factory)
//         {
// #ifdef ADDLE_DEBUG
//             FactoryNotFoundException ex(typeid(Interface).name());
// #else
//             FactoryNotFoundException ex;
// #endif
//             ADDLE_THROW(ex);
//         }
//         
//         auto result = reinterpret_cast<Interface*>(factory->make());
//         
//         try
//         {
//             initialize_if_needed(*result, args...);
//         }
//         catch(...)
//         {
//             delete result;
//             throw;
//         }
// 
//         return result;
        std::abort();
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
        // Trait-like helper functions that determine whether the given type has an
    // initialize function.
    //
    // (The `int`/`long` dummy arguments are a SFINAE trick. Calling this
    // function with 0 as the argument will favor the `int` overload if it
    // exists, i.e., there is an initialize function. But if there is no
    // initialize function the `long` specialization will be used.)

//     template<typename T, typename... ArgTypes>
//     static constexpr decltype(
//         Note: If a call is made to ServiceLocator::make() with an invalid
//         set of arguments then it should produce an error here.
//         std::declval<T&>().initialize(std::declval<ArgTypes>()...), 
//     bool()) needs_init(int) { return true; }
// 
//     Note: this specialization accepting only one template parameter will
//     cause the build to fail if arguments are passed into make() for an
//     interface that doesn't have an initialize function. This is desired
//     behavior but may be unclear from error messages if it happens.
//     template<typename T>
//     static constexpr bool needs_init(long) { return false; }
// 
//     template<typename T>
//     static constexpr decltype(
//         std::declval<T>().initialize(), 
//     bool()) needs_no_param_init(int) { return true; }
// 
//     template<typename T>
//     static constexpr bool needs_no_param_init(long) { return false; }
// 
//     For interfaces that provide an initialize function, this will call it
//     with 0 or more given arguments.
//     template<typename T, typename... ArgTypes>
//     static inline typename std::enable_if<
//         needs_init<T, ArgTypes...>(0)
//     >::type initialize_if_needed(T& obj, ArgTypes... args)
//     {
//         static_assert(
//             std::is_void<decltype(obj.initialize(args...))>::value,
//             "initialize function should have return type void"
//         );
//         obj.initialize(args...);
//     }
// 
//     For interfaces that do not provide an initialize function, this will do
//     nothing.
//     template<typename T>
//     static inline typename std::enable_if<
//         !needs_init<T>(0) && !needs_no_param_init<T>(0)
//     >::type initialize_if_needed(T& obj)
//     {
//         Q_UNUSED(obj);
//     }
//     
//     ServiceLocator() = default;
//     virtual ~ServiceLocator();
// 
//     static ServiceLocator* _instance;
//     
//     TODO: caching
//     TODO: async safety
//     
//     QHash<QPair<std::type_index, AddleId>, void*> _objects;
//     QList<IServiceConfig*> _configs;
//     
//     Factories known to be correct for the given combination of interface type 
//     and (optionally) ID regardless of other arguments.
//     QHash<QPair<std::type_index, AddleId>, IFactory*> _indiscriminateFactories;
//     QHash<std::type_index, QSet<AddleId>> _knownIds;
//     
//     QHash<AddleId, IServiceConfig*> _pendingPersistentObjects;
//     
//     template<typename Interface>
//     inline Interface& get_p(AddleId id = AddleId())
//     {
//         auto index = qMakePair(std::type_index(typeid(Interface)), id);
//         
//         if (_objects.contains(index))
//         {
//             return *reinterpret_cast<Interface*>(_objects.value(index));
//         }
//         else
//         {
//             auto factory = getFactory(FactorySelector(index.first, index.second));
//             
//             if (!factory)
//             {
// #ifdef ADDLE_DEBUG
//                 FactoryNotFoundException ex(typeid(Interface).name(), id);
// #else
//                 FactoryNotFoundException ex;
// #endif
//                 ADDLE_THROW(ex);
//             }
//             
//             auto result = factory->make(id);
//             _objects[index] = result;
//             
//             if (id && _pendingPersistentObjects.contains(id))
//             {
//                 _pendingPersistentObjects.value(id)->initializeObject(id, result);
//                 _pendingPersistentObjects.remove(id);
//             }
//             
//             return *reinterpret_cast<Interface*>(result);
//         }
//     }
//         
//     IFactory* getFactory(const FactorySelector& selector);
//     
//     void addConfig(IServiceConfig* config);
// 
//     friend class ServiceLocatorControl;
};

} // namespace Addle

#endif // SERVICELOCATOR_HPP
