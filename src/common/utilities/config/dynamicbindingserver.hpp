/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#pragma once

#include <any>
#include <unordered_map>
#include <tuple>
#include <typeindex>
#include <vector>

#include "interfaces/services/ifactory.hpp"

namespace Addle::config_detail {

class DynamicBindingServer
{
public:
    DynamicBindingServer() = default;
    DynamicBindingServer(const DynamicBindingServer&) = delete;
    DynamicBindingServer(DynamicBindingServer&&) = delete;
    
    template<typename Interface>
    void bind(std::function<Interface* (init_params_placeholder_t<Interface>)>&& make_function)
    {
        auto& interfaceBindings = _bindings[typeid(Interface)];
        interfaceBindings.push_back( binding_info { {}, std::move(make_function) } );
    }
    
    template<typename Interface>
    Interface* make(init_params_placeholder_t<Interface> params) const
    {
        auto i = _bindings.find(typeid(Interface));
        if (i == _bindings.end())
        {
            // TODO: better messaging
            assert(false);
        }
        
        for (const auto& binding : (*i).second)
        {
            if (binding.filter.has_value())
            {
                //TODO
            }
            
            return std::any_cast< 
                std::function<Interface* (init_params_placeholder_t<Interface>)>
            >(binding.make_function)(params);
        }
        
        assert(false); // TODO 
    }
    
    template<typename Interface>
    std::shared_ptr<IFactory<Interface>> getFactory() const
    {
        auto i = _factories.find(typeid(Interface));
        if (i == _factories.end())
        {
            //assert binding is available
            
            auto factory = std::make_shared<DynamicFactory<Interface>>(*this);
            _factories[typeid(Interface)] = factory;
            
            return factory;
        }
        else
        {
            return std::any_cast< std::shared_ptr<IFactory<Interface>> >((*i).second);
        }
    }
    
private:
    struct binding_info
    {
        std::any filter;
        std::any make_function;
    };
    
    template<typename Interface>
    class DynamicFactory : public IFactory<Interface>
    {
    public:
        DynamicFactory(const DynamicBindingServer& dynamicBindings)
            : _dynamicBindings(dynamicBindings)
        {
        }
        
        Interface* make_p(const config_detail::init_params_placeholder_t<Interface>& params) const
        {
            return _dynamicBindings.make<Interface>(params);
        }
        
    private:
        const DynamicBindingServer& _dynamicBindings;
    };
    
    std::unordered_map<std::type_index, std::vector<binding_info>> _bindings;
    mutable std::unordered_map<std::type_index, std::any> _factories;
    
    template<typename...> friend class CoreConfig;
};

} // namespace Addle::config_detail
