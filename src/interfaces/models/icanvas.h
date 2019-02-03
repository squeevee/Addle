#ifndef ICANVAS_H
#define ICANVAS_H

#include <QObject>
#include <QRect>

#include "interfaces/drawing/idrawingoperation.h"

class ICanvas
{
public:
    virtual ~ICanvas() {}

    virtual void applyOperation(IDrawingOperation& operation) = 0;

signals:
    virtual void boundaryChanged(QRect newBoundary) = 0;

    virtual void applyingDrawingOperation(const IDrawingOperation& operation) = 0;
    virtual void appliedDrawingOperation(const IDrawingOperation& operation) = 0;
};

Q_DECLARE_INTERFACE(ICanvas, "Addle.ICanvas")

#endif //ICANVAS_H