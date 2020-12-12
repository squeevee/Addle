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

#include <boost/mpl/placeholders.hpp>

namespace Addle {
namespace Traits {

template<typename I> struct is_public_makeable : std::false_type {};
template<typename I> struct is_private_makeable : std::false_type {};

template<typename I>
struct is_makeable : std::integral_constant<bool,
    is_public_makeable<I>::value
    || is_private_makeable<I>::value
> {};

// TODO: template aliases instead of derived structs for metafunctions
// TODO: boost::disjunction

template<typename I> struct init_params {};

template<typename I> struct is_service : std::false_type {};
template<typename I> struct is_singleton_gettable : std::false_type {};

template<typename I> struct repo_id_type {};

template<typename I> struct is_global_repo_gettable : std::false_type {};
template<typename I> struct is_local_repo_gettable : std::false_type {};

template<typename I> struct is_repo_gettable : std::integral_constant<bool,
    is_global_repo_gettable<I>::value
    || is_local_repo_gettable<I>::value
> {};

template<typename I> struct is_gettable : std::integral_constant<bool,
    is_singleton_gettable<I>::value
    || is_repo_gettable<I>::value
> {};

template<typename I> struct gettable_template_info {};

} // namespace Traits

/**
 * @def
 * @brief Declares that Interface can be made with ServiceLocator::make()
 */
#define DECL_MAKEABLE(Interface) \
namespace Traits { template<> struct is_public_makeable<Interface> : std::true_type {}; }

#define DECL_INIT_PARAMS(Interface, ...) \
namespace Traits { template<> struct init_params<Interface> { typedef std::tuple<__VA_ARGS__> type; }; }

/**
 * @def
 * @brief Declares that Interface is a service.
 */
#define DECL_SERVICE(Interface) \
namespace Traits { \
    template<> struct is_private_makeable<Interface> : std::true_type {}; \
    template<> struct is_singleton_gettable<Interface> : std::true_type {}; \
    template<> struct is_service<Interface> : std::true_type {}; \
}

/**
 * @def 
 * @brief Delcares that Interface belongs in a global repository, with instances
 * identified by IdType.
 */
#define DECL_GLOBAL_REPO_MEMBER(Interface, IdType) \
namespace Traits { \
    template<> struct is_private_makeable<Interface> : std::true_type {}; \
    template<> struct is_global_repo_gettable<Interface> : std::true_type {}; \
    template<> struct repo_id_type<Interface> { typedef IdType type; }; \
}

/**
 * @def 
 * @brief Declares that Interface is permissible in a local repository, with
 * instances identified by IdType.
 */
#define DECL_LOCAL_REPO_MEMBER(Interface, IdType) \
namespace Traits { \
    template<> struct is_private_makeable<Interface> : std::true_type {}; \
    template<> struct is_local_repo_gettable<Interface> : std::true_type {}; \
    template<> struct repo_id_type<Interface> { typedef IdType type; }; \
}

} // namespace Addle
#endif // TRAITS_HPP
