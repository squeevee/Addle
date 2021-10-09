#pragma once

#include <boost/mp11.hpp>
#include <boost/type_traits.hpp>

#include <boost/di.hpp>
#include <boost/di/extension/injections/extensible_injector.hpp>

#include "./di.hpp"
#include "../config/factoryparams.hpp"

namespace Addle::aux_config3 {

template<bool, typename>
struct extendWithParams_impl;

template<bool UseNames, template<typename...> class L, typename... ParamList>
struct extendWithParams_impl<UseNames, L<ParamList...>>
{
    using params_t = L<ParamList...>;
    
    template<typename ParamEntry>
    static auto _entry(const params_t& params)
    {
        using value_t = boost::remove_cv_ref_t<typename ParamEntry::value_type>;
        
        const auto& keyword = 
            boost::parameter::keyword<typename ParamEntry::tag_type>::instance;
        auto&& value = params[keyword];
        
        static_assert(!std::is_pointer_v<std::remove_pointer_t<value_t>>
            && !std::is_pointer_v<std::remove_reference_t<value_t>>);
        // I doubt DI (as is) has a way to pass pointers-to-pointers or 
        // references-to-pointers as factory parameters, and in the (quite 
        // unlikely) case we need something like that, I'm sure we can work 
        // around it with a wrapper class.
        
        using bound_t = boost::mp11::mp_if<
                std::is_pointer<value_t>,
                std::remove_cv_t<std::remove_pointer_t<value_t>>,
                value_t
            >;
        
        if constexpr (UseNames)
            return boost::di::bind<bound_t>()
                .to(std::forward<decltype(value)>(value))
                .named(keyword)
                [boost::di::override];
        else
            return boost::di::bind<bound_t>()
                .to(std::forward<decltype(value)>(value))
                [boost::di::override];
    }
    
    template<class Interface, class Impl, typename Injector>
    static auto apply(Injector& injector, const params_t& params)
    {
        static_assert(std::is_same_v<params_t, config_detail::factory_params_t<Interface>>);
        
        return di_injector(
                injector.runtimeConfig(),
                boost::di::extension::make_extensible(injector),
                boost::di::bind<Interface>().template to<Impl>()[boost::di::override],
                _entry<ParamList>(params)...
            );
    }
};

template<class Interface, class Impl, typename Injector,
    template<typename...> class L, typename... ParamList>
auto extendWithParams(Injector& injector, const L<ParamList...>& params)
{
    constexpr bool UseNames = boost::mp11::mp_apply<boost::mp11::mp_any,
            boost::mp11::mp_transform<is_named, ctor_list<Impl>>
        >::value;

    return extendWithParams_impl<UseNames, L<ParamList...>>
        ::template apply<Interface, Impl>(injector, params);
}
    
}
