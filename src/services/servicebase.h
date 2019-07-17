#ifndef SERVICEBASE_H
#define SERVICEBASE_H

#include "interfaces/services/iservice.h"
#include "interfaces/services/iservicelocator.h"

class ServiceBase : public virtual IService
{
public:
    virtual void setServiceLocator(IServiceLocator* serviceLocator) { _serviceLocator = serviceLocator; }

protected:
    IServiceLocator* _serviceLocator;
};

#endif //SERVICEBASE_H