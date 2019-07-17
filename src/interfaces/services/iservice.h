#ifndef ISERVICE_H
#define ISERVICE_H

#include <QObject>

#include "iservicelocator.h"

class IService
{
public:
    virtual ~IService() {}

    virtual void setServiceLocator(IServiceLocator* serviceLocator) = 0;
};

#endif //ISERVICE_H