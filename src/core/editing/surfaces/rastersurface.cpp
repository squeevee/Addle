#include "rastersurface.hpp"


void RasterSurface::render(QPainter& painter, QRect region)
{
    
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