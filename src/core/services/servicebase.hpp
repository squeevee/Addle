#ifndef SERVICEBASE_HPP
#define SERVICEBASE_HPP

#include "compat.hpp"

#include "interfaces/services/iservice.hpp"
#include "interfaces/servicelocator/iservicelocator.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT ServiceBase : public virtual IService
{
public:
    virtual ~ServiceBase() = default;

    virtual void setServiceLocator(IServiceLocator* serviceLocator) { _serviceLocator = serviceLocator; }

protected:
    IServiceLocator* _serviceLocator;
};

} // namespace Addle

#endif // SERVICEBASE_HPP