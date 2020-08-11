#ifndef RASTERSURFACEHANDLE_HPP
#define RASTERSURFACEHANDLE_HPP

#include "compat.hpp"
#include <QObject>
#include <QSharedData>
#include <QSharedDataPointer>

#include <QRect>
#include <QImage>
#include <QPainter>
namespace Addle {

class IRasterSurface;
class ADDLE_COMMON_EXPORT RasterPaintHandle
{
public:
    RasterPaintHandle(RasterPaintHandle&& other);
    ~RasterPaintHandle();

    QPainter& painter() { return *_painter; }

    QRect area() const { return _area; }

private:
    RasterPaintHandle(IRasterSurface& surface, QImage& buffer, QPoint bufferOffset, QRect area);

    IRasterSurface& _surface;

    QRect _area;

    QPainter* _painter = nullptr;

    QPoint _bufferOffset;
    QImage& _buffer;

    bool _final = true;

    friend class IRasterSurface;
};

} // namespace Addle
#endif // RASTERSURFACEHANDLE_HPP