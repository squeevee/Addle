#ifndef STATICCONFIG_HPP
#define STATICCONFIG_HPP

#include <boost/type_traits/is_detected.hpp>

#ifndef ADDLE_SKIP_COMMON_STATIC_CONFIG

#include "interfaces/services/iviewrepository.hpp"
#include "utilities/view/viewrepository.hpp"

#endif

namespace Addle { namespace Config { namespace detail {

template<typename Interface>
struct static_binding {};

#define DECL_STATIC_BINDING(Interface, Impl_) \
template<> struct static_binding<Interface> { typedef Impl_ Impl; };

#define DECL_STATIC_TEMPLATE_BINDING(Interface, Impl_) \
template<typename T> struct static_binding<Interface<T>> { typedef Impl_<T> Impl; };

template<typename Interface>
using _detect_static_binding = typename static_binding<Interface>::Impl;

template<typename Interface>
using has_static_binding = boost::is_detected<_detect_static_binding, Interface>;


}}} // namespace Addle::Config::detail


#endif // STATICCONFIG_HPP
