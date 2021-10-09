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
#include <boost/mp11.hpp>

#include <QSharedPointer>

#include "interfaces/traits.hpp"

#include "./servicestorage.hpp"

template<typename T>
struct boost::di::aux::deref_type<QSharedPointer<T>> { using type = boost::di::aux::decay_t<T>; };

template<typename T>
struct boost::di::ctor_traits<QSharedPointer<T>> { using type = boost::di::inject<>; };

template <class T, class U>
struct boost::di::type_traits::rebind_traits<QSharedPointer<T>, U> { using type = QSharedPointer<U>; };

template <class T, class TName, class _>
struct boost::di::type_traits::rebind_traits<QSharedPointer<T>, boost::di::named<TName, _>> { using type = named<TName, QSharedPointer<T>>; };

namespace Addle::aux_config3 {

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

template<typename... TDeps>
using service_storage_t = boost::mp11::mp_apply<ServiceStorage,
    boost::mp11::mp_transform_q<
        boost::mp11::mp_bind<boost::mp11::mp_list, 
            boost::mp11::mp_bind<dep_interface_t, boost::mp11::_1>, 
            boost::mp11::mp_bind<dep_impl_t, boost::mp11::_1>
        >,
        boost::mp11::mp_filter_q<
            boost::mp11::mp_compose<dep_interface_t, Traits::is_singleton>,
            boost::di::core::bindings_t<TDeps...>
        >
    >>;

template<class RuntimeConfig>
struct di_runtime_config_traits
{
    // template<typename I> static I* make(const RuntimeConfig*, ...);
    // template<typename I> static I* make_shared(const RuntimeConfig*, ...);
    // template<typename I> static I* get_service(const RuntimeConfig*);
    // template<typename I, typename F> static I* get_informal_service(const RuntimeConfig*, F&&);
};

template<class RuntimeConfig>
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
            return QSharedPointer<T>::create(std::forward<TArgs>(args)...);
        else
            return this->runtime_make_shared<T>();
    }
    
    template<class T, std::enable_if_t<
            Traits::is_makeable<T>::value
            && !config_detail::has_factory_params<T>::value
        , void*> = nullptr>
    inline T* runtime_make() const
    {
        return di_runtime_config_traits<RuntimeConfig>::template make<T>(runtimeConfig);
    }
    
    template<class T, std::enable_if_t<
            Traits::is_makeable<T>::value
            && config_detail::has_factory_params<T>::value
        , void*> = nullptr>
    inline T* runtime_make() const
    {
        return config_detail::make_factory_param_dispatcher<T>(
            std::bind(&di_runtime_config_traits<RuntimeConfig>
                    ::template make<T, const config_detail::factory_params_t<T>&>,
                runtimeConfig,
                std::placeholders::_1))();
    }
    
    template<class T, std::enable_if_t<
            Traits::is_singleton<T>::value
            && !Traits::is_makeable<T>::value
        , void*> = nullptr>
    inline T* runtime_make() const
    {
        return di_runtime_config_traits<RuntimeConfig>::template get_service<T>(runtimeConfig);
    }
    
    template<class T, std::enable_if_t<!config_detail::has_factory_params<T>::value, void*> = nullptr>
    inline QSharedPointer<T> runtime_make_shared() const
    {
        static_assert(Traits::is_makeable<T>::value);
        return di_runtime_config_traits<RuntimeConfig>::template make_shared<T>(runtimeConfig);
    }
    
    template<class T, std::enable_if_t<config_detail::has_factory_params<T>::value, void*> = nullptr>
    inline QSharedPointer<T> runtime_make_shared() const
    {
        static_assert(Traits::is_makeable<T>::value);
        return config_detail::make_factory_param_dispatcher<T>(
            std::bind(&di_runtime_config_traits<RuntimeConfig>
                    ::template make_shared<T, const config_detail::factory_params_t<T>&>,
                runtimeConfig,
                std::placeholders::_1))();
    }
    
    template<class T>
    inline T* runtime_get_singleton() const
    {
        static_assert(Traits::is_singleton<T>::value);
        return di_runtime_config_traits<RuntimeConfig>::template get_singleton<T>(runtimeConfig);
    }
    
    di_provider(const RuntimeConfig* config_)
        : runtimeConfig(config_)
    {
    }
    
    const RuntimeConfig* runtimeConfig;
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
        
        template <class TProvider>
        using _get_ref_t = decltype(*(std::declval<TProvider>().get()));
        
        template<class TProvider>
        using service_storage_t = typename boost::remove_cv_ref_t<
                decltype(std::declval<TProvider>().cfg())
            >::service_storage_t;
        
        template <class T, class, class TProvider>
        static std::enable_if_t<
                std::is_reference<_get_ref_t<TProvider>>::value,
                boost::di::wrappers::shared<di_service_scope, _get_ref_t<TProvider>>>
            try_create(const TProvider&);
        
        template <class, class, class TProvider, 
            std::enable_if_t<boost::mp11::mp_valid<service_storage_t, TProvider>::value,
            void*> = nullptr>
        auto create(const TProvider& provider) const 
        {
            using wrapper = boost::di::wrappers::shared<di_service_scope, 
                _get_ref_t<TProvider>>;
            using service_storage_t = service_storage_t<TProvider>;
            
            if constexpr (service_storage_t::template contains<TExpected>::value)
            {
                auto result = provider.cfg().template getService<TExpected>();
                if (Q_UNLIKELY(!result))
                {
                    std::tie(result, std::ignore) = provider.cfg()
                        .template initializeService<TExpected>(
                            [&] () { return provider.get(); }
                        );
                }
                
                assert(result);
                return wrapper { *result };
            }
            else if constexpr (Traits::is_singleton<TExpected>::value)
            {
                return wrapper { *(provider.get()) };
            }
            else
            {
                auto* runtimeConfig = provider.cfg().runtimeConfig;
                auto result = di_runtime_config_traits<
                        std::remove_cv_t<std::remove_pointer_t<decltype(runtimeConfig)>>
                    >::template get_informal_service<TExpected>(
                        runtimeConfig, [&] { return provider.get(); }
                    );
                    
                assert(result);
                return wrapper { *result };
            }
        }
        
        template <class, class, class TProvider, 
            std::enable_if_t<!boost::mp11::mp_valid<service_storage_t, TProvider>::value,
            void*> = nullptr>
        auto create(const TProvider& provider) const
            -> boost::di::wrappers::shared<di_service_scope, _get_ref_t<TProvider>>;
        // a bit hackish, this exists so this class fulfills the "scopable" 
        // concept, to enable `.in(di_service_scope {})` in DI bind expressions, 
        // but is undefined as service scope is not actually usable with a 
        // generic provider.
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

template<class ServiceStorage_, class RuntimeConfig>
struct di_cfg : boost::di::config, ServiceStorage_
{
    using service_storage_t = ServiceStorage_;
    
    template<typename T>
    using memory_traits = di_memory_traits<T>;
    
    template<typename T>
    using scope_traits = di_scope_traits<T>;
    
    auto provider(...) noexcept { return di_provider(runtimeConfig); }
    
    const RuntimeConfig* runtimeConfig = nullptr;
};

#ifdef ADDLE_DEBUG
#define ADDLE_DI_POLICIES boost::di::core::pool< \
    >
#else
#define ADDLE_DI_POLICIES boost::di::core::pool<>
#endif

template<class RuntimeConfig, class... TDeps>
struct di_injector : public boost::di::core::injector<di_cfg<service_storage_t<TDeps...>, RuntimeConfig>, ADDLE_DI_POLICIES, TDeps...>
{
    using cfg_t = di_cfg<service_storage_t<TDeps...>, RuntimeConfig>;
    using base_injector_t = boost::di::core::injector<cfg_t, ADDLE_DI_POLICIES, TDeps...>;
    using base_injector_t::is_creatable;
        
    di_injector(const RuntimeConfig* runtimeConfig, TDeps&&... deps) noexcept
        : base_injector_t { boost::di::core::init {}, std::forward<TDeps>(deps)... }
    {
        static_cast<cfg_t&>(this->cfg()).runtimeConfig = runtimeConfig;
    }
    
    const RuntimeConfig* runtimeConfig() const
    { 
        return static_cast<const cfg_t&>(this->cfg()).runtimeConfig; 
    }
};

}

#undef ADDLE_DI_POLICIES
