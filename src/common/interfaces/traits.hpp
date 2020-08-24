/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef TRAITS_HPP
#define TRAITS_HPP

#include <QObject>
#include <type_traits>
#include "idtypes/addleid.hpp"

namespace Addle {
namespace Traits {

template<typename I> struct is_public_makeable_by_type : std::false_type {};
template<typename I> struct is_public_makeable_by_id : std::false_type {};
template<typename I> struct is_private_makeable_by_type : std::false_type {};
template<typename I> struct is_private_makeable_by_id : std::false_type {};

template<typename I>
struct is_public_makeable : std::integral_constant<bool,
    is_public_makeable_by_type<I>::value || is_public_makeable_by_id<I>::value
> {};

template<typename I>
struct is_private_makeable : std::integral_constant<bool,
    is_private_makeable_by_type<I>::value || is_private_makeable_by_id<I>::value
> {};

template<typename I>
struct is_makeable_by_type : std::integral_constant<bool,
    is_public_makeable_by_type<I>::value || is_private_makeable_by_type<I>::value
> {};

template<typename I>
struct is_makeable_by_id : std::integral_constant<bool,
    is_public_makeable_by_id<I>::value || is_private_makeable_by_id<I>::value
> {};

template<typename I>
struct is_makeable : std::integral_constant<bool,
    is_public_makeable_by_type<I>::value
    || is_public_makeable_by_id<I>::value
    || is_private_makeable_by_type<I>::value 
    || is_private_makeable_by_id<I>::value
> {};

template<typename I> struct id_type {};

template<typename I> struct is_gettable_by_type : std::false_type {};
template<typename I> struct is_gettable_by_id : std::false_type {};

template<typename I>
struct is_gettable : std::integral_constant<bool,
    is_gettable_by_type<I>::value || is_gettable_by_id<I>::value
> {};

template<typename I> struct expects_initialize : std::false_type {};

#define DECL_MAKEABLE(Interface) \
namespace Traits { template<> struct is_public_makeable_by_type<Interface> : std::true_type {}; }

#define DECL_SERVICE(Interface) \
namespace Traits {\
    template<> struct is_private_makeable_by_type<Interface> : std::true_type {};\
    template<> struct is_gettable_by_type<Interface> : std::true_type {};\
}

#define DECL_PERSISTENT_OBJECT_TYPE(Interface, IdType) \
namespace Traits {\
    template<> struct is_private_makeable_by_id<Interface> : std::true_type {};\
    template<> struct is_gettable_by_id<Interface> : std::true_type {};\
    template<> struct id_type<Interface> { typedef IdType type; };\
}

#define DECL_EXPECTS_INITIALIZE(Interface) \
namespace Traits { template<> struct expects_initialize<Interface> : std::true_type {}; }


// // TODO: the names and semantics of these traits could use some work

// template<class T>
// struct is_makeable_by_type : std::false_type {};

// #define DECL_MAKEABLE_BY_TYPE(Interface) template<> struct is_makeable_by_type<Interface> : std::true_type {};

// template<class Interface>
// struct is_gettable_by_type : std::false_type {};

// #define DECL_GETTABLE_BY_TYPE(Interface) template<> struct is_gettable_by_type<Interface> : std::true_type {};



// template<class Interface>
// struct is_makeable_by_id : std::false_type {};

// // #define DECL_MAKEABLE_BY_ID(Interface, IdType_) \
// // template<> struct is_makeable_by_id<Interface> : std::true_type { typedef IdType_ IdType; };

// template<class Interface>
// struct is_gettable_by_id : std::false_type {};

// #define DECL_GETTABLE_BY_ID(Interface) template<> struct is_gettable_by_id<Interface> : std::true_type {};

// #define DECL_PERSISTENT_OBJECT_TYPE(Interface, IdType_) \
// template<> struct is_makeable_by_id<Interface> : std::true_type { typedef IdType_ IdType; }; \
// template<> struct is_gettable_by_id<Interface> : std::true_type { typedef IdType_ IdType; };

// /**
//  * @struct expects_initialize
//  * @brief A static (compile-time) trait describing whether an interface expects
//  * to be initailized
//  * 
//  * Use DECL_EXPECTS_INITIALIZE to apply this trait to an interface.
//  *  
//  * @sa
//  * - [Initialization](src/docs/initialization.md)
//  * - InitializeHelper
//  * - DECL_EXPECTS_INITIALIZE
//  */
// template<class T>
// struct expects_initialize : std::false_type
// {
// };

// /**
//  * @def DECL_EXPECTS_INITIALIZE
//  * @brief
//  * Convenience macro declaring that `Interface` has trait expects_initialize.
//  * 
//  * @param Interface
//  * The interface being declared with the trait
//  */
// #define DECL_EXPECTS_INITIALIZE(Interface) template<> struct expects_initialize<Interface> : std::true_type {};

} // namespace Traits
} // namespace Addle
#endif // TRAITS_HPP