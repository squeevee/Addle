#ifndef SERVICEBASE_HPP
#define SERVICEBASE_HPP

#include "common/interfaces/services/iservice.hpp"
#include "common/interfaces/servicelocator/iservicelocator.hpp"

class ServiceBase : public virtual IService
{
public:
    virtual ~ServiceBase() = default;

    virtual void setServiceLocator(IServiceLocator* serviceLocator) { _serviceLocator = serviceLocator; }

protected:
    IServiceLocator* _serviceLocator;
};

#endif // SERVICEBASE_HPP