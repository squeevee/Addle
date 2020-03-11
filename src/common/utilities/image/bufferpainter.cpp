#include "bufferpainter.hpp"

#include "interfaces/editing/icanpaintbuffer.hpp"

BufferPainter::BufferPainter(QImage& buffer, QRect paintArea, QPoint bufferOffset)
    : _buffer(buffer), _paintArea(paintArea), _bufferOffset(bufferOffset)
{
    _painter = new QPainter(&_buffer);
    _notifier = new BufferPainterNotifier();
    emit _notifier->painting(_paintArea);

    _painter->translate(-_bufferOffset);
}

BufferPainter::BufferPainter(BufferPainter&& other)
    : _buffer(other._buffer),
    _notifier(other._notifier),
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
        emit _notifier->painted(_paintArea);
        delete _painter;
        delete _notifier;
    }
}

QPainter& BufferPainter::getPainter()
{
    /*assert painter*/
    return *_painter; 
}

