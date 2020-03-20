#ifndef RASTERSURFACEHANDLE_HPP
#define RASTERSURFACEHANDLE_HPP

#include <QObject>
#include <QSharedData>
#include <QSharedDataPointer>

#include <QRect>
#include <QImage>
#include <QPainter>

class IRasterSurface;
class RasterPaintHandle
{
public:
    RasterPaintHandle(RasterPaintHandle&& other);
    ~RasterPaintHandle();

    QPainter& getPainter() { return *_painter; }

    void merge(const IRasterSurface& other);

    // "private" constructor for use by IPaintableRasterSurface implementation.
    RasterPaintHandle(IRasterSurface& surface, QImage& buffer, QPoint bufferOffset, QRect area);

private:
    IRasterSurface& _surface;

    QRect _area;

    QPainter* _painter = nullptr;

    QPoint _bufferOffset;
    QImage& _buffer;

    bool _final = true;
};

#endif // RASTERSURFACEHANDLE_HPP