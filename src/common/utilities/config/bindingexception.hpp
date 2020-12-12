#ifndef BINDINGEXCEPTION_HPP
#define BINDINGEXCEPTION_HPP

#include "exceptions/addleexception.hpp"

#include "bindingselector.hpp"

namespace Addle { 
  
DECL_LOGIC_ERROR(BindingException)
class BindingException : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(BindingException)
public:
    enum Why
    {
        BindingNotFound
    };
    
    BindingException(Why why, const Config::detail::BindingSelector& selector = Config::detail::BindingSelector())
            :
#ifdef ADDLE_DEBUG
        AddleException(""), // TODO: there absolutely must be a better way to do this
#endif
        _why(why), _selector(selector)
    {
    }
    
private:
    Why _why;
    Config::detail::BindingSelector _selector;
};
    
}

#endif // BINDINGEXCEPTION_HPP
