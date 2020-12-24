/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#pragma once

#include <memory>
#include <tuple>

#include <boost/di/extension/injections/extensible_injector.hpp>

#include "interfaces/services/ifactory.hpp"

namespace Addle::config_detail {
        
// TODO: make init_params into a forwarding tuple
    
// template<typename... Args>
// constexpr decltype(auto) _bind_params(std::tuple<Args...>&& params)
// {
//     return boost::di::bind<Args>
// }
    
template<class Interface, class Impl, class Injector>
class Factory : public IFactory<Interface>
{
public:
    Factory(const Injector& injector)
        : _injector(const_cast<Injector&>(injector))
    {
    }
    
    ~Factory() = default;

    Interface* make_p(const config_detail::init_params_placeholder_t<Interface>& params) const
    {
        if constexpr (config_detail::has_init_params<Interface>::value)
        {
            auto injector = std::apply(
                [&](auto&&... args) {
                    return boost::di::make_injector(std::forward<decltype(args)>(args)...);
                },
                std::tuple_cat(
                    std::make_tuple(
                        boost::di::extension::make_extensible(_injector),
                        boost::di::bind<Interface>().template to<Impl>()[boost::di::override]
                    ),
                    boost::mp11::tuple_transform(
                        [] (auto&& param) {
                            using param_t = decltype(param);
                            return boost::di::bind<std::remove_const_t<std::remove_reference_t<param_t>>>()
                                .to(std::forward<param_t>(param))[boost::di::override];
                        },
                        params
                    )
                )
            );
            return injector.template create<Interface*>();
        }
        else
        {
            return _injector.template create<Interface*>();
        }
        return nullptr;
    }
    
private:
    Injector& _injector;
};

} // namespace Addle::config_detail
