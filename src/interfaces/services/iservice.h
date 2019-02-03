#ifndef ISERVICE_H
#define ISERVICE_H

#include <QObject>

class IService
{
public:
    IService() {}
    virtual ~IService() {}

};

Q_DECLARE_INTERFACE(IService, "Addle.IService")

#endif //ISERVICE_H