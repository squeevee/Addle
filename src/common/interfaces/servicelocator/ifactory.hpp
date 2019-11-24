#ifndef IFACTORY_HPP
#define IFACTORY_HPP
 
#include "iservicelocator.hpp"

#include "imakeable.hpp"

//class IFactoryProduct;
class IFactory
{
public:
    virtual ~IFactory() = default;
    virtual IMakeable* make() = 0;
    
    virtual void setServiceLocator(IServiceLocator* serviceLocator) = 0;
    
#ifdef ADDLE_DEBUG
    virtual const char* getFactoryTypeName() = 0;
    virtual const char* getProductTypeName() = 0;
#endif
};

#endif // IFACTORY_HPP
