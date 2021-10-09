#pragma once

#include <unordered_map>
#include <memory>
#include <typeinfo>

#include <QUuid>

#include <tuple>

#include <boost/di.hpp>
#include <boost/di/extension/injections/extensible_injector.hpp>
#include <boost/parameter.hpp>
#include <boost/mp11.hpp>
#include <boost/type_traits.hpp>

#include "interfaces/services/ifactory.hpp"

#include "./di.hpp"
#include "./extendwithparams.hpp"
#include "../config/factoryparams.hpp"

#include "utilities/metaprogramming.hpp"
#include "utilities/hashfunctions.hpp"

namespace Addle::aux_config3 {

using anonymous_unique_ptr = std::unique_ptr<void, void(*)(void*)>;
template<typename T>
void _anonymous_deleter_impl(void* obj_)
{
    delete static_cast<T*>(obj_);
}

template<typename T>
anonymous_unique_ptr make_anonymous_unique_ptr(T* obj)
{
    return anonymous_unique_ptr(obj, &_anonymous_deleter_impl<T>);
}

using make_function_ptr = void*(*)(void*, const void*);
using make_shared_function_ptr = void(*)(void*, void*, const void*);

struct make_function_binding { make_function_ptr make; void* injector; };
struct make_shared_function_binding { make_shared_function_ptr make; void* injector; };

// AddleConfig manages Addle's runtime linkage and dependency injection. 
// A newly loaded module uses AddleConfig to make its classes available to the
// rest of the program, and AddleConfig can be queried at runtime for
// information about currently available modules, and interfaces/implementing 
// classes.

class AddleConfig
{
public:
    AddleConfig() = default;
    
    AddleConfig(const AddleConfig&) = delete;
    AddleConfig(AddleConfig&&) = delete;
    
    template<class... TDeps>
    QUuid addInjector(TDeps&&... deps);
    
private:    
    struct ServiceEntry {
        mutable QAtomicPointer<void> service;
        struct { void*(*get)(void*); void* injector; } getter;
    };
    
    template<class Injector, class Interface, class Impl>
    static void* make_function_impl(void* injector_, const void* params_);
    
    template<class Injector, class Interface, class Impl>
    static void make_shared_function_impl(void* result_, void* injector_, const void* params_);

    template<class Injector, class Interface>
    static void* get_service_impl(void* injector_);
    
    template<class Interface, class Impl, class Injector>
    void addMakeFunction(Injector*);
    
    template<class Interface, class Injector>
    void addService(Injector*);
    
    template<class Interface, std::enable_if_t<
            Traits::is_makeable<Interface>::value
            && !config_detail::has_factory_params<Interface>::value,
        void*> = nullptr>
    Interface* make() const
    {
        auto it = _makeFunctions.find(typeid(Interface));
        assert(it != _makeFunctions.end());
        
        return static_cast<Interface*>((*it).second.make(
                (*it).second.injector,
                nullptr
            ));
    }
    
    template<class Interface, std::enable_if_t<
            Traits::is_makeable<Interface>::value
            && !config_detail::has_factory_params<Interface>::value,
        void*> = nullptr>
    QSharedPointer<Interface> makeShared() const
    {
        auto it = _makeSharedFunctions.find(typeid(Interface));
        assert(it != _makeSharedFunctions.end());
        
        QSharedPointer<Interface> result;
        
        (*it).second.make(
                &result,
                (*it).second.injector,
                nullptr
            );
        
        return result;
    }
    
    template<class Interface, std::enable_if_t<
            Traits::is_makeable<Interface>::value
            && config_detail::has_factory_params<Interface>::value,
        void*> = nullptr>
    Interface* make(const config_detail::factory_params_t<Interface>& params) const
    {
        auto it = _makeFunctions.find(typeid(Interface));
        assert(it != _makeFunctions.end());
        
        return static_cast<Interface*>((*it).second.make(
                (*it).second.injector,
                std::addressof(params)
            ));
    }
    
    template<class Interface, std::enable_if_t<
            Traits::is_makeable<Interface>::value
            && config_detail::has_factory_params<Interface>::value,
        void*> = nullptr>
    QSharedPointer<Interface> makeShared(const config_detail::factory_params_t<Interface>& params) const
    {
        auto it = _makeSharedFunctions.find(typeid(Interface));
        assert(it != _makeSharedFunctions.end());
        
        QSharedPointer<Interface> result;
        
        (*it).second.make(
                &result,
                (*it).second.injector,
                std::addressof(params)
            );
        
        return result;
    }
    
    template<class Interface, std::enable_if_t<
            Traits::is_singleton<Interface>::value,
        void*> = nullptr>
    Interface* getService() const
    {
        auto it = _services.find(typeid(Interface));
        assert(it != _services.end());
        
        auto result = static_cast<Interface*>((*it).second.service.loadRelaxed());
        if (Q_UNLIKELY(!result))
        {
            // getter.get is assumed to access the injector's ServiceStorage 
            // through di_service_scope. Multiple concurrent calls to getter.get 
            // should cause cause a wait on all calling threads until the 
            // service has been initialized once (unless the service is already 
            // initialized, in which case it is wait-free), and should always 
            // return the same pointer, so this call should be thread-safe 
            // without a local mutex.
            (*it).second.service.storeRelaxed((result = static_cast<Interface*>(
                    (*it).second.getter.get((*it).second.getter.injector)
                )));
        }
        
        assert(result);
        return result;
    }
    
    template<class Interface, typename F>
    Interface* getInformalService(F&& f) const
    {
        const QMutexLocker lock(&_informalServiceLock);
        
        auto it = _informalServices.find(typeid(Interface));
        if(it != _informalServices.end())
        {
            return static_cast<Interface*>((*it).second.get());
        }
        else
        {
            Interface* result;
            _informalServices.insert(std::make_pair(
                    std::type_index(typeid(Interface)),
                    make_anonymous_unique_ptr((result = std::invoke(f)))        
                ));
                
            return result;
        }
    }
    
    std::unordered_map<QUuid, anonymous_unique_ptr> _injectors;
    
    // TODO: index by QUuid so multiple make functions can be bound to the same 
    // interface type and selected with predicates
    
    std::unordered_map<std::type_index, make_function_binding> 
    _makeFunctions;
    std::unordered_map<std::type_index, make_shared_function_binding> 
    _makeSharedFunctions;
    std::unordered_map<std::type_index, ServiceEntry>
    _services;
    
    mutable std::unordered_map<std::type_index, anonymous_unique_ptr>   
    _informalServices;    
    mutable QMutex _informalServiceLock;
    
    friend struct di_runtime_config_traits<AddleConfig>;
};

template<>
struct di_runtime_config_traits<AddleConfig>
{
    template<typename I, typename... TArgs> 
    static inline I* make(const AddleConfig* config, TArgs&&... args)
    { return config->make<I>(std::forward<TArgs&&>(args)...); }
    
    template<typename I, typename... TArgs> 
    static inline QSharedPointer<I> make_shared(const AddleConfig* config, TArgs&&... args)
    { return config->makeShared<I>(std::forward<TArgs&&>(args)...); }
    
    template<typename I> 
    static inline I* get_service(const AddleConfig* config) 
    { return config->getService<I>(); }
    
    template<typename I, typename F>
    static inline I* get_informal_service(const AddleConfig* config, F&& f)
    { return config->getInformalService<I>(std::forward<F>(f)); }
};

template<class... TDeps>
QUuid AddleConfig::addInjector(TDeps&&... deps)
{    
    using interfaces_list = boost::mp11::mp_transform<dep_interface_t, 
            boost::di::core::bindings_t<TDeps...>>;
    using impl_list = boost::mp11::mp_transform<dep_impl_t, 
            boost::di::core::bindings_t<TDeps...>>;
            
    auto id = QUuid::createUuid();
        
    auto injector = new di_injector(
            this,
            std::forward<TDeps>(deps)...,
            boost::di::bind<AddleConfig>().to(this)
//             service_storage_t<TDeps...> {}
        );
    
    _injectors.insert(std::make_pair(
            id, make_anonymous_unique_ptr(injector)
        ));
    
    boost::mp11::mp_for_each<boost::mp11::mp_filter_q<
        boost::mp11::mp_bind<Traits::is_makeable,
            boost::mp11::mp_bind<boost::mp11::mp_at, interfaces_list, boost::mp11::_1>
        >,
        boost::mp11::mp_iota<boost::mp11::mp_size<interfaces_list>>
    >>( [&](auto i) {
        constexpr std::size_t I = decltype(i)::value;
        this->addMakeFunction<
                boost::mp11::mp_at_c<interfaces_list, I>,
                boost::mp11::mp_at_c<impl_list, I>
            >(injector);
    });
    
    boost::mp11::mp_for_each<boost::mp11::mp_transform<boost::mp11::mp_identity, 
        boost::mp11::mp_filter<Traits::is_singleton, interfaces_list>>
    >( [&](auto t) {
        using Interface = typename decltype(t)::type;
        this->addService<Interface>(injector);
    });
    
    return id;
}


template<class Interface, class Impl, class Injector>
void AddleConfig::addMakeFunction(Injector* injector)
{
    _makeFunctions[typeid(Interface)] = {
            &make_function_impl<Injector, Interface, Impl>,
            injector
        };
    _makeSharedFunctions[typeid(Interface)] = {
            &make_shared_function_impl<Injector, Interface, Impl>,
            injector
        };
}

template<class Interface, class Injector>
void AddleConfig::addService(Injector* injector)
{
    _services[typeid(Interface)] = {
            QAtomicPointer<void>(),
            { &get_service_impl<Injector, Interface>, injector }
        };
}

template<class Injector, class Interface, class Impl>
void* AddleConfig::make_function_impl(void* injector_, const void* params_)
{
    auto& injector = *static_cast<Injector*>(injector_);
    
    if constexpr (config_detail::has_factory_params<Interface>::value)
    {
        const auto& params = *static_cast<
            const config_detail::factory_params_t<Interface>*>(params_);   
        return extendWithParams<Interface, Impl>(injector, params)
            .template create<Impl*>();
    }
    else
    {
        return injector.template create<Impl*>();
    }
}

template<class Injector, class Interface, class Impl>
void AddleConfig::make_shared_function_impl(void* result_, void* injector_, const void* params_)
{
    auto& result = *static_cast<QSharedPointer<Interface>*>(result_);
    auto& injector = *static_cast<Injector*>(injector_);
    
    if constexpr (config_detail::has_factory_params<Interface>::value)
    {
        const auto& params = *static_cast<
            const config_detail::factory_params_t<Interface>*>(params_);   
        result = extendWithParams<Interface, Impl>(injector, params)
            .template create<QSharedPointer<Impl>>();
    }
    else
    {
        result = injector.template create<QSharedPointer<Impl>>();
    }
}

template<class Injector, class Interface>
void* AddleConfig::get_service_impl(void* injector_)
{
    auto& injector = *static_cast<Injector*>(injector_);
    auto& service = injector.template create<Interface&>();
    return std::addressof(service);
}
    
}
