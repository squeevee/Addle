#ifndef CANVAS_H
#define CANVAS_H

#include <QObject>
#include <QImage>
#include <QColor>
#include <QRect>

#include "interfaces/models/icanvas.h"
#include "interfaces/drawing/idrawingoperation.h"

class Canvas : public QObject, public ICanvas
{
    Q_OBJECT
    Q_INTERFACES(ICanvas)

public:
    Canvas(QImage* image = nullptr);
    virtual ~Canvas() {}

    void applyOperation(IDrawingOperation& operation);

signals:
    void boundaryChanged(QRect newBoundary);

    void applyingDrawingOperation(const IDrawingOperation& operation);
    void appliedDrawingOperation(const IDrawingOperation& operation);

private:

    QImage* _image;
    
    QColor _background_color;

};

#endif //CANVAS_H