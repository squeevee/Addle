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
// #include "idtypes/addleid.hpp"

#include <boost/mp11.hpp>

#include "utilities/config/factoryparams.hpp"

// rename InterfaceTraits
namespace Addle::Traits {

template<typename Interface> struct is_makeable : std::false_type {};

// rename is_service
template<typename Interface> struct is_singleton : std::false_type {};

template<typename Interface> struct repo_hints
{
    // # Sample members for specializations of this trait (all optional):
    //
    // ## Hint that Repository should or shouldn't make certain inferences 
    // static constexpr bool infer_defaults = false; (true by default)
    // If set to false:
    // - Repository will not automatically consider a key getter hint as a hint 
    //   for a default key type.
    // - Repository will not automatically consider Interface::id as a hint for 
    //   a default key type.
    // - Repository will not automatically consider Interface::id as a key 
    //   getter for key deduction.
    // - Repsoitory will not automatically consider any factory parameter for
    //   key inference in makeMember.
    //
    // Automatic inferrences are overridden by explicit hints.
    //
    // ## Hint one or more appropriate key types for the interface
    // using key_type = ...;
    // or using key_types = <Mp11-compatible type list>;
    //
    // ## Hint for getting a repo key from an interface instance
    // static const auto (&?) key_getter = <e.g., member function pointer>;
    // or static key_type key_getter(Interface&) { ... } 
    //
    // ## Hint a factory parameter corresponding to the key
    // (used by Repository::makeMember)
    // static const auto& key_param = <param keyword object>;
    //
    // ## Define specialized hints for different key types
    // template<typename T> using for_ = aux_<Interface>::repo_hints_impl<T>;
    // with:
    // namespace aux_<Interface> { 
    // template<typename T>
    // struct repo_hints_impl {
    //     ... infer_defaults = ... (as above)
    //     ... key_getter = ... (as above)
    //     ... key_param = ... (as above)
    // };
    // It's practical to place the struct itself in the aux_ namespace so it can 
    // be specialized.
    //
    // Hints given for a key type will override hints given for the interface
};

} // namespace Addle::Traits

#define ADDLE_DECL_MAKEABLE(Interface)                                      \
    template<> struct Addle::Traits                                         \
        ::is_makeable<Interface> : std::true_type {}; 

#define ADDLE_DECL_SERVICE(Interface)                                       \
    template<> struct Addle::Traits                                         \
        ::is_singleton<Interface> : std::true_type {};             


#endif // TRAITS_HPP
