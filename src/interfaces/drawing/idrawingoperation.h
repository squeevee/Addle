#ifndef IDRAWING_OPERATION_H
#define IDRAWING_OPERATION_H

#include <QRect>
#include <QImage>

enum DrawingOperationType
{

};

class IDrawingOperation
{
public:
    virtual ~IDrawingOperation() {}

    virtual DrawingOperationType getDrawingOperationType() = 0;

    virtual QRect getBoundary() = 0;
    virtual void apply(QImage& image) = 0;
};

#endif //IDRAWING_OPERATION_H