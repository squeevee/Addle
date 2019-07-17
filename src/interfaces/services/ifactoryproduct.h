#ifndef IFACTORYPRODUCT_H
#define IFACTORYPRODUCT_H
 
#include "ifactory.h"
 
class IFactoryProduct
{
public:
    virtual ~IFactoryProduct() = default;
    
    virtual IFactory* getFactory() = 0;
    
    virtual const char* getFactoryProductName() = 0;
};

#endif //IFACTORYPRODUCT_H
