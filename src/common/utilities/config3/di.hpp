/**
 * Extensions to [Boost::ext].DI supporting:
 * - hybrid compile-time/runtime bindings coordinated through AddleConfig
 * - QSharedPointer support
 * - A new "service" scope, similar to singleton but with the objects owned by
 * the injector rather than having static lifetime
 */

#pragma once

#include <type_traits>

#include <boost/di.hpp> 
#include <boost/di/extension/injections/extensible_injector.hpp>
#include <boost/mp11.hpp>

#include <QSharedPointer>

#include "interfaces/traits.hpp"

#include "../config/factoryparams.hpp"
#include "utilities/metaprogramming.hpp"

template<typename T>
struct boost::di::aux::deref_type<QSharedPointer<T>> { using type = boost::di::aux::decay_t<T>; };

template<typename T>
struct boost::di::ctor_traits<QSharedPointer<T>> { using type = boost::di::inject<>; };

template <class T, class U>
struct boost::di::type_traits::rebind_traits<QSharedPointer<T>, U> { using type = QSharedPointer<U>; };

template <class T, class TName, class _>
struct boost::di::type_traits::rebind_traits<QSharedPointer<T>, boost::di::named<TName, _>> { using type = named<TName, QSharedPointer<T>>; };

template <class T>
struct boost::di::scopes::detail::wrapper_traits<QSharedPointer<T>>;

namespace Addle::aux_config3 {
    
class di_service_storage_base; // defined in servicestorage.hpp

template<typename TDep> using dep_interface_t   = typename TDep::expected;
template<typename TDep> using dep_impl_t        = typename TDep::given;

template<typename> struct is_named : std::false_type {};
template<typename Name, typename T> struct is_named<boost::di::named<Name, T>> : std::true_type {};

template<typename T>
using _ctor_list_impl_1 = typename T::boost_di_inject__::type;

template<typename T>
using _ctor_list_impl_2 = _ctor_list_impl_1<boost::di::ctor_traits<T>>;

template<typename T>
using ctor_list = boost::mp11::mp_eval_or<
        boost::mp11::mp_eval_or<boost::di::inject<>, _ctor_list_impl_1, T>,
        _ctor_list_impl_2, T
    >;

// memory trait indicating an object should be allocated and managed as a 
// QSharedPointer
struct di_qshared_memory /*: boost::di::type_traits::heap*/ {};

template<typename T> struct remove_qsharedpointer { using type = T; };
template<typename T> struct remove_qsharedpointer<QSharedPointer<T>> { using type = T; };

template<class> struct di_runtime_bindings_traits;

template<class RuntimeBindings>
struct di_provider : boost::di::providers::stack_over_heap
{
    using base_provider_t = boost::di::providers::stack_over_heap;
    
    template <class TInit, class T, class... TArgs>
    using base_is_creatable = base_provider_t::is_creatable<TInit, T, TArgs...>;
    
    template <class TInit, class T, class... TArgs>
    using is_creatable = boost::mp11::mp_or<
            base_is_creatable<TInit, T, TArgs...>,
            Traits::is_makeable<T>,
            Traits::is_singleton<T>
        >;
    
    template<class T, class TInit, class TMem, class... TArgs>
    auto get(const TInit& init, const TMem& mem, TArgs&&... args) const
    {
        if constexpr (base_is_creatable<TInit, T, TArgs...>::value)
            return base_provider_t::get<T>(init, mem, std::forward<TArgs>(args)...);
        else
            return this->runtime_make<T>();
    }
    
    template<class T, class TInit, class... TArgs>
    auto get(const TInit&, const di_qshared_memory&, TArgs&&... args) const
    {
        if constexpr (base_is_creatable<TInit, T, TArgs...>::value)
        {
            // TODO: For QObject classes, it'd be better to create shared 
            // pointers that use QObject::deleteLater rather than the default 
            // deleter. This would eliminate the possibility of certain races 
            // between the event loop and destructors of shared objects.
            // 
            // This could have unintended side-effects, e.g., in tests it may be
            // necessary to call `QCoreApplication::sendPostedEvents` before any
            // destructors can be observed.
            
            return QSharedPointer<T>::create(std::forward<TArgs>(args)...);
        }
        else
        {
            return this->runtime_make_shared<T>();
        }
    }
    
    template<class T, std::enable_if_t<
            Traits::is_makeable<T>::value && !config_detail::has_factory_params<T>::value
        , void*> = nullptr>
    inline T* runtime_make() const
    {
        return di_runtime_bindings_traits<RuntimeBindings>::template make<T>(runtimeBindings);
    }
    
    template<class T, std::enable_if_t<
            Traits::is_makeable<T>::value && config_detail::has_factory_params<T>::value
        , void*> = nullptr>
    inline T* runtime_make() const
    {
        return config_detail::make_factory_param_dispatcher<T>(
            [&] (auto&& params) {
                return di_runtime_bindings_traits<RuntimeBindings>::template make<T>(runtimeBindings, std::move(params));
            })();
    }
    
//     template<class T, std::enable_if_t<
//             Traits::is_singleton<T>::value
//             && !Traits::is_makeable<T>::value,
//         void*> = nullptr>
//     inline T* runtime_make() const
//     {
//         return di_runtime_bindings_traits<RuntimeBindings>
//             ::template get_service<T>(RuntimeBindings);
//     }
    
    template<class T, std::enable_if_t<
            Traits::is_makeable<T>::value
            && !config_detail::has_factory_params<T>::value, 
        void*> = nullptr>
    inline QSharedPointer<T> runtime_make_shared() const
    {
        return di_runtime_bindings_traits<RuntimeBindings>::template make_shared<T>(runtimeBindings);
    }
    
    template<class T, std::enable_if_t<
            Traits::is_makeable<T>::value
            && config_detail::has_factory_params<T>::value, 
        void*> = nullptr>
    inline QSharedPointer<T> runtime_make_shared() const
    {
        return config_detail::make_factory_param_dispatcher<T>(
            [&] (auto&& params) {
                return di_runtime_bindings_traits<RuntimeBindings>
                    ::template make_shared<T>(runtimeBindings, std::move(params));
            })();
    }
    
//     template<class T>
//     inline T* runtime_get_singleton() const
//     {
//         static_assert(Traits::is_singleton<T>::value);
//         return di_runtime_bindings_traits<RuntimeBindings>::template get_singleton<T>(runtimeBindings);
//     }
    
    di_provider(const RuntimeBindings* runtimeBindings_)
        : runtimeBindings(runtimeBindings_)
    {
    }
    
    const RuntimeBindings* runtimeBindings;
};

// This scope and wrapper mirror boost::di:scopes::unique but allowing use of
// QSharedPointer with the same conversion semantics of a plain pointer

template<class TScope, typename T>
struct di_qshared_wrapper
{
    template<typename U, std::enable_if_t<std::is_convertible_v<T*, U*>, void*> = nullptr>
    inline operator QSharedPointer<U> () const & noexcept 
    {
        return pointer;
    }
    
    template<typename U, std::enable_if_t<std::is_convertible_v<T*, U*>, void*> = nullptr>
    inline operator QSharedPointer<U> () && noexcept 
    {
        return std::move(pointer);
    }
    
    QSharedPointer<T> pointer;
};

struct di_qshared_scope : boost::di::scopes::unique
{
    template<class TExpected, class TGiven>
    struct scope : boost::di::scopes::unique::scope<TExpected, TGiven>
    {
        template <class T, class, class TProvider>
        static std::enable_if_t<
                TProvider::template is_creatable<di_qshared_memory>::value, T
            > try_create(const TProvider&);
        
        template <class, class, class TProvider>
        auto create(const TProvider& provider) const 
        {
            using wrapper = di_qshared_wrapper<di_qshared_scope, 
                    boost::di::aux::decay_t<decltype(provider.get(di_qshared_memory{}))>
                >;
            
            return wrapper { provider.get(di_qshared_memory {}) };
        }
    };
};

struct di_service_scope
{
    template<class TExpected, class TGiven>
    struct scope
    {
        template<class, class>
        using is_referable = std::true_type;
                
        template<class TProvider>
        using runtime_config_t = decltype(std::declval<TProvider>().cfg().runtimeBindings);
        using _wrapper_t = boost::di::wrappers::shared<di_service_scope, TExpected&>;
        
        template<class ServiceStorage_>
        using _contains_expected = typename ServiceStorage_::template contains<TExpected>;
            
        template<class TProvider>
        using service_storage_t = mp_only<
            boost::mp11::mp_filter<
                _contains_expected,
                boost::mp11::mp_filter_q<
                    boost::mp11::mp_bind<std::is_base_of, di_service_storage_base, boost::mp11::_1>,
                    boost::mp11::mp_transform<dep_impl_t, typename TProvider::injector::deps>
                >
            >>;
            
        //,
//             std::enable_if_t<
//                    boost::mp11::mp_valid<runtime_config_t, TProvider>::value
//                 && (
//                        boost::mp11::mp_valid<service_storage_t, TProvider>::value
//                     || Traits::is_singleton<TExpected>::value
//                     || !std::is_abstract_v<TExpected>
//                 )
//             , void*> = nullptr>
        template <class, class, class TProvider>
        static _wrapper_t try_create(const TProvider&);
        
        template <class, class, class TProvider, 
            std::enable_if_t<
                boost::mp11::mp_valid<runtime_config_t, TProvider>::value
                && boost::mp11::mp_valid<service_storage_t, TProvider>::value
            , void*> = nullptr>
        auto create(const TProvider& provider) const 
        {
            auto&& serviceStorage = provider.super()
                .template create<service_storage_t<TProvider>>();
            
            auto result = serviceStorage.template getService<TExpected>();
            if (Q_UNLIKELY(!result))
            {
                std::tie(result, std::ignore) = serviceStorage
                    .template initializeService<TExpected>(
                        [&] () { return provider.get(); }
                    );
            }
            
            assert(result);
            return _wrapper_t( static_cast<TExpected&>(*result) );
        }
        
        template <class, class, class TProvider, 
            std::enable_if_t<
                boost::mp11::mp_valid<runtime_config_t, TProvider>::value
                && !boost::mp11::mp_valid<service_storage_t, TProvider>::value
                && std::is_abstract_v<TExpected>
                && Traits::is_singleton<TExpected>::value
            , void*> = nullptr>
        auto create(const TProvider& provider) const 
        {
            using _bindings_traits = di_runtime_bindings_traits< 
                    std::remove_cv_t<std::remove_pointer_t< runtime_config_t<TProvider> >> 
                >;
            runtime_config_t<TProvider> runtimeBindings = provider.cfg().runtimeBindings;
            
            auto result = _bindings_traits::template get_service<TExpected>(runtimeBindings);
            
            assert(result);
            return _wrapper_t{ static_cast<TExpected&>(*result) };
        }
        
        template <class, class, class TProvider, 
            std::enable_if_t<
                boost::mp11::mp_valid<runtime_config_t, TProvider>::value
                && !boost::mp11::mp_valid<service_storage_t, TProvider>::value
                && !std::is_abstract_v<TExpected>
            , void*> = nullptr>
        auto create(const TProvider& provider) const 
        {
            using _bindings_traits = di_runtime_bindings_traits< 
                    std::remove_cv_t<std::remove_pointer_t< runtime_config_t<TProvider> >> 
                >;
            runtime_config_t<TProvider> runtimeBindings = provider.cfg().runtimeBindings;
            
            auto result = _bindings_traits::template get_or_initialize_service<TExpected>(
                    runtimeBindings, [&]() { return provider.get(); }
                );
            
            assert(result);
            return _wrapper_t{ static_cast<TExpected&>(*result) };
        }
        
//         template <class, class, class TProvider, 
//             std::enable_if_t<!boost::mp11::mp_valid<runtime_config_t, TProvider>::value,
//             void*> = nullptr>
//         auto create(const TProvider& provider) const -> _wrapper_t;
//         // a bit hackish, this exists so this class fulfills the "scopable" 
//         // concept, to enable `.in(di_service_scope {})` in DI bind expressions, 
//         // but is not defined.
    };
};

template<typename T>
struct di_memory_traits : boost::di::type_traits::memory_traits<T> {};

template<typename T>
struct di_memory_traits<QSharedPointer<T>> { using type = di_qshared_memory; };

template<typename T>
struct di_scope_traits
{ 
    using type = boost::mp11::mp_if<
        Traits::is_singleton<std::remove_cv_t<std::remove_pointer_t<T>>>,
        di_service_scope,
        boost::di::scopes::unique>;
};

template<typename T>
struct di_scope_traits<QSharedPointer<T>> 
{ 
    using type = boost::mp11::mp_if<
        Traits::is_singleton<std::remove_cv_t<T>>,
        di_service_scope,
        di_qshared_scope>;
};

template<typename T>
struct di_scope_traits<T&> { using type = di_service_scope; };

template<class RuntimeBindings>
struct di_cfg : boost::di::config
{
    using runtime_config_t = RuntimeBindings;
    
    template<typename T>
    using memory_traits = di_memory_traits<T>;
    
    template<typename T>
    using scope_traits = di_scope_traits<T>;
    
    auto provider(...) noexcept { return di_provider(runtimeBindings); }
    
    const RuntimeBindings* runtimeBindings = nullptr;
};

#ifdef ADDLE_DEBUG
#define ADDLE_DI_POLICIES boost::di::core::pool< \
    >
#else
#define ADDLE_DI_POLICIES boost::di::core::pool<>
#endif

template<class RuntimeBindings>
using _runtime_bindings_dep_t = boost::di::core::dependency<boost::di::scopes::instance, RuntimeBindings, const RuntimeBindings&>;

template<class RuntimeBindings, class... TDeps>
struct di_injector : public boost::di::core::injector<di_cfg<RuntimeBindings>, 
    ADDLE_DI_POLICIES, _runtime_bindings_dep_t<RuntimeBindings>, TDeps...>
{
    using cfg_t = di_cfg<RuntimeBindings>;
    using base_injector_t = boost::di::core::injector<cfg_t, ADDLE_DI_POLICIES, 
        _runtime_bindings_dep_t<RuntimeBindings>, TDeps...>;
    using base_injector_t::is_creatable;
        
    di_injector(const RuntimeBindings* runtimeBindings, TDeps&&... deps) noexcept
        : base_injector_t { 
            boost::di::core::init {}, 
            boost::di::bind<RuntimeBindings>.to(*runtimeBindings),
            std::forward<TDeps>(deps)... 
        }
    {
        static_cast<cfg_t&>(this->cfg()).runtimeBindings = runtimeBindings;
    }
    
    const RuntimeBindings* runtimeBindings() const
    { 
        return static_cast<const cfg_t&>(this->cfg()).runtimeBindings; 
    }
};

#undef ADDLE_DI_POLICIES

template<bool, typename>
struct bindFactoryParams;

template<bool UseNames, template<typename...> class L, typename... ParamList>
struct bindFactoryParams<UseNames, L<ParamList...>>
{
    using params_t = L<ParamList...>;
    
    template<typename ParamEntry>
    static auto _entry(params_t&& params)
    {
        using value_t = typename ParamEntry::value_type;
        
        static_assert(!std::is_pointer_v<std::remove_pointer_t<value_t>>
            && !std::is_pointer_v<std::remove_reference_t<value_t>>);
        // I doubt DI (as is) has a way to pass pointers-to-pointers or 
        // references-to-pointers as factory parameters, and in the (quite 
        // unlikely) case we need something like that, I'm sure we can work 
        // around this limitation with a wrapper class.
        
        using bound_t = boost::mp11::mp_if<
                std::is_pointer<value_t>,
                std::remove_cv_t<std::remove_pointer_t<value_t>>,
                value_t
            >;
        
        const auto& keyword = boost::parameter::keyword<typename ParamEntry::tag_type>::instance;
        auto&& value = params[keyword];
        
        if constexpr (UseNames)
            return boost::di::bind<bound_t>().to(static_cast<value_t&&>(value)).named(keyword)[boost::di::override];
        else
            return boost::di::bind<bound_t>().to(static_cast<value_t&&>(value))[boost::di::override];
    }
    
    template<typename Injector, typename... TExtraDeps>
    static auto extend(Injector& injector, params_t&& params, TExtraDeps&&... extraDeps)
    {
        return boost::di::make_injector<typename Injector::config>(
                boost::di::extension::make_extensible(injector),
                _entry<ParamList>(std::move(params))...,
                std::forward<TExtraDeps>(extraDeps)...
            );
    }
};



}

template <class T>
struct boost::di::scopes::detail::wrapper_traits<QSharedPointer<T>> 
{ using type = Addle::aux_config3::di_qshared_wrapper<instance, T>; };
