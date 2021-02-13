/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#pragma once

#include <boost/parameter.hpp>

#include <functional>
#include <tuple>

#include <boost/type_traits/detected_or.hpp>
#include <boost/type_traits/is_detected_convertible.hpp>

#include "interfaces/iamqobject.hpp"
#include "interfaces/traits.hpp"
#include "interfaces/services/ifactory.hpp"

#include "utilities/qobject.hpp"

namespace Addle {
    
namespace config_detail {

template<typename Interface>
using _binding_filter_arg_factory_params_t = const factory_params_t<Interface>&;

template<typename Interface>
using _binding_filter_arg_id_t = typename Traits::repo_id_type<Interface>::type;
    
template<typename Interface>
using binding_filter_arg_t = typename boost::mp11::mp_if<
        has_factory_params<Interface>,
        boost::mp11::mp_defer<_binding_filter_arg_factory_params_t, Interface>,
        boost::mp11::mp_if<
            Traits::is_repo_member<Interface>,
            boost::mp11::mp_defer<_binding_filter_arg_id_t, Interface>,
            boost::mp11::mp_identity<void>
        >
    >::type;
    
template<typename Interface>
using binding_filter_t = std::function<bool(binding_filter_arg_t<Interface>)>;

template<typename Interface, typename F>
inline binding_filter_t<Interface> make_binding_filter(F&& f)
{
    return make_binding_filter<Interface>(std::forward<F>(f));
}

template<typename T>
using _static_cast_bool_t = decltype(static_cast<bool>(std::declval<T>()));

template<typename T>
using _dereferenced_t = decltype(*std::declval<T>());

template<typename T>
using _is_pointer_like = std::conjunction<
        boost::is_detected<_static_cast_bool_t, T>,
        boost::is_detected<_dereferenced_t, T>
    >;

// normalizes a parameter into a `const QObject*` from one of the following
// forms:
// - reference to (optionaly const-qualified) QObject
// - object, such as pointer, dereferenceable into (optionally const-qualified)
//  QObject
// - reference to (optionally const-qualified) interface implemented as QObject
// - object, such as pointer, dereferenceable into (optionally const-qualified) 
//  interface implemented as QObject
template<typename Parameter>
inline const QObject* _inspect_as_qobject(Parameter&& p)
{
    if constexpr (std::is_convertible_v<Parameter*, const QObject*>)
    {
        return static_cast<const QObject*>(std::addressof(p));
    }
    else if constexpr (_is_pointer_like<Parameter>::value)
    {
        if (!static_cast<bool>(p))
            return nullptr;
        
        return static_cast<const QObject*>(std::addressof(*p));
    }
    else if constexpr (Traits::implemented_as_QObject<
            std::remove_cv_t<std::remove_reference_t<Parameter>>
        >::value)
    {
        return static_cast<const QObject*>(
            std::addressof(qobject_interface_cast<const QObject&>(p))
        );
    }
    else if constexpr (std::conjunction_v<
            _is_pointer_like<Parameter>,
            Traits::implemented_as_QObject<
                std::remove_cv_t<std::remove_reference_t<_dereferenced_t<Parameter>>>
            >
        >)
    {
        if (!static_cast<bool>(p))
            return nullptr;
        
        return static_cast<const QObject*>(
            std::addressof(qobject_interface_cast<const QObject&>(*p))
        );
    }
    else
    {
        static_assert(std::is_void_v<Parameter>, "Unsupported parameter");
    }
}

} // namespace config_detail

namespace Config {

template<typename Interface, typename Filter, typename... ParameterTags>
inline auto filter_by_factory_parameters(Filter&& f, const boost::parameter::keyword<ParameterTags>&...)
{
    static_assert(config_detail::has_factory_params<Interface>::value);
    
    return [f] (const config_detail::factory_params_t<Interface>& params) -> bool {
        return std::apply(
            f,
            generate_tuple_over_list<boost::mp11::mp_list<ParameterTags...>>(
                [&params](auto t) -> auto& {
                    using ParameterTag_ = typename decltype(t)::type;
                    return params[boost::parameter::keyword<ParameterTag_>::instance];
                }
            )
        );
    };
}

template<typename Interface, typename ParameterTag>
inline auto filter_by_factory_parameter_qiid(const char* qiid, const boost::parameter::keyword<ParameterTag>& tag)
{
    QByteArray qiid_(qiid);

    return filter_by_factory_parameters(
        [qiid_] (auto&& arg) -> bool {
            const QObject* obj = config_detail::_inspect_as_qobject(std::forward<decltype(arg)>(arg));
            return obj && obj->inherits(qiid_.constData());
        },
        tag
    );
}

template<typename IdType>
inline auto filter_by_id(IdType id)
{
    return [id] (const auto& params) -> bool {
        return params[config_detail::generic_id_parameter::id_] == id;
    };
}

template<typename Interface, typename Function>
inline auto filter_by(Function&& f)
{
    return [f] (config_detail::binding_filter_arg_t<Interface> arg) -> bool {
        return std::apply(f, arg);
    };
}

}

} // namespace Addle
