/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef RASTERBITHANDLES_HPP
#define RASTERBITHANDLES_HPP

#include "compat.hpp"
#include <QRect>
#include <QImage>
namespace Addle {

class IRasterSurface;
class ADDLE_COMMON_EXPORT RasterBitReader
{
public:
    RasterBitReader(RasterBitReader&& other);
    ~RasterBitReader();

    inline QImage::Format format() { return _buffer.format(); }

    inline QPoint offset() const { return _area.topLeft() - _bufferOffset; }

    inline int pixelWidth() const { return _pixelWidth; }
    inline QRect area() const { return _area; }

    const uchar* scanLine(int line) const;

private:
    RasterBitReader(const IRasterSurface& surface,
        const QImage& buffer,
        QPoint bufferOffset,
        QRect area);

    const IRasterSurface& _surface;
    const QImage& _buffer;
    
    const QPoint _bufferOffset;
    const QRect _area;

    const int _pixelWidth;

    bool _final = true;

    friend class IRasterSurface;
};

class ADDLE_COMMON_EXPORT RasterBitWriter
{
public:
    RasterBitWriter(RasterBitWriter&& other);
    ~RasterBitWriter();

    inline QImage::Format format() { return _buffer.format(); }

    inline QRect area() const { return _area; }
    inline int pixelWidth() const { return _pixelWidth; }

    uchar* scanLine(int line) const;
private:
    RasterBitWriter(IRasterSurface& surface, QImage& buffer, QPoint bufferOffset, QRect area);

    IRasterSurface& _surface;
    QImage& _buffer;

    QPoint _bufferOffset;
    QRect _area;

    const int _pixelWidth;

    bool _final = true;

    friend class IRasterSurface;
};

} // namespace Addle
#endif // RASTERBITHANDLES_HPP