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

// TODO an esoteric weakness of Addle's current design:
//
// Suppose you have object A which is managed by a shared pointer, and in turn
// has a shared pointer to object B. Object B was initialized with a plain 
// reference to object A. The last reference to A is released, A is deleted, its 
// shared reference to B is released. Under expected conditions, A held the last 
// shared reference to B and B is deleted within the destructor of A. But the 
// current design doesn't guarantee this in any way, and B risks having a stale 
// reference to A.
//
// The easiest "safe" solution would be for B to have something like a QPointer
// to A which is automatically invalidated when A is deleted. However, QPointer
// cannot be escalated into a strong reference, meaning that if the last
// reference to A was released on a different thread than the one where B is
// checking the pointer's validity, it's a race condition. This is more safe
// than a plain reference in that it is more likely to catch a problem if one 
// occurs, but it is no guarantee either, making it of not much value when the 
// problem is exceptional to begin with.
//
// The most airtight solution is for A to have access to its own shared pointer, 
// and for B to be initialized with a weak pointer, but the Qt-provided 
// mechanism for an object accessing its own shared pointer adds complications 
// to interface inheritance. A cleaner implementation could be done entirely
// between the factory and the implementation with no modification to the
// interface, but of course that wouldn't be trivial to make. And it creates an
// implicit requirement that A be managed by shared pointer, which may not be
// appropriate in all cases.
//
// The thing is that in practice I'd *want* B to have a plain reference to A.
// For one, I find it satisfying in terms of "resource acquisition is 
// initialization" that the dependencies are injected into the implementation 
// constructor so that its data member *can be* a reference type (in a former 
// version it had to be a pointer). But moreover, the design of B probably
// assumes that its reference to A is valid. What would B even do if it found
// the reference were stale? Probably just throw an exception. Users of B would
// be implicitly required to ensure A is valid to be able to use B, and the only
// advantage to using a weak pointer is that B knows when there's a problem.
//
// Which brings me to the "solution" I am favoring. The factory detects
// potentially unsafe dependency orderings like this and installs an observer
// that raises an error if A is destroyed before B. It's easy to implement,
// easy to make reasonably thread-safe, eliminates a potentially gigantic amount 
// of redundant unlikely-to-fail reference checks in implementation code,
// doesn't impose particular requirements about the lifetime management of A or
// B, and seems correct for all the cases I can think of that exist so far. 
// Because the error indicates an invalid program state, it could be 
// instantaneous and fatal -- but maybe we take the polar opposite approach and 
// print a warning to a log, save a backup of the current document, and hope for 
// the best (maybe the former in debug and the latter in release).
//
// I anticipate that some future situation will eventually warrant the 
// this-as-shared-pointer feature -- i.e., where the dependency truly is
// weak. We'll cross that bridge when we get to it.

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
                        
                        static_assert(!std::is_pointer_v<std::remove_pointer_t<boost::remove_cv_ref_t<Value>>>);
                        // I doubt di (as is) has a way to pass 
                        // pointers-to-pointers as factory parameters, but I'm 
                        // not going to make an issue of it unless it ends up 
                        // being something we actually need.
                        
                        using bound_t = boost::mp11::mp_if<
                                std::is_pointer<boost::remove_cv_ref_t<Value>>,
                                std::remove_cv_t<std::remove_pointer_t<boost::remove_cv_ref_t<Value>>>,
                                boost::remove_cv_ref_t<Value>
                            >;
                        
                        return boost::di::bind<bound_t>()
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
