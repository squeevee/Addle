#include "bufferpainter.hpp"

#include "interfaces/editing/ihavebufferpainter.hpp"

BufferPainter::BufferPainter(QImage& buffer, IHaveBufferPainter* owner, QRect paintArea, QPoint bufferOffset)
    : _buffer(buffer), _owner(owner), _paintArea(paintArea), _bufferOffset(bufferOffset)
{
    if (_owner)
        _owner->beforeBufferPainted(_paintArea);

    _painter = new QPainter(&_buffer);
    _painter->translate(-_bufferOffset);
}

BufferPainter::BufferPainter(BufferPainter&& other)
    : _buffer(other._buffer),
    _owner(other._owner),
    _paintArea(other._paintArea),
    _bufferOffset(other._bufferOffset),
    _painter(other._painter)
{
    other._final = false;
}

BufferPainter::~BufferPainter()
{
    if (_final)
    {
        delete _painter;
        if (_owner)
            _owner->afterBufferPainted(_paintArea);
    }
}

QPainter& BufferPainter::getPainter()
{
    /*assert painter*/
    return *_painter; 
}