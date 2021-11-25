/**
 * This header should be included by the source file that defines a module's
 * `addle_module_config` routine, and not by any other files.
 */

#pragma once

#include "./servicestorage.hpp"
#include "./interfacebindingconfig.hpp"
#include "./di.hpp"

namespace Addle::aux_config3 {
    
template<class T>
struct _dep_is_override_of_q
{
    template<typename TDep>
    using fn = boost::mp11::mp_and<
            std::is_same<typename TDep::expected, T>,
            std::is_same<typename TDep::priority, boost::di::core::override>
        >;
};

template<class Interface, class Impl>
struct di_bind_functor_with_nullary_factory_params
{
    using params_t = config_detail::factory_params_t<Interface>;
   
    template<class Injector, typename _>
    Interface* operator() (const Injector& injector, boost::di::scopes::detail::arg<Interface*, Interface, _>) const
    {
        constexpr bool UseNames = boost::mp11::mp_apply<boost::mp11::mp_any,
                boost::mp11::mp_transform<is_named, ctor_list<Impl>>
            >::value;
                    
        return config_detail::make_factory_param_dispatcher<Interface>(
            [&](auto&& params) {
                return bindFactoryParams<UseNames, params_t>::extend(
                        const_cast<Injector&>(injector),
                        std::move(params),
                        boost::di::bind<Interface>.template to<Impl>()[boost::di::override]
                    ).template create<Interface*>();
            }
        )();
    }
    
    template<class Injector, typename _>
    QSharedPointer<Interface> operator() (const Injector& injector, boost::di::scopes::detail::arg<QSharedPointer<Interface>, Interface, _>) const
    {
        constexpr bool UseNames = boost::mp11::mp_apply<boost::mp11::mp_any,
                boost::mp11::mp_transform<is_named, ctor_list<Impl>>
            >::value;
                    
        return config_detail::make_factory_param_dispatcher<Interface>(
            [&](auto&& params) {
                return bindFactoryParams<UseNames, params_t>::extend(
                        const_cast<Injector&>(injector),
                        std::move(params),
                        boost::di::bind<Interface>.template to<Impl>()[boost::di::override]
                    ).template create<QSharedPointer<Interface>>();
            }
        )();
    }
};
    
template<class Interface, class Impl>
struct di_bind_functor_with_required_factory_params
{
    template <class...>
    struct _never { static constexpr auto value = false; };
    
    template<class Injector, typename _>
    Interface* operator() (const Injector&, boost::di::scopes::detail::arg<Interface*, Interface, _>) const
    {
        static_assert(_never<Injector>::value, "see comment at " __FILE__ ":" BOOST_PP_STRINGIZE(__LINE__));
        
        // WARNING 
        // We reach this point if all of the following are true:
        // - `Impl` is bound to `Interface`
        // - some class `A` is bound in the same injector
        // - the constructor of `A` takes a parameter of type `Interface*`
        // - `Interface` has one or more *required* factory parameters
        // 
        // Naively, the injector would attempt to default-construct all required 
        // parameters, which would cause unexpected behavior with no
        // diagnostics. This functor is here to raise a build error and prevent 
        // that.
        // 
        // Note that the related case where `Impl` and `A` are bound in 
        // different injectors, is already a build error since that implicitly 
        // goes through a factory parameter dispatch.
        //
        // Technically, we *could* support this case, by requiring the 
        // parameters to be present as named dependencies in the injector, but 
        // doing so would require a lot of non-trivial extension to the DI code 
        // for something that is probably not useful.
        
        Q_UNREACHABLE();
    }
    
    template<class Injector, typename _>
    QSharedPointer<Interface> operator() (const Injector&, boost::di::scopes::detail::arg<QSharedPointer<Interface>, Interface, _>) const
    {
        static_assert(_never<Injector>::value, "see comment at " __FILE__ ":" BOOST_PP_STRINGIZE(__LINE__));
        // see comment above
        
        Q_UNREACHABLE();
    }
};

template<typename TDep
    , bool = Traits::is_makeable<typename TDep::expected>::value
    , bool = config_detail::has_factory_params<typename TDep::expected>::value
    , bool = std::is_same_v<typename TDep::scope, boost::di::scopes::instance>>
struct _make_binding_traits
{
    static constexpr bool enable = false;
};

template<typename TDep, bool _>
struct _make_binding_traits<TDep, true, false, _>
{
    static constexpr bool enable = true;
    using interface_t   = typename TDep::expected;
    
    template<typename Injector>
    static void make_impl(void* result, void* injector_, void*, MakeFormat format)
    {
        const auto& injector = *reinterpret_cast<Injector*>(injector_);
        
        switch(format)
        {
            case MakeFormat::Plain:
                *reinterpret_cast<interface_t**>(result) 
                    = injector.template create<interface_t*>();
                return;
                
            case MakeFormat::Shared:
                *reinterpret_cast<QSharedPointer<interface_t>*>(result) 
                    = injector.template create<QSharedPointer<interface_t>>();
                return;
        }
        
        Q_UNREACHABLE();
    }
};

template<typename TDep>
struct _make_binding_traits<TDep, true, true, false>
{
    static constexpr bool enable = true;
    using interface_t   = typename TDep::expected;
    using impl_t        = typename TDep::given;
    using params_t      = config_detail::factory_params_t<interface_t>;
    
    template<typename Injector>
    static void make_impl(void* result, void* injector_, void* params_, MakeFormat format)
    {
        constexpr bool UseNames = boost::mp11::mp_apply<boost::mp11::mp_any,
                boost::mp11::mp_transform<is_named, ctor_list<impl_t>>
            >::value;

        auto injector = bindFactoryParams<UseNames, params_t>::template extend(
                  *reinterpret_cast<Injector*>(injector_) 
                , std::move(*reinterpret_cast<params_t*>(params_))
                , boost::di::bind<interface_t>.template to<impl_t>()[boost::di::override]
            );
            
        switch(format)
        {
            case MakeFormat::Plain:
                *reinterpret_cast<interface_t**>(result) 
                    = injector.template create<interface_t*>();
                return;
                
            case MakeFormat::Shared:
                *reinterpret_cast<QSharedPointer<interface_t>*>(result) 
                    = injector.template create<QSharedPointer<interface_t>>();
                return;
        }
        
        Q_UNREACHABLE();
    }
};

template<typename TDep>
struct _make_binding_traits<TDep, true, true, true>
{
    static constexpr bool enable = true;
    using interface_t   = typename TDep::expected;
    using params_t      = config_detail::factory_params_t<interface_t>;
    
    template<typename Injector>
    static void make_impl(void* result, void* injector_, void* params_, MakeFormat format)
    {
        auto injector = bindFactoryParams<false, params_t>::extend(
                  *reinterpret_cast<Injector*>(injector_) 
                , std::move(*reinterpret_cast<params_t*>(params_))
            );
            
        switch(format)
        {
            case MakeFormat::Plain:
                *reinterpret_cast<interface_t**>(result) 
                    = injector.template create<interface_t*>();
                return;
                
            case MakeFormat::Shared:
                *reinterpret_cast<QSharedPointer<interface_t>*>(result) 
                    = injector.template create<QSharedPointer<interface_t>>();
                return;
        }
        
        Q_UNREACHABLE();
    }
};

template<class Interface, class Injector>
void* _get_binding_impl(void* injector)
{
    return std::addressof(static_cast<Injector*>(injector)->template create<Interface&>());
}

/** 
 * ModuleConfig provides the means for extending InterfaceBindingConfig. A 
 * module's `addle_module_config` function will be given a ModuleConfig object, 
 * which it can use to bind the module's classes to Addle interfaces.
 * 
 * When all desired bindings are added to the ModuleConfig, it can be added to
 * the InterfaceBindingConfig by calling `commit()`. If there are any conflicts 
 * or inconsistencies, either internally or with InterfaceBindingConfig, this 
 * will throw an exception of type `ModuleConfigCommitFailure` and the module 
 * will not be initialized.
 */
// rename ModuleBindings?
class ModuleConfig
{
public:
    ModuleConfig(const ModuleConfig&) = delete;
    ModuleConfig(ModuleConfig&&) = default;
    
    QByteArray id() const { return _id; }
    
    const InterfaceBindingConfig& injectorConfig() const { return _injectorConfig; }
    InterfaceBindingConfig& injectorConfig() { return _injectorConfig; }
    
    template<typename... TDeps>
    ModuleConfig& addBindings(TDeps&&... deps);
    
    template<typename Interface, typename Impl, typename Tag, typename Value>
    ModuleConfig& bindOnParamValue(const boost::parameter::keyword<Tag>&, Value&& v);
    
//     template<typename Interface, typename Impl, typename Param>
//     ModuleConfig& bindOnParamType(const boost::parameter::keyword<Param>&, QByteArray typeName);
//     
//     template<typename Interface, typename Impl, typename Function>
//     ModuleConfig& bindOnCondition(Function&& func);
//     
//     template<typename Interface, typename Impl>
//     ModuleConfig& bindOnCondition(BindingCondition cond);
    
    void commit(); // defined in interfacebindingconfig.cpp
    
private:
    ModuleConfig(InterfaceBindingConfig& injectorConfig, QByteArray id)
        : _injectorConfig(injectorConfig), _id(std::move(id))
    {
    }
    
    InterfaceBindingConfig& _injectorConfig;
    QByteArray _id;
    
    bool _committed = false;
    
    std::vector<anonymous_unique_ptr> _injectors;
    QList<MakeBinding> _makeBindings;
    QList<GetBinding> _getBindings;
    QList<std::function<void()>> _postRoutines;
    
    friend class InterfaceBindingConfig;
};

template<class TDep>
using _make_binding_is_enabled = boost::mp11::mp_bool<_make_binding_traits<TDep>::enable>;

template<class TDep>
struct make_binder
{
    template<class Injector>
    MakeBinding operator()(Injector* injector) const
    {
        return {
              &_make_binding_traits<TDep>::template make_impl<Injector>
            , injector
            , &typeid(typename TDep::expected)
#ifdef ADDLE_DEBUG
            , &typeid(typename TDep::given)
#endif
        };
    }
};

template<class TDep>
using _get_binding_is_enabled = Traits::is_singleton<typename TDep::expected>;

template<class TDep>
using _binding_is_service_storage = std::is_base_of<di_service_storage_base, typename TDep::given>;

template<class TDep>
struct get_binder
{
    template<typename Injector>
    GetBinding operator()(Injector* injector) const
    {
        return {
              &_get_binding_impl<typename TDep::expected, Injector>
            , injector
            , &typeid(typename TDep::expected)
#ifdef ADDLE_DEBUG
            , &typeid(typename TDep::given)
#endif
        };
    }
};

template<typename TDep,
    class = typename TDep::expected,
    class = typename TDep::given,
    class = boost::mp11::mp_bool<std::is_convertible_v<typename TDep::scope, boost::di::scopes::instance>>,
    class = boost::mp11::mp_bool<Traits::is_makeable<typename TDep::expected>::value>,
    class = boost::mp11::mp_bool<config_detail::is_makeable_with_params<typename TDep::expected>::value>
>
struct dep_transform_trait_impl : boost::mp11::mp_false {};

template<typename TDep, typename Interface, typename Impl>
struct dep_transform_trait_impl<TDep, Interface, Impl, boost::mp11::mp_false, boost::mp11::mp_true, boost::mp11::mp_true>
    : boost::mp11::mp_true
{
    using functor_type = di_bind_functor_with_nullary_factory_params<Interface, Impl>;
};

template<typename TDep, typename Interface, typename Impl>
struct dep_transform_trait_impl<TDep, Interface, Impl, boost::mp11::mp_false, boost::mp11::mp_true, boost::mp11::mp_false>
    : boost::mp11::mp_true
{
    using functor_type = di_bind_functor_with_required_factory_params<Interface, Impl>;
};

template<typename TDep>
using dep_transform_trait = boost::mp11::mp_eval_or<
        boost::mp11::mp_false,
        dep_transform_trait_impl,
        TDep
    >;

template<typename TDep, std::enable_if_t<dep_transform_trait<boost::remove_cv_ref_t<TDep>>::value, void*> = nullptr>
auto _dep_transform(const TDep&)
{
    return boost::di::bind<typename TDep::expected>.to( typename dep_transform_trait<TDep>::functor_type {} );
}
    
template<typename TDep, std::enable_if_t<!dep_transform_trait<boost::remove_cv_ref_t<TDep>>::value, void*> = nullptr>
std::decay_t<TDep> _dep_transform(TDep&& dep) { return std::move(dep); }

template<typename... TDeps>
ModuleConfig& ModuleConfig::addBindings(TDeps&&... deps)
{
    using service_storage_list = boost::mp11::mp_filter<_get_binding_is_enabled, boost::mp11::mp_list<TDeps...>>;
    using service_storage_t = boost::mp11::mp_apply<
            ServiceStorage,
            boost::mp11::mp_unique_if<
                boost::mp11::mp_transform<_make_storage_pair, service_storage_list>,
                _storage_pair_match_interfaces
            >
        >;

    auto injector = new di_injector (
            &_injectorConfig, 
            _dep_transform(std::forward<TDeps>(deps))...,
            boost::di::bind<>.to(service_storage_t {})
        );
    
    using make_binding_list = boost::mp11::mp_filter<_make_binding_is_enabled, boost::mp11::mp_list<TDeps...>>;
    _makeBindings.reserve(_makeBindings.size() + boost::mp11::mp_size<make_binding_list>::value);
    
    boost::mp11::mp_for_each<boost::mp11::mp_transform<make_binder, make_binding_list>>(
        [&] (auto&& binder) { _makeBindings.append(binder(injector)); }
    );
    
//     using get_binding_list = boost::mp11::mp_append<
//             service_storage_list,
//             boost::mp11::mp_flatten<
//                 boost::mp11::mp_filter<_binding_is_service_storage, boost::mp11::mp_list<TDeps...>>
//             >
//         >;
        
    _getBindings.reserve(_getBindings.size() + boost::mp11::mp_size<service_storage_list>::value);
    
    boost::mp11::mp_for_each<boost::mp11::mp_transform<get_binder, service_storage_list>>(
        [&] (auto&& binder) { _getBindings.append(binder(injector)); }
    );
    
    _injectors.push_back(make_anonymous_unique_ptr(injector));
    
    return *this;
}

template<typename Interface, typename Impl, typename Tag, typename Value>
ModuleConfig& ModuleConfig::bindOnParamValue(const boost::parameter::keyword<Tag>&, Value&& v)
{
    static_assert(Traits::is_makeable<Interface>::value);
    
    auto bind = boost::di::bind<Interface>.template to<Impl>();
    auto injector = new di_injector (
            &_injectorConfig, 
            std::move(bind)
        );
    
    auto binding = make_binder<decltype(bind)>{} (injector);
    
    binding.condition = std::move(
        BindingConditionBuilder<config_detail::factory_params_t<Interface>, Tag> {}
            .setValue(std::forward<Value>(v))
            .result);
    
    _makeBindings.append(std::move(binding));
    _injectors.push_back(make_anonymous_unique_ptr(injector));
    
    return *this;
}

}
