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
#include <boost/parameter.hpp>

#include "interfaces/services/ifactory.hpp"

namespace Addle::config_detail {

template<class Interface, class Impl, class Injector>
class factory_with_params : public IFactory<Interface>
{
public:
    factory_with_params(const Injector& injector)
        : _injector(const_cast<Injector&>(injector))
    {
    }
    
    virtual ~factory_with_params() = default;
    
protected:
    Interface* make_p(
            const config_detail::factory_params_t<Interface>& params
        ) const override
    {        
        auto injector = std::apply(
            [&](auto&&... args) {
                return boost::di::make_injector(
                    std::forward<decltype(args)>(args)...
                );
            },
            std::tuple_cat(
                std::make_tuple(
                    boost::di::extension::make_extensible(_injector),
                    boost::di::bind<Interface>()
                        .template to<Impl>()
                        [boost::di::override]
                ),
                generate_tuple_over_list<config_detail::factory_params_t<Interface>>(
                    [&] (auto t) {
                        using Tag = typename decltype(t)::type::tag_type;
                        using Value = typename decltype(t)::type::value_type;
                        
                        const auto& keyword = 
                            boost::parameter::keyword<Tag>::instance;
                        Value value = params[keyword];
                        
                        return boost::di::bind<boost::remove_cv_ref_t<Value>>()
                            .to(std::forward<Value>(value))
                            //.named(keyword)
                            [boost::di::override];
                            
                        // TODO: using keywords as names does work, but DI has 
                        // limited (?) ability to deduce named dependencies if
                        // names are not given in the impl constructor.
                        //
                        // In particular, this leads to the gotcha that some
                        // parameters are queitly default-constructed instead of
                        // passed through from the factory arguments if the impl
                        // constructor is not annotated with names.
                        //
                        // As a rule, factory parameters colliding with each
                        // other or other dependencies should be rare, so for
                        // the time being injector names are disabled. A more
                        // elegant (or at least, more effective) solution is
                        // probably out there and should be implemented
                        // eventually.
                    }
                )
            )
        );
        return injector.template create<Interface*>();
    }
    
private:
    Injector& _injector;
};


template<class Interface, class Impl, class Injector>
class factory_without_params : public IFactory<Interface>
{
public:
    factory_without_params(const Injector& injector)
        : _injector(injector)
    {
    }
    
    virtual ~factory_without_params() = default;
    
protected:
    Interface* make_p() const override
    {
        return _injector.template create<Interface*>();
    }
    
private:
    const Injector& _injector;
};

template<class Interface, class Impl, class Injector>
using Factory = boost::mp11::mp_if<
        config_detail::has_factory_params<Interface>,
        factory_with_params<Interface, Impl, Injector>,
        factory_without_params<Interface, Impl, Injector>
    >;

} // namespace Addle::config_detail
