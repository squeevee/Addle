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

template<typename I> struct is_public_makeable : std::false_type {};
template<typename I> struct is_private_makeable_by_type : std::false_type {};
template<typename I> struct is_private_makeable_by_id : std::false_type {};

template<typename I>
struct is_private_makeable : std::integral_constant<bool,
    is_private_makeable_by_type<I>::value || is_private_makeable_by_id<I>::value
> {};

template<typename I>
struct is_makeable : std::integral_constant<bool,
    is_public_makeable<I>::value
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

/**
 * @def
 * @brief Declares that Interface can be made with ServiceLocator::make()
 */
#define DECL_MAKEABLE(Interface) \
namespace Traits { template<> struct is_public_makeable<Interface> : std::true_type {}; }

/**
 * @def
 * @brief Declares that Interface is a service that can be gotten with
 * ServiceLocator::get()
 */
#define DECL_SERVICE(Interface) \
namespace Traits { \
    template<> struct is_private_makeable_by_type<Interface> : std::true_type {}; \
    template<> struct is_gettable_by_type<Interface> : std::true_type {}; \
}

/**
 * @def 
 * @brief Delcares that Interface is a persistent object type that can be gotten
 * with ServiceLocator::get() passing in an ID of type IdType
 */
#define DECL_PERSISTENT_OBJECT_TYPE(Interface, IdType) \
namespace Traits { \
    template<> struct is_private_makeable_by_id<Interface> : std::true_type {}; \
    template<> struct is_gettable_by_id<Interface> : std::true_type {}; \
    template<> struct id_type<Interface> { typedef IdType type; }; \
}

} // namespace Traits
} // namespace Addle
#endif // TRAITS_HPP
