#ifndef ICANVASSERVICE_H
#define ICANVASSERVICE_H

#include <QObject>

#include "interfaces/services/iservice.h"
#include "interfaces/models/icanvas.h"

class ICanvasService : public IService
{
public:
    ICanvasService() {}
    virtual ~ICanvasService() {}

    virtual ICanvas* getMainCanvas() = 0;

};

Q_DECLARE_INTERFACE(ICanvasService, "Addle.ICanvasService")

#endif //ICANVASSERVICE_H