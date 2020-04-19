#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

#include <cmath>

#include <QPoint>
#include <QPointF>
#include <QRectF>

static inline QPoint pointFloor(QPointF in)
{
    return QPoint(floor(in.x()), floor(in.y()));
}

static inline QRect coarseBoundRect(QRectF rect)
{
    return QRect(
        (int)floor(rect.left()),
        (int)floor(rect.top()),
        (int)ceil(rect.right()) - (int)floor(rect.left()),
        (int)ceil(rect.bottom()) - (int)floor(rect.top())
    );
}

#endif // MATHUTILS_HPP