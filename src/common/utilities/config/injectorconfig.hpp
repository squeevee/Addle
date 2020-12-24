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

#include "utilities/generate_tuple_over.hpp"

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
    
using namespace boost::mp11;

template<typename T>
using is_injector_makeable = mp_and<Traits::is_public_makeable<T>, mp_not<config_detail::has_init_params<T>>>;

template<typename T>
using is_injector_instantiable = mp_or<Traits::is_singleton_gettable<T>, is_injector_makeable<T>>;

template<typename... Interfaces>
using module_injector_exposed_types =
    mp_append<
        mp_transform<
            std::add_lvalue_reference_t,
            mp_filter<Traits::is_service, mp_list<Interfaces...>>
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
        
        mp_transform_q<
            mp_compose<IFactory, std::add_const_t, std::add_lvalue_reference_t>,
            mp_filter<Traits::is_public_makeable, mp_list<Interfaces...>>
        >
    >;

template<typename... Interfaces>
using injector_exposed_types =
    mp_append<
        module_injector_exposed_types<Interfaces...>,
        
        mp_transform_q<
            mp_compose<IRepository, std::add_const_t, std::add_lvalue_reference_t>,
            mp_filter<Traits::is_global_repo_gettable, mp_list<Interfaces...>>
        >,
        
        mp_transform_q<
            mp_compose<IRepository, std::add_pointer_t>,
            mp_filter<Traits::is_local_repo_gettable, mp_list<Interfaces...>>
        >,
        mp_transform_q<
            mp_compose<IRepository, std::unique_ptr>,
            mp_filter<Traits::is_local_repo_gettable, mp_list<Interfaces...>>
        >,
        mp_transform_q<
            mp_compose<IRepository, QSharedPointer>,
            mp_filter<Traits::is_local_repo_gettable, mp_list<Interfaces...>>
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

template<class Interface, class Impl = void>
struct extensible_binding
{
    using interface = Interface;
    using impl = Impl;
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
struct is_binding_kind
{
    static_assert(
        mp_any<
            _limited_is_same_template<Template, normal_binding>,
            _limited_is_same_template<Template, deferred_binding>,
            _limited_is_same_template<Template, extensible_binding>
        >::value
    );
    
    template<class Class>
    using fn = _limited_is_spec_of<Class, Template>;
};

template<typename Binding>
using bound_interface = typename std::remove_const_t<std::remove_reference_t<Binding>>::interface;
    
template<template<typename...> class Trait>
using bound_interface_has_trait = mp_bind<Trait, mp_bind<bound_interface, _1>>;

template<typename... Bindings>
auto get_normal_bindings(const mp_list<Bindings...>&)
{
    return generate_tuple_over<
        mp_filter_q<
            bound_interface_has_trait<is_injector_instantiable>,
            mp_filter_q< is_binding_kind<normal_binding> , mp_list<Bindings...> >
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
auto get_normal_factory_bindings(FactoryStorage& factoryStorage, const mp_list<Bindings...>&)
{
    return generate_tuple_over<
        mp_filter_q<
            bound_interface_has_trait<Traits::is_public_makeable>,
            mp_filter_q< is_binding_kind<normal_binding> , mp_list<Bindings...> >
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

template<typename RepoStorage, typename... Interfaces>
auto get_singleton_repo_bindings(RepoStorage& repoStorage, const mp_list<Interfaces...>&)
{
    return generate_tuple_over<
        mp_filter<
            Traits::is_global_repo_gettable,
            mp_list<Interfaces...>
        >
    >(
        [&] (auto index) {
            using Interface = typename decltype(index)::type;
            
            return boost::di::bind<IRepository<Interface>>()
                .template to([&](const auto& injector) -> const IRepository<Interface> & {
                    auto& repo = std::get< std::unique_ptr<IRepository<Interface>> >(repoStorage);
                    if (!repo)
                    {
                        repo = std::make_unique<
                            SingletonRepository<Interface, boost::remove_cv_ref_t<decltype(injector)>>
                        >(
                            injector
                        );
                    }
                    return *repo;
                })
                [boost::di::override];
        }
    );
}

template<typename... Interfaces>
auto get_unique_repo_bindings(const mp_list<Interfaces...>&)
{
    return generate_tuple_over<
        mp_filter<
            Traits::is_local_repo_gettable,
            mp_list<Interfaces...>
        >
    >(
        [] (auto index) {
            using Interface = typename decltype(index)::type;
            
            return boost::di::bind<IRepository<Interface>>()
                .template to([](const auto& injector) -> IRepository<Interface> * {
                    return new UniqueRepository<Interface, boost::remove_cv_ref_t<decltype(injector)>>(
                        injector
                    );
                })
                [boost::di::override];
        }
    );
}

template<typename... Bindings>
auto get_deferred_bindings(const DynamicBindingServer& dynamicBindings, const mp_list<Bindings...>&)
{
    return std::tuple_cat(
        generate_tuple_over<
            mp_filter_q<
                bound_interface_has_trait<is_injector_instantiable>,
                mp_filter_q< is_binding_kind<deferred_binding>, mp_list<Bindings...> >
            >
        >(
            [&] (auto index) {
                using Interface = typename decltype(index)::type::interface;
                
                return boost::di::bind<Interface>().template to(
                    [&]() -> Interface * {
                        return dynamicBindings.make<Interface>(_nil_params {});
                    }
                )[boost::di::override];
            }
        ),
        generate_tuple_over<
            mp_filter_q<
                bound_interface_has_trait<Traits::is_makeable>,
                mp_filter_q< is_binding_kind<deferred_binding>, mp_list<Bindings...> >
            >
        >(
            [&] (auto index) {
                using Interface = typename decltype(index)::type::interface;
                
                return boost::di::bind<IFactory<Interface>>()
                    .template to([&]() -> std::shared_ptr<IFactory<Interface>> { 
                        return dynamicBindings.getFactory<Interface>();
                    })
                    [boost::di::override];
            }
        )
    );
}

template<typename... Interfaces>
class ModuleInjectorConfig
{
    using _injector_t = mp_apply<
            boost::di::injector,
            mp_append<
                module_injector_exposed_types<Interfaces...>
            >
        >;
    
    using _factory_storage_t = mp_apply<
            std::tuple,
            mp_transform_q<
                mp_compose<IFactory, std::unique_ptr>,
                mp_filter<Traits::is_public_makeable, mp_list<Interfaces...>>
            >
        >;
        
public:
    template<typename CoreInjector, typename... Bindings>
    ModuleInjectorConfig(CoreInjector& coreInjector, DynamicBindingServer& dynamicBindings, Bindings&&... bindings)
        : _injector (
            std::apply(
                [](auto&&... make_injector_args) {
                    return boost::di::make_injector(
                        std::forward<decltype(make_injector_args)>(make_injector_args)...
                    );
                },
                std::tuple_cat(
                    std::make_tuple( boost::di::extension::make_extensible(coreInjector) ),
                    get_normal_bindings(mp_list<Bindings...>{}),
                    get_normal_factory_bindings(_factoryStorage, mp_list<Bindings...>{})
                )
            )
        )
    {
        mp_for_each<
            mp_list<Bindings...>
        >(
            [&]( auto binding ) {
                using Interface = typename decltype(binding)::interface;
                
                if constexpr (has_init_params<Interface>::value)
                {
                    dynamicBindings.bind<Interface>([&] (init_params_t<Interface> params) {
                        return _injector.template create<const IFactory<Interface>&>()
                            .make_p(params);
                    });
                }
                else
                {
                    dynamicBindings.bind<Interface>([&] (_nil_params) {
                        return _injector.template create<Interface*>();
                    });
                }
            }
        );
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
                mp_filter<Traits::is_public_makeable, mp_list<Interfaces...>>
            >
        >;
        
    using _repo_storage_t = mp_apply<
            std::tuple,
            mp_transform_q<
                mp_compose<IRepository, std::unique_ptr>,
                mp_filter<Traits::is_global_repo_gettable, mp_list<Interfaces...>>
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
                    get_normal_bindings(mp_list<Bindings...>{}),
                    get_normal_factory_bindings(_normalFactoryStorage, mp_list<Bindings...>{}),
                    get_singleton_repo_bindings(_repoStorage, mp_list<Interfaces...>{}),
                    get_unique_repo_bindings(mp_list<Interfaces...>{}),
                    get_deferred_bindings(_dynamicBindings, mp_list<Bindings...>{})
                )
            )
        )
    {   
    }
    
    template<typename Interface>
    Interface& getSingleton()
    {
        static_assert(Traits::is_singleton_gettable<Interface>::value);
        
        return _injector.template create<Interface&>();
    }
    
    template<typename... Bindings>
    InjectorConfig& extend(Bindings&&... bindings)
    {
        using module_config_t = 
            mp_apply<
                ModuleInjectorConfig,
                mp_transform<
                    bound_interface,
                    mp_list<Bindings...>
                >
            >;
        
        _modules.push_back(
            std::make_shared<module_config_t>(
                _injector,
                _dynamicBindings,
                std::forward<Bindings>(bindings)...
            )
        );
        
        return *this;
    }
    
private:
    _normal_factory_storage_t _normalFactoryStorage;
    _repo_storage_t _repoStorage;
    
    DynamicBindingServer _dynamicBindings;
    std::vector<std::any> _modules;
    
    _injector_t _injector;
};


    
} // namespace config_detail

using InjectorConfig = config_detail::InjectorConfig<ADDLE_CONFIG_INTERFACES>;

template<class Interface, class Impl>
inline config_detail::normal_binding<Interface, Impl> bind() { return config_detail::normal_binding<Interface, Impl>{}; }

template<class Interface>
inline config_detail::deferred_binding<Interface> defer_binding() { return config_detail::deferred_binding<Interface>{}; }

//defined by core module
//note: we're still in the Addle namespace
extern std::unique_ptr<InjectorConfig> core_config();

} // namespace Addle
