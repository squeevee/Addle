#ifndef MAKEABLE_TRAIT_HPP
#define MAKEABLE_TRAIT_HPP

#include <type_traits>

template<class T>
struct is_makeable : std::false_type
{
};

#define DECL_MAKEABLE(Interface) template<> struct is_makeable<Interface> : std::true_type {};

#endif // MAKEABLE_TRAIT_HPP