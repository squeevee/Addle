#ifndef DRAWING_OPERATION_H
#define DRAWING_OPERATION_H

#include <QRect>
#include <QImage>

#include "interfaces/drawing/idrawingoperation.h"

class DrawingOperation : public IDrawingOperation
{
public:
    virtual DrawingOperationType getDrawingOperationType() = 0;

    virtual QRect getBoundary() = 0;
    virtual void apply(QImage& image) = 0;
};

#endif //DRAWING_OPERATION_H