#ifndef SERVICELOCATORCONTROL_HPP
#define SERVICELOCATORCONTROL_HPP

#include "servicelocator.hpp"

#include "utilities/errors.hpp"

namespace Addle {

class ServiceLocatorControl 
{
public:
    static inline void configure(IServiceConfig* config)
    {
        ADDLE_ASSERT(config);
        
        if (!ServiceLocator::_instance)
        {
            ServiceLocator::_instance = new ServiceLocator;
        }
        
        try 
        {
            ServiceLocator::_instance->addConfig(config);
        }
        catch(...)
        {
            delete config;
            throw;
        }
    }
    
    static inline void destroy()
    {
        ADDLE_ASSERT(ServiceLocator::_instance);
        
        delete ServiceLocator::_instance;
        ServiceLocator::_instance = nullptr;

    }
};
    
} // namespace Addle

#endif // SERVICELOCATORCONTROL_HPP
