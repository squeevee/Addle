/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#pragma once

#include <functional>
#include <tuple>

#include <boost/type_traits/detected_or.hpp>
#include <boost/type_traits/is_detected_convertible.hpp>

#include "interfaces/iamqobject.hpp"
#include "interfaces/traits.hpp"
#include "interfaces/services/ifactory.hpp"

#include "utilities/qobject.hpp"

namespace Addle::config_detail {

template<typename Interface>
using binding_filter_t = 
    std::function<bool(const init_params_placeholder_t<Interface>&)>;
    
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

// converts a parameter into a `const QObject*` from one of the following forms:
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

template<typename Interface, std::size_t N>
inline binding_filter_t<Interface> filter_binding_by_parameter_qiid(const char* qiid)
{
    static_assert(has_init_params<Interface>::value);
    
    QByteArray qiid_(qiid);

    return [qiid_] (const init_params_t<Interface>& params) -> bool {
        const QObject* param = _inspect_as_qobject(std::get<N>(params));
        return param->inherits(qiid_.constData());
    };
}

} // namespace Addle::config_detail
