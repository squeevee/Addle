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

#include "bindingfilter.hpp"
#include "interfaces/services/ifactory.hpp"

namespace Addle::config_detail {

class DynamicBindingServer
{
public:
    DynamicBindingServer() = default;
    DynamicBindingServer(const DynamicBindingServer&) = delete;
    DynamicBindingServer(DynamicBindingServer&&) = delete;
    
    template<typename Interface>
    void deferBinding()
    {
        // TODO: debug checking
    }
    
    template<typename Interface>
    void fillDeferredBinding(std::shared_ptr<IFactory<Interface>> factory)
    {
        // TODO: checking
        _deferredBindings[typeid(Interface)] = factory;
    }
    
    template<typename Interface>
    void deferConditionalBinding()
    {
        // TODO: debug checking
    }
        
    template<typename Interface>
    void fillConditionalBinding(binding_filter_t<Interface>&& filter, std::shared_ptr<IFactory<Interface>> factory)
    {
        // TODO: checking
        _conditionalBindings[typeid(Interface)].push_back(
            ConditionalBindingData { std::move(filter), factory }
        );
    }
    
    template<typename Interface>
    std::shared_ptr<IFactory<Interface>> getDelegateFactory() const
    {
        auto i = _delegateFactories.find(typeid(Interface));
        if (i == _deferredBindings.end())
        {
            std::shared_ptr<IFactory<Interface>> factory 
                = std::make_shared<DelegateFactory<Interface>>(*this);
            
            _delegateFactories[typeid(Interface)] = factory;
            
            return factory;
        }
        else
        {
            return std::any_cast<
                std::shared_ptr<IFactory<Interface>>
            >((*i).second);
        }
    }
    
    template<typename Interface>
    typename std::enable_if<
        !has_factory_params<Interface>::value,
        Interface*
    >::type delegate_make() const
    {
        auto i = _deferredBindings.find(typeid(Interface));
        if (i != _deferredBindings.end())
        {
            const auto& binding = (*i).second;
            assert(binding.has_value());
            
            return std::any_cast<
                std::shared_ptr<IFactory<Interface>>
            >(binding)->make();
        }
        
        // hint: interface was not bound
        assert(false); // TODO: messaging 
    }
    
    template<typename Interface>
    typename std::enable_if<
        has_factory_params<Interface>::value,
        Interface*
    >::type delegate_make(const factory_params_t<Interface>& params) const
    {
        {
            auto i = _deferredBindings.find(typeid(Interface));
            if (i != _deferredBindings.end())
            {
                const auto& binding = (*i).second;
                assert(binding.has_value());
                
                return std::any_cast<
                    std::shared_ptr<IFactory<Interface>>
                >(binding)->make(params);
            }
        }
        
        {
            auto i = _conditionalBindings.find(typeid(Interface));
            if (i != _conditionalBindings.end())
            {
                const auto& bindingDataSet = (*i).second;
                assert(!bindingDataSet.empty());
                
                for (auto& bindingData : bindingDataSet)
                {
                    const auto& filter = std::any_cast<
                        binding_filter_t<Interface>
                    >(bindingData.filter);
                    
                    const auto& factory = std::any_cast<
                        std::shared_ptr<IFactory<Interface>>
                    >(bindingData.factory);
                    
                    if (filter(params))
                        return factory->make(params);
                }
                
                // hint: params failed all filters
                assert(false);
            }
        }
        
        // hint: interface was not bound
        assert(false); // TODO: messaging 
    }
    
private:
    struct ConditionalBindingData
    {
        std::any filter;
        std::any factory;
    };
    
    template<typename Interface>
    class delegate_factory_without_params : public IFactory<Interface>
    {
    public:
        virtual ~delegate_factory_without_params() = default;
        
        delegate_factory_without_params(const DynamicBindingServer& dynamicBindings)
            : _dynamicBindings(dynamicBindings)
        {
        }
        
        Interface* make_p() const
        {
            return _dynamicBindings.delegate_make<Interface>();
        }
        
    private:
        const DynamicBindingServer& _dynamicBindings;
    };
    
    template<typename Interface>
    class delegate_factory_with_params : public IFactory<Interface>
    {
    public:
        virtual ~delegate_factory_with_params() = default;
        
        delegate_factory_with_params(const DynamicBindingServer& dynamicBindings)
            : _dynamicBindings(dynamicBindings)
        {
        }
        
        Interface* make_p(const factory_params_t<Interface>& params) const
        {
            return _dynamicBindings.delegate_make<Interface>(params);
        }
        
    private:
        const DynamicBindingServer& _dynamicBindings;
    };
    
    template<typename Interface>
    using DelegateFactory = typename boost::mp11::mp_if<
            has_factory_params<Interface>,
            boost::mp11::mp_defer<delegate_factory_with_params, Interface>,
            boost::mp11::mp_defer<delegate_factory_without_params, Interface>
        >::type;
        
    std::unordered_map<std::type_index, std::any> _deferredBindings;
    std::unordered_map<std::type_index, std::vector<ConditionalBindingData>> _conditionalBindings;
    mutable std::unordered_map<std::type_index, std::any> _delegateFactories;
    
    template<typename...> friend class CoreConfig;
};

} // namespace Addle::config_detail
