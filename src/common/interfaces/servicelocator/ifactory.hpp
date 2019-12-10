#ifndef IFACTORY_HPP
#define IFACTORY_HPP
 
#include <typeinfo>

#include "iservicelocator.hpp"

class IFactory
{
public:
    virtual ~IFactory() = default;
    virtual void* make() = 0;
    
    virtual const std::type_info& getInterfaceType() = 0;
    virtual const std::type_info& getImplementationType() = 0;

    virtual void setServiceLocator(IServiceLocator* serviceLocator) = 0;
    
#ifdef ADDLE_DEBUG
    virtual const char* getFactoryTypeName() = 0;
    virtual const char* getProductTypeName() = 0;
#endif
};

#endif // IFACTORY_HPP
