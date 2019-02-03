#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QObject>
#include <QPixmap>
#include <QTransform>
#include <QRect>

#include "models/canvas.h"

class Viewport : public QObject
{

    Q_OBJECT

public:

    Viewport() {}
    virtual ~Viewport() {}
    
    QPixmap* render();

private slots:

    void canvasBoundaryChanged(QRect newBoundary) {}

private:

    QTransform _transform;
};

#endif //VIEWPORT_H