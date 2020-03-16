#include "rastersurface.hpp"

void RasterSurface::initialize(QImage image)
{
    image.convertTo(QImage::Format_ARGB32_Premultiplied);
    _buffer.initialize(image);
}

void RasterSurface::render(QPainter& painter, QRect region)
{
    _buffer.render(painter, region);
}

BufferPainter RasterSurface::makeBufferPainter(QRect region)
{
    BufferPainter painter = _buffer.makeBufferPainter(region);
    connect(
        painter.getNotifier(),
        &BufferPainterNotifier::painted,
        this, 
        &RasterSurface::onBufferPainted
    );
    return painter;
}


void RasterSurface::onBufferPainted(QRect region)
{
    emit changed(region);
}