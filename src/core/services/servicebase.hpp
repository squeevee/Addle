#ifndef SERVICEBASE_HPP
#define SERVICEBASE_HPP

#include "core/compat.hpp"

#include "interfaces/services/iservice.hpp"
#include "interfaces/servicelocator/iservicelocator.hpp"

class ADDLE_CORE_EXPORT ServiceBase : public virtual IService
{
public:
    virtual ~ServiceBase() = default;

    virtual void setServiceLocator(IServiceLocator* serviceLocator) { _serviceLocator = serviceLocator; }

protected:
    IServiceLocator* _serviceLocator;
};

#endif // SERVICEBASE_HPP