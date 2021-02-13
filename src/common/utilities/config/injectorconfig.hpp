/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#pragma once

#include <vector>

#include <boost/di.hpp>

#include <boost/mp11.hpp>
#include <boost/mp11/tuple.hpp>

#include <boost/type_traits/remove_cv_ref.hpp>

#include "idtypes/moduleid.hpp"

#include "interfaces/traits.hpp"
#include "interfaces/services/ifactory.hpp"
#include "interfaces/services/irepository.hpp"
#include "interfaces.hpp"

#include "factory.hpp"
#include "repository.hpp"
#include "bindingfilter.hpp"
#include "dynamicbindingserver.hpp"

#include "interfaces/views/iviewrepository.hpp"
#include "utilities/view/viewrepository.hpp"

#include "utilities/metaprogramming.hpp"

#include "injectbundle.hpp"

#include <QSharedPointer>

namespace boost::di {

// hackish, but it works
template<typename T>
struct _qsharedpointer_helper : QSharedPointer<T>
{    
    _qsharedpointer_helper(T* ptr)
        : QSharedPointer<T>(ptr)
    {
    }
    
    _qsharedpointer_helper(const _qsharedpointer_helper&) = default;
    _qsharedpointer_helper(_qsharedpointer_helper&&) = default;
};
    
template<typename T>
struct ctor_traits<QSharedPointer<T>>
{
    using boost_di_inject__ = aux::type_list<_qsharedpointer_helper<T>>; 
};

}

namespace Addle {

namespace config_detail {
    
using namespace boost::mp11; // TODO: nah

template<typename T>
using is_injector_makeable = mp_and<Traits::is_makeable<T>, mp_not<config_detail::has_factory_params<T>>>;

template<typename T>
using is_injector_instantiable = mp_or<Traits::is_singleton<T>, is_injector_makeable<T>>;

template<typename... Interfaces>
using injector_exposed_types =
    mp_append<
        mp_transform<
            std::add_lvalue_reference_t,
            mp_filter<Traits::is_singleton, mp_list<Interfaces...>>
        >,
        mp_transform<
            std::add_pointer_t,
            mp_filter<is_injector_makeable, mp_list<Interfaces...>>
        >,
        mp_transform<
            std::unique_ptr,
            mp_filter<is_injector_makeable, mp_list<Interfaces...>>
        >,
        mp_transform<
            QSharedPointer,
            mp_filter<is_injector_makeable, mp_list<Interfaces...>>
        >,
//         mp_transform_q<
//             mp_compose<IFactory, std::add_lvalue_reference_t>,
//             mp_filter<Traits::is_makeable, mp_list<Interfaces...>>
//         >,
        mp_transform_q<
            mp_compose<IFactory, std::add_const_t, std::add_lvalue_reference_t>,
            mp_filter<Traits::is_makeable, mp_list<Interfaces...>>
        >,
        
//         mp_transform_q<
//             mp_compose<IRepository, std::add_const_t, std::add_lvalue_reference_t>,
//             mp_filter<Traits::is_singleton_repo_member, mp_list<Interfaces...>>
//         >,
        
        mp_transform_q<
            mp_compose<IRepository, std::add_lvalue_reference_t>,
            mp_filter<Traits::is_singleton_repo_member, mp_list<Interfaces...>>
        >,
        
        mp_transform_q<
            mp_compose<IRepository, std::add_pointer_t>,
            mp_filter<Traits::is_unique_repo_member, mp_list<Interfaces...>>
        >,
        mp_transform_q<
            mp_compose<IRepository, std::unique_ptr>,
            mp_filter<Traits::is_unique_repo_member, mp_list<Interfaces...>>
        >,
        mp_transform_q<
            mp_compose<IRepository, QSharedPointer>,
            mp_filter<Traits::is_unique_repo_member, mp_list<Interfaces...>>
        >,
        
        mp_transform_q<
            mp_compose<IViewRepository, std::add_lvalue_reference_t>,
            mp_filter<aux_view::is_view, mp_list<Interfaces...>>
        >
    >;

template<class Interface, class Impl>
struct normal_binding
{
    using interface = Interface;
    using impl = Impl;
};

template<class Interface>
struct deferred_binding
{
    using interface = Interface;
};

template<class Interface>
struct deferred_conditional_binding
{
    using interface = Interface;
};

template<class Interface, class Impl>
struct filled_deferred_binding
{
    using interface = Interface;
    using impl = Impl;
};

template<class Interface, class Impl = void>
struct filled_conditional_binding
{
    using interface = Interface;
    using impl = Impl;
    
    binding_filter_t<Interface> filter;
};

// Note: these traits are not general purpose, as they can only compare class
// templates whose parameters are all types.

template<class Class, template<typename...> class Template>
struct _limited_is_spec_of : std::false_type {};

template<template<typename...> class Template, typename... Params>
struct _limited_is_spec_of<Template<Params...>, Template> : std::true_type {};

template<template<typename...> class T1, template<typename...> class T2>
struct _limited_is_same_template : std::false_type {};

template<template<typename...> class T>
struct _limited_is_same_template<T, T> : std::true_type {};

template<template<typename...> class Template>
struct is_binding_kind_q
{
    static_assert(
        mp_any<
            _limited_is_same_template<Template, normal_binding>,
            _limited_is_same_template<Template, deferred_binding>,
            _limited_is_same_template<Template, deferred_conditional_binding>,
            _limited_is_same_template<Template, filled_deferred_binding>,
            _limited_is_same_template<Template, filled_conditional_binding>
        >::value
    );
    
    template<class Class>
    using fn = _limited_is_spec_of<Class, Template>;
};

template<template<typename...> class Template, class Class>
using is_binding_kind = typename is_binding_kind_q<Template>::template fn<Class>;

template<typename T>
using is_immediate_binding = mp_or<
        is_binding_kind<normal_binding, T>,
        is_binding_kind<filled_deferred_binding, T>
    >;

template<typename T>
using is_delegate_binding = mp_or<
        is_binding_kind<deferred_binding, T>,
        is_binding_kind<deferred_conditional_binding, T>
    >;
    
template<typename Binding>
using bound_interface = typename std::remove_const_t<std::remove_reference_t<Binding>>::interface;
    
template<template<typename...> class Trait>
using bound_interface_has_trait = mp_bind<Trait, mp_bind<bound_interface, _1>>;

template<typename... Bindings>
auto get_immediate_bindings(const mp_list<Bindings...>&)
{
    return generate_tuple_over_list<
        mp_filter_q<
            bound_interface_has_trait<is_injector_instantiable>,
            mp_filter< is_immediate_binding , mp_list<Bindings...> >
        >
    >(
        [] (auto index) {
            using Interface = typename decltype(index)::type::interface;
            using Impl = typename decltype(index)::type::impl;
            
            return boost::di::bind<Interface>().template to<Impl>()[boost::di::override];
        }
    );
}

template<typename FactoryStorage, typename... Bindings>
auto get_immediate_factory_bindings(FactoryStorage& factoryStorage, const mp_list<Bindings...>&)
{
    return generate_tuple_over_list<
        mp_filter_q<
            bound_interface_has_trait<Traits::is_makeable>,
            mp_filter< is_immediate_binding , mp_list<Bindings...> >
        >
    >(
        [&] (auto index) {
            using Interface = typename decltype(index)::type::interface;
            using Impl = typename decltype(index)::type::impl;
            
            return boost::di::bind<IFactory<Interface>>()
                .template to([&](const auto& injector) -> const IFactory<Interface>& {
                        auto& factory = std::get< std::unique_ptr<IFactory<Interface>> >(factoryStorage);
                        if (!factory)
                        {
                            factory = std::make_unique<
                                Factory<Interface, Impl, boost::remove_cv_ref_t<decltype(injector)>>
                            >(
                                injector
                            );
                        }
                        return *factory;
                    })
                [boost::di::override];
        }
    );
}

template<typename... Interfaces>
auto get_singleton_repo_bindings(const mp_list<Interfaces...>&)
{
    return generate_tuple_over_list<
        mp_filter<
            Traits::is_singleton_repo_member,
            mp_list<Interfaces...>
        >
    >(
        [&] (auto index) {
            using Interface = typename decltype(index)::type;
            
            return boost::di::bind<IRepository<Interface>>()
                .template to<Repository<Interface>>()
                [boost::di::override];
        }
    );
}

template<typename... Interfaces>
auto get_unique_repo_bindings(const mp_list<Interfaces...>&)
{
    return generate_tuple_over_list<
        mp_filter<
            Traits::is_unique_repo_member,
            mp_list<Interfaces...>
        >
    >(
        [] (auto index) {
            using Interface = typename decltype(index)::type;
            
            return boost::di::bind<IRepository<Interface>>()
                .template to<Repository<Interface>>()
                [boost::di::override];
        }
    );
}

    
template<typename... Bindings>
auto get_delegate_bindings(const DynamicBindingServer& bindingServer, const mp_list<Bindings...>&)
{
    return std::tuple_cat(
        generate_tuple_over_list<
            mp_filter_q<
                bound_interface_has_trait<is_injector_instantiable>,
                mp_filter<is_delegate_binding, mp_list<Bindings...>>
            >
        >(
            [&] (auto t) {
                using Interface = typename decltype(t)::type::interface;
                
                return boost::di::bind<Interface>().template to(
                    [&]() -> Interface * {
                        return bindingServer.delegate_make<Interface>();
                    }
                )[boost::di::override];
            }
        ),
        generate_tuple_over_list<
            mp_filter_q<
                bound_interface_has_trait<Traits::is_makeable>,
                mp_filter<is_delegate_binding, mp_list<Bindings...>>
            >
        >(
            [&] (auto t) {
                using Interface = typename decltype(t)::type::interface;
                
                return boost::di::bind<IFactory<Interface>>()
                    .template to([&]() -> std::shared_ptr<IFactory<Interface>> { 
                        return bindingServer.getDelegateFactory<Interface>();
                    }
                )[boost::di::override];
            }
        )
    );
}

template<typename Injector, typename... Bindings>
void apply_dynamic_bindings(DynamicBindingServer& bindingServer, Injector& injector, Bindings&&... bindings)
{
    boost::mp11::tuple_for_each(
        boost::mp11::mp_apply<std::tuple, mp_list<Bindings...>>(
            std::forward<Bindings>(bindings)...
        ),
        [&bindingServer, &injector] (auto binding)
        {
            using Binding = decltype(binding);
            using Interface = typename Binding::interface;
            
            if constexpr (is_binding_kind<deferred_binding, Binding>::value)
            {
                bindingServer.deferBinding<Interface>();
            }
            else if constexpr (is_binding_kind<filled_deferred_binding, Binding>::value)
            {
                using Impl = typename Binding::impl;
                
                bindingServer.fillDeferredBinding<Interface>(
                    std::make_shared<Factory<Interface, Impl, Injector>>(injector)
                );
            }
//             else if constexpr (is_binding_kind<deferred_conditional_binding, Binding>::value)
//             {
//                 
//             }
            else if constexpr (is_binding_kind<filled_conditional_binding, Binding>::value)
            {
                using Impl = typename Binding::impl;
               
                bindingServer.fillConditionalBinding<Interface>(
                    std::move(binding.filter),
                    std::make_shared<Factory<Interface, Impl, Injector>>(injector)
                );
            }
        }
    );
}


template<typename... Interfaces>
auto get_view_repository_bindings(const mp_list<Interfaces...>&)
{
    return generate_tuple_over_list<
        mp_filter<
            aux_view::is_view,
            mp_list<Interfaces...>
        >
    >(
        [] (auto index) {
            using View = typename decltype(index)::type;
            
            return boost::di::bind<IViewRepository<View>>()
                .template to<ViewRepository<View>>()
                [boost::di::override];
        }
    );
}

template<typename... Interfaces>
class ModuleInjectorConfig
{
    using _injector_t = mp_apply<
            boost::di::injector,
            mp_append<
                injector_exposed_types<Interfaces...>
            >
        >;
    
    using _factory_storage_t = mp_apply<
            std::tuple,
            mp_transform_q<
                mp_compose<IFactory, std::unique_ptr>,
                mp_filter<Traits::is_makeable, mp_list<Interfaces...>>
            >
        >;
        
public:
    template<typename CoreInjector, typename... Bindings>
    ModuleInjectorConfig(CoreInjector& coreInjector, DynamicBindingServer& dynamicBindingServer, Bindings&&... bindings)
        : _injector (
            std::apply(
                [](auto&&... make_injector_args) {
                    return boost::di::make_injector(
                        std::forward<decltype(make_injector_args)>(make_injector_args)...
                    );
                },
                std::tuple_cat(
                    std::make_tuple( boost::di::extension::make_extensible(coreInjector) ),
                    get_immediate_bindings(mp_list<Bindings...>{}),
                    get_immediate_factory_bindings(_factoryStorage, mp_list<Bindings...>{})
                )
            )
        )
    {
        apply_dynamic_bindings(dynamicBindingServer, _injector, std::forward<Bindings>(bindings)...);
    }
    
private:
    _factory_storage_t _factoryStorage;
        
    _injector_t _injector;
};

template<typename... Interfaces>
class InjectorConfig
{
    using _injector_t = mp_apply<
            boost::di::injector,
            injector_exposed_types<Interfaces...>
        >;
        
    using _normal_factory_storage_t = mp_apply<
            std::tuple,
            mp_transform_q<
                mp_compose<IFactory, std::unique_ptr>,
                mp_filter<Traits::is_makeable, mp_list<Interfaces...>>
            >
        >;
    
public:
    
    template<typename... Bindings>
    InjectorConfig(Bindings&&... bindings)
        : _injector (
            std::apply(
                [](auto&&... make_injector_args) {
                    return boost::di::make_injector(
                        std::forward<decltype(make_injector_args)>(make_injector_args)...
                    );
                },
                std::tuple_cat(
                    get_immediate_bindings(mp_list<Bindings...>{}),
                    get_immediate_factory_bindings(_normalFactoryStorage, mp_list<Bindings...>{}),
                    get_singleton_repo_bindings(mp_list<Interfaces...>{}),
                    get_unique_repo_bindings(mp_list<Interfaces...>{}),
                    get_delegate_bindings(_dynamicBindingServer, mp_list<Bindings...>{}),
                    get_view_repository_bindings(mp_list<Interfaces...>{})
                )
            )
        )
    {   
        apply_dynamic_bindings(_dynamicBindingServer, _injector, std::forward<Bindings>(bindings)...);
    }
    
    template<typename Interface>
    Interface& getSingleton()
    {
        static_assert(
            Traits::is_singleton<boost::remove_cv_ref_t<Interface>>::value
        );
        return _injector.template create<Interface&>();
    }

    template<typename... Bindings>
    InjectorConfig& extend(Bindings&&... bindings)
    {
        using module_config_t = ModuleInjectorConfig<Interfaces...>;
        
        _modules.push_back(
            std::make_shared<module_config_t>(
                _injector,
                _dynamicBindingServer,
                std::forward<Bindings>(bindings)...
            )
        );
        
        return *this;
    }
    
private:
    _normal_factory_storage_t _normalFactoryStorage;
    
    DynamicBindingServer _dynamicBindingServer;
    std::vector<std::any> _modules;
    
    _injector_t _injector;
};


    
} // namespace config_detail

using InjectorConfig = config_detail::InjectorConfig<ADDLE_CONFIG_INTERFACES>;

namespace Config {

template<class Interface, class Impl>
inline config_detail::normal_binding<Interface, Impl> bind()
{ 
    return config_detail::normal_binding<Interface, Impl> {};
}

template<class Interface>
inline config_detail::deferred_binding<Interface> defer_binding()
{
    return config_detail::deferred_binding<Interface> {};
}

template<class Interface, class Impl>
inline config_detail::filled_deferred_binding<Interface, Impl> fill_deferred_binding()
{
    return config_detail::filled_deferred_binding<Interface, Impl> {};
}

template<class Interface>
inline config_detail::deferred_conditional_binding<Interface> defer_conditional_bind()
{
    return config_detail::deferred_conditional_binding<Interface> {};
}

template<class Interface, class Impl, class Filter>
inline config_detail::filled_conditional_binding<Interface, Impl> fill_conditional_bind(Filter&& filter)
{
    return config_detail::filled_conditional_binding<Interface, Impl> { std::move(filter) };
}

} // namespace Config

//defined by core module
//note: we're still in the Addle namespace
extern std::unique_ptr<InjectorConfig> core_config();

} // namespace Addle
