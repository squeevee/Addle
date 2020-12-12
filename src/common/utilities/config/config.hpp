#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <boost/mpl/for_each.hpp>

#include "injector.hpp"

namespace Addle { namespace Config {

typedef detail::BindingSet BindingSet;
    
template<class IdWrapper>
extern BindingSet* get_module_binding();
    
typedef BindingSet* (*get_binding_t)();

class ServiceContainer : public detail::Container
{
    struct add_module_binding_functor
    {
        add_module_binding_functor(Container& container_)
            : container(container_)
        {
        }
        
        template<class IdWrapper>
        inline void operator()(const IdWrapper&) const
        {
            container.addBindings(get_module_binding<IdWrapper>());
        }
        
        Container& container;
    };
    
public:
    void addBindings(get_binding_t f)
    {
        Container::addBindings(f());
    }
    
    template<class IdWrappers>
    void addBindings()
    {
        boost::mpl::for_each<IdWrappers>(
            add_module_binding_functor(*this)
        );
    }
};
    
}} // namespace Addle::Config

#endif // CONFIG_HPP
