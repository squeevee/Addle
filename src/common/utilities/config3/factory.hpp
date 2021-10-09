#pragma once

#include "addleconfig.hpp"
#include "../config/factoryparams.hpp"
#include "interfaces/traits.hpp"
#include "./extendwithparams.hpp"

namespace Addle::aux_config3 {

template<typename Interface, bool IsAbstract, bool HasParameters>
class Factory_impl
{
public:
    Factory_impl(const AddleConfig& coord) : _config(coord) {}
    
    template<typename... Args>
    Interface* make(Args&&... args) const
    {
        return config_detail::make_factory_param_dispatcher<Interface>(
            [&](const auto& params) {
                return di_runtime_config_traits<AddleConfig>
                    ::template make<Interface>(&_config, params);
            })(std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    QSharedPointer<Interface> makeShared(Args&&... args) const
    {
        return config_detail::make_factory_param_dispatcher<Interface>(
            [&](const auto& params) {
                return di_runtime_config_traits<AddleConfig>
                    ::template make_shared<Interface>(&_config, params);
            })(std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    std::unique_ptr<Interface> makeUnique(Args&&... args) const
    {
        return std::unique_ptr<Interface>(make(std::forward<Args>(args)...));
    }
    
private:
    const AddleConfig& _config;
};

template<typename Interface>
class Factory_impl<Interface, true, false>
{
public:
    Factory_impl(const AddleConfig& coord) : _config(coord) {}
    
    Interface* make() const
    {
        return di_runtime_config_traits<AddleConfig>
            ::template make<Interface>(&_config);
    }
    
    QSharedPointer<Interface> makeShared() const
    {
        return di_runtime_config_traits<AddleConfig>
            ::template make_shared<Interface>(&_config);
    }
    
    std::unique_ptr<Interface> makeUnique() const
    {
        return std::unique_ptr<Interface>(make());
    }
    
private:
    const AddleConfig& _config;
};

// NOTE to use factory parameters, ADDLE_DECL_FACTORY_PARAMETERS must be applied 
// to the class given as `Interface` even if it is not itself an interface.
template<typename Interface>
class Factory_impl<Interface, false, true>
{
public:
    Factory_impl(const AddleConfig& coord) : _config(coord) {}
    
    template<typename... Args>
    Interface* make(Args&&... args) const
    {
        return config_detail::make_factory_param_dispatcher<Interface>(
            [&](const auto& params) {
                auto i = di_injector(&_config);
                return extendWithParams<Interface, Interface>(i, params)
                    .template create<Interface*>();
            })(std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    QSharedPointer<Interface> makeShared(Args&&... args) const
    {
        return config_detail::make_factory_param_dispatcher<Interface>(
            [&](const auto& params) {
                auto i = di_injector(&_config);
                return extendWithParams<Interface, Interface>(i, params)
                    .template create<QSharedPointer<Interface>>();
            })(std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    std::unique_ptr<Interface> makeUnique(Args&&... args) const
    {
        return std::unique_ptr<Interface>(make(std::forward<Args>(args)...));
    }
    
private:
    const AddleConfig& _config;
};

template<typename Interface>
class Factory_impl<Interface, false, false>
{
public:
    Factory_impl(const AddleConfig& coord) : _config(coord) {}
    
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
    const AddleConfig& _config;
};

template<typename Interface>
class Factory : public Factory_impl<Interface, 
        std::is_abstract_v<Interface>,
        config_detail::has_factory_params<Interface>::value
    >
{
    using base_t = Factory_impl<Interface, 
            std::is_abstract_v<Interface>,
            config_detail::has_factory_params<Interface>::value
        >;
    static_assert(
        std::is_class_v<Interface>
        && std::is_same_v<Interface, boost::remove_cv_ref_t<Interface>>
        && !std::is_pointer_v<boost::remove_cv_ref_t<Interface>>
        && boost::is_complete<Interface>::value
    );
public:
    Factory(const AddleConfig& config) : base_t(config) {}
};

}
