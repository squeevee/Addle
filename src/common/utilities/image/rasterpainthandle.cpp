#include "rasterpainthandle.hpp"

#include "interfaces/editing/irastersurface.hpp"

RasterPaintHandle::RasterPaintHandle(RasterPaintHandle&& other)
    : _surface(other._surface),
    _area(other._area),
    _buffer(other._buffer),
    _bufferOffset(other._bufferOffset),
    _painter(other._painter)
{
    other._final = false;
}

RasterPaintHandle::RasterPaintHandle(IRasterSurface& surface, QImage& buffer, QPoint bufferOffset, QRect area)
    : _surface(surface), _buffer(buffer), _bufferOffset(bufferOffset), _area(area)
{
    _painter = new QPainter(&_buffer);    
    _painter->translate(-_bufferOffset);
}

RasterPaintHandle::~RasterPaintHandle()
{
    if (_final)
    {
        _surface.onPaintHandleDestroyed(*this);
        delete _painter;
    }
}