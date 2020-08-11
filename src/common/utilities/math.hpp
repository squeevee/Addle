#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

#include <cmath>

#include <QPoint>
#include <QPointF>
#include <QRectF>
namespace Addle {

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

static inline QRect coarseBoundRect(QPointF pos, double size)
{
    double halfSize = size / 2;

    return coarseBoundRect(
        QRectF(pos - QPointF(halfSize, halfSize), QSizeF(size, size))
    );
}

// static inline bool near(QPointF p1, QPointF p2, double range)
// {
//     const double a = p1.x() - p2.x();
//     const double b = p1.y() - p2.y();
//     return (a * a) + (b * b) <= (range * range);
// }

static inline double distance(const QPointF& p1, const QPointF& p2)
{
    const double a = p1.x() - p2.x();
    const double b = p1.y() - p2.y();
    return sqrt(a * a + b * b);
}

} // namespace Addle
#endif // MATHUTILS_HPP