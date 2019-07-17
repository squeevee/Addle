#ifndef IFACTORY_H
#define IFACTORY_H
 
#include "iservicelocator.h"

class IFactoryProduct;
class IFactory
{
public:
    virtual ~IFactory() = default;
    virtual IFactoryProduct* make() = 0;
    
    virtual void setServiceLocator(IServiceLocator* serviceLocator) = 0;
    
    virtual const char* getFactoryName() = 0;
};

#endif //IFACTORY_H
