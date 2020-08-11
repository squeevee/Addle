#ifndef ISERVICE_HPP
#define ISERVICE_HPP

#include <QObject>

#include "interfaces/servicelocator/iservicelocator.hpp"
namespace Addle {

class IService
{
public:

// #define ADDLE_STRING__ISERVICE__SERVICE_THREAD_NAME_TEMPLATE "addle:%1-service-thread"
//     static const QString SERVICE_THREAD_NAME_TEMPLATE;
    
    virtual ~IService() {}

    //virtual void setServiceLocator(IServiceLocator* serviceLocator) = 0;

};

} // namespace Addle

#endif // ISERVICE_HPP