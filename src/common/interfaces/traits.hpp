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

#include <boost/mp11.hpp>

#include "utilities/config/factoryparams.hpp"


namespace Addle::Traits {

template<typename Interface> struct is_makeable : std::false_type {};

template<typename Interface> struct is_singleton : std::false_type {};

template<typename Interface> struct repo_id_type {};

template<typename Interface> struct is_singleton_repo_member : std::false_type {};
template<typename Interface> struct is_unique_repo_member : std::false_type {};

template<typename Interface>
using is_repo_member = boost::mp11::mp_or<
        is_singleton_repo_member<Interface>,
        is_unique_repo_member<Interface>
    >;

} // namespace Addle::Traits

#define ADDLE_DECL_MAKEABLE(Interface)                                      \
    template<> struct Addle::Traits                                         \
        ::is_makeable<Interface> : std::true_type {}; 

#define ADDLE_DECL_SERVICE(Interface)                                       \
    template<> struct Addle::Traits                                         \
        ::is_singleton<Interface> : std::true_type {};             


#define ADDLE_DECL_SINGLETON_REPO_MEMBER_BASIC(Interface, IdType)           \
    ADDLE_DECL_MAKEABLE(Interface)                                          \
    template<> struct Addle::Traits                                         \
        ::is_singleton_repo_member<Interface> : std::true_type {};          \
    template<> struct Addle::Traits                                         \
        ::repo_id_type<Interface> { using type = IdType; };                 
        
        
#define ADDLE_DECL_SINGLETON_REPO_MEMBER(Interface, IdType)                 \
    ADDLE_DECL_SINGLETON_REPO_MEMBER_BASIC(Interface, IdType)               \
    ADDLE_DECL_FACTORY_PARAMETERS(                                          \
        Interface,                                                          \
        (required                                                           \
            ( id, (typename Addle::Traits::repo_id_type<Interface>::type))  \
        )                                                                   \
    )

#define ADDLE_DECL_UNIQUE_REPO_MEMBER_BASIC(Interface, IdType)              \
    ADDLE_DECL_MAKEABLE(Interface)                                          \
    template<> struct Addle::Traits                                         \
        ::is_unique_repo_member<Interface> : std::true_type {};             \
    template<> struct Addle::Traits                                         \
        ::repo_id_type<Interface> { using type = IdType; };                 
        
#define ADDLE_DECL_UNIQUE_REPO_MEMBER(Interface, IdType)                    \
    ADDLE_DECL_UNIQUE_REPO_MEMBER_BASIC(Interface, IdType)                  \
    ADDLE_DECL_FACTORY_PARAMETERS(                                          \
        Interface,                                                          \
        (required                                                           \
            ( id, (typename Addle::Traits::repo_id_type<Interface>::type))  \
        )                                                                   \
    )

#endif // TRAITS_HPP
