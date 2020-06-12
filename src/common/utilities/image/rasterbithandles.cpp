#include "rasterbithandles.hpp"
#include <QtDebug>

#include "interfaces/editing/irastersurface.hpp"

RasterBitReader::RasterBitReader(
    const IRasterSurface& surface,
    const QImage& buffer,
    QPoint bufferOffset,
    QRect area)
    : _surface(surface),
    _buffer(buffer),
    _bufferOffset(bufferOffset),
    _area(area),
    _pixelWidth(buffer.depth() / 8)
{
}

RasterBitReader::RasterBitReader(RasterBitReader&& other)
    : _surface(other._surface),
    _buffer(other._buffer),
    _bufferOffset(other._bufferOffset),
    _area(other._area),
    _pixelWidth(other._pixelWidth)
{
    other._final = false;
}

RasterBitReader::~RasterBitReader()
{
    if (_final) _surface.onBitReaderDestroyed(*this);
}

const uchar* RasterBitReader::scanLine(int line) const
{
    return _buffer.constScanLine(line + _area.top() - _bufferOffset.y())
        + (_area.left() - _bufferOffset.x()) * _pixelWidth;
}

RasterBitWriter::RasterBitWriter(
    IRasterSurface& surface,
    QImage& buffer,
    QPoint bufferOffset,
    QRect area)
    : _surface(surface),
    _buffer(buffer),
    _bufferOffset(bufferOffset),
    _area(area),
    _pixelWidth(buffer.depth() / 8)
{
}

RasterBitWriter::RasterBitWriter(RasterBitWriter&& other)
    : _surface(other._surface),
    _buffer(other._buffer),
    _bufferOffset(other._bufferOffset),
    _area(other._area),
    _pixelWidth(other._pixelWidth)
{
    other._final = false;
}

RasterBitWriter::~RasterBitWriter()
{
    if (_final) _surface.onBitWriterDestroyed(*this);
}

uchar* RasterBitWriter::scanLine(int line) const
{
    return _buffer.scanLine(line + _area.top() - _bufferOffset.y())
        + (_area.left() - _bufferOffset.x()) * _pixelWidth;
}