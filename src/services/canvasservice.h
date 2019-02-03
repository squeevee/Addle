#ifndef CANVASSERVICE_H
#define CANVASSERVICE_H

#include <QObject>

#include "interfaces/services/icanvasservice.h"
#include "interfaces/models/icanvas.h"

class CanvasService : public QObject, public ICanvasService
{
    Q_OBJECT
    Q_INTERFACES(ICanvasService)

public:
    CanvasService() {}
    virtual ~CanvasService() {}

    virtual ICanvas* getMainCanvas() { return _mainCanvas; }

private:

    ICanvas* _mainCanvas = nullptr;
};

#endif //CANVASSERVICE_H