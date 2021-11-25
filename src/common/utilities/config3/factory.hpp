#pragma once 

#include "interfaces/traits.hpp"

#include "./di.hpp"
#include "./interfacebindingconfig.hpp"
#include "../config/factoryparams.hpp"

namespace Addle {
namespace aux_config3 {

template<typename Interface
    , bool = std::is_abstract_v<Interface>
    , bool = config_detail::has_factory_params<Interface>::value>
class Factory_impl
{
public:
    using params_t = config_detail::factory_params_t<Interface>;
    
    Factory_impl(const InterfaceBindingConfig& config) : _config(config) {}
    
    Interface* make(params_t params) const
    {
        return di_runtime_bindings_traits<InterfaceBindingConfig>
            ::template make<Interface>(&_config, std::move(params));
    }
    
    template<typename... Args,
        std::enable_if_t<
            config_detail::is_makeable_with_params<Interface, Args...>::value
        , void*> = nullptr>
    Interface* make(Args&&... args) const
    {
        return config_detail::make_factory_param_dispatcher<Interface>(
                [&](params_t&& params) { return make(std::move(params)); }
            )(std::forward<Args>(args)...);
    }
    
    QSharedPointer<Interface> makeShared(params_t params) const
    {
        return di_runtime_bindings_traits<InterfaceBindingConfig>
            ::template make_shared<Interface>(&_config, std::move(params));
    }
    
    template<typename... Args,
        std::enable_if_t<
            config_detail::is_makeable_with_params<Interface, Args...>::value
        , void*> = nullptr>
    QSharedPointer<Interface> makeShared(Args&&... args) const
    {
        return config_detail::make_factory_param_dispatcher<Interface>(
                [&](params_t&& params) { return makeShared(std::move(params)); }
            )(std::forward<Args>(args)...);
    }
    
    std::unique_ptr<Interface> makeUnique(params_t params) const
    {
        return std::unique_ptr<Interface>(make(std::move(params)));
    }
    
    template<typename... Args,
        std::enable_if_t<
            config_detail::is_makeable_with_params<Interface, Args...>::value
        , void*> = nullptr>
    std::unique_ptr<Interface> makeUnique(Args&&... args) const
    {
        return std::unique_ptr<Interface>(make(std::forward<Args>(args)...));
    }
    
private:
    const InterfaceBindingConfig& _config;
};

template<typename Interface>
class Factory_impl<Interface, true, false>
{
public:
    Factory_impl(const InterfaceBindingConfig& coord) : _config(coord) {}
    
    Interface* make() const
    {
        return di_runtime_bindings_traits<InterfaceBindingConfig>
            ::template make<Interface>(&_config);
    }
    
    QSharedPointer<Interface> makeShared() const
    {
        return di_runtime_bindings_traits<InterfaceBindingConfig>
            ::template make_shared<Interface>(&_config);
    }
    
    std::unique_ptr<Interface> makeUnique() const
    {
        return std::unique_ptr<Interface>(make());
    }
    
private:
    const InterfaceBindingConfig& _config;
};

// NOTE to use factory parameters, ADDLE_DECL_FACTORY_PARAMETERS must be applied 
// to the class given as `Interface` even if it is not itself an interface.
template<typename Interface>
class Factory_impl<Interface, false, true>
{
public:
    using params_t = config_detail::factory_params_t<Interface>;
    Factory_impl(const InterfaceBindingConfig& coord) : _config(coord) {}
    
    Interface* make(params_t params)
    {
        constexpr bool UseNames = boost::mp11::mp_apply<boost::mp11::mp_any,
                boost::mp11::mp_transform<is_named, ctor_list<Interface>>
            >::value;
            
        auto baseInjector = di_injector(&_config);
        return bindFactoryParams<UseNames, params_t>::extend(baseInjector, std::move(params))
            .template create<Interface*>();
    }
    
    template<typename... Args,
        std::enable_if_t<
            config_detail::is_makeable_with_params<Interface, Args...>::value
        , void*> = nullptr>
    Interface* make(Args&&... args) const
    {
        return config_detail::make_factory_param_dispatcher<Interface>(
                [&](params_t&& params) { return make(std::move(params)); }
            )(std::forward<Args>(args)...);
    }
    
    QSharedPointer<Interface> makeShared(params_t params)
    {
        constexpr bool UseNames = boost::mp11::mp_apply<boost::mp11::mp_any,
                boost::mp11::mp_transform<is_named, ctor_list<Interface>>
            >::value;
            
        auto baseInjector = di_injector(&_config);
        return bindFactoryParams<UseNames, params_t>::extend(baseInjector, std::move(params))
            .template create<QSharedPointer<Interface>>();
    }
    
    template<typename... Args,
        std::enable_if_t<
            config_detail::is_makeable_with_params<Interface, Args...>::value
        , void*> = nullptr>
    QSharedPointer<Interface> makeShared(Args&&... args) const
    {
        return config_detail::make_factory_param_dispatcher<Interface>(
                [&](params_t&& params) { return makeShared(std::move(params)); }
            )(std::forward<Args>(args)...);
    }
    
    std::unique_ptr<Interface> makeUnique(params_t params)
    {
        return std::unique_ptr<Interface>(make(std::move(params)));
    }
    
    template<typename... Args,
        std::enable_if_t<
            config_detail::is_makeable_with_params<Interface, Args...>::value
        , void*> = nullptr>
    std::unique_ptr<Interface> makeUnique(Args&&... args) const
    {
        return std::unique_ptr<Interface>(make(std::forward<Args>(args)...));
    }
    
private:
    const InterfaceBindingConfig& _config;
};

template<typename Interface>
class Factory_impl<Interface, false, false>
{
public:
    Factory_impl(const InterfaceBindingConfig& coord) : _config(coord) {}
    
    Interface* make() const
    {
        return di_injector(&_config).template create<Interface*>();
    }
    
    QSharedPointer<Interface> makeShared() const
    {
        return di_injector(&_config).template create<QSharedPointer<Interface>>();
    }
    
    std::unique_ptr<Interface> makeUnique() const
    {
        return std::unique_ptr<Interface>(make());
    }
    
private:
    const InterfaceBindingConfig& _config;
};

}

template<typename Interface>
class Factory : public aux_config3::Factory_impl<Interface>
{
    static_assert(
        std::is_class_v<Interface>
        && std::is_same_v<Interface, boost::remove_cv_ref_t<Interface>>
        && !std::is_pointer_v<boost::remove_cv_ref_t<Interface>>
        && boost::is_complete<Interface>::value
    );
public:
    Factory(const aux_config3::InterfaceBindingConfig& config) 
        : aux_config3::Factory_impl<Interface>(config) {}
};

}
