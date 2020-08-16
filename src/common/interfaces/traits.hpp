#ifndef TRAITS_HPP
#define TRAITS_HPP

#include <QObject>
#include <type_traits>
#include "idtypes/persistentid.hpp"
namespace Addle {

// TODO: the names and semantics of these traits could use some work

template<class T>
struct is_makeable : std::false_type
{
};

#define DECL_MAKEABLE(Interface) template<> struct is_makeable<Interface> : std::true_type {};

/**
 * @struct expects_initialize
 * @brief A static (compile-time) trait describing whether an interface expects
 * to be initailized
 * 
 * Use DECL_EXPECTS_INITIALIZE to apply this trait to an interface.
 *  
 * @sa
 * - [Initialization](src/docs/initialization.md)
 * - InitializeHelper
 * - DECL_EXPECTS_INITIALIZE
 */
template<class T>
struct expects_initialize : std::false_type
{
};

/**
 * @def DECL_EXPECTS_INITIALIZE
 * @brief
 * Convenience macro declaring that `Interface` has trait expects_initialize.
 * 
 * @param Interface
 * The interface being declared with the trait
 */
#define DECL_EXPECTS_INITIALIZE(Interface) template<> struct expects_initialize<Interface> : std::true_type {};

// template<class Interface>
// struct implemented_as_QObject : std::false_type {};

// /**
//  * @def DECL_IMPLEMENTED_AS_QOBJECT
//  * @brief
//  * Convenience macro declaring that `Interface` has trait implemented_as_QObject,
//  * and registering `Interface` as an interface in Qt.
//  * 
//  * @param Interface
//  * The interface being declared with the trait and registered.
//  */
// #define DECL_IMPLEMENTED_AS_QOBJECT(Interface) template<> struct implemented_as_QObject<Interface> : std::true_type {};


template<class Interface>
struct is_makeable_by_id : std::false_type { };

#define DECL_MAKEABLE_BY_ID(Interface, IdType_) \
template<> struct is_makeable_by_id<Interface> : std::true_type { typedef IdType_ IdType; };

template<class Interface>
struct is_gettable_by_id : std::false_type { };

#define DECL_PERSISTENT_OBJECT_TYPE(Interface, IdType_) \
template<> struct is_makeable_by_id<Interface> : std::true_type { typedef IdType_ IdType; }; \
template<> struct is_gettable_by_id<Interface> : std::true_type { typedef IdType_ IdType; };

} // namespace Addle
#endif // TRAITS_HPP