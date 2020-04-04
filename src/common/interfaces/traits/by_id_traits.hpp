#ifndef BY_ID_TRAITS_HPP
#define BY_ID_TRAITS_HPP

#include <type_traits>

#include "idtypes/persistentid.hpp"

template<class Interface>
struct is_makeable_by_id : std::false_type { };

#define DECL_MAKEABLE_BY_ID(Interface, IdType_) \
template<> struct is_makeable_by_id<Interface> : std::true_type { typedef IdType_ IdType; };

template<class Interface>
struct is_gettable_by_id : std::false_type { };

#define DECL_GETTABLE_BY_ID(Interface, IdType_) template<> struct is_gettable_by_id<Interface> : std::true_type { typedef IdType_ IdType; };


#endif // BY_ID_TRAITS_HPP