#include "rastersurface.hpp"


void RasterSurface::initialize(QRect area, InitFlags flags)
{
    _format = QImage::Format_ARGB32_Premultiplied;

    if (_area.isValid())
    {
        _buffer = QImage(area.size(), _format);
        _bufferOffset = _area.topLeft();
        _area = area;
    }
}

void RasterSurface::initialize(QImage image, InitFlags flags)
{
    _format = QImage::Format_ARGB32_Premultiplied;

    if (image.format() != _format)
        image.convertTo(_format);
    _buffer = image;
    _area = image.rect();
}

QImage RasterSurface::copy(QRect copyArea, QPoint* offset) const
{
    QRect copyRect = copyArea.isValid() ? _area.intersected(copyArea) : _area;

    if (offset) *offset = copyRect.topLeft();
    return _buffer.copy(copyRect.translated(_bufferOffset));
}

void RasterSurface::render(QPainter& painter, QRect area) const
{
    QRect intersection = _area.intersected(area);

    painter.drawImage(intersection, _buffer, intersection.translated(_bufferOffset));
}

void RasterSurface::allocate(QRect allocArea)
{
    if (!_area.isValid())
    {
        _area = allocArea;
        _buffer = QImage(_area.size(), _format);
        return;
    }

    if (_area.contains(allocArea)) return;


    QRect bufferArea(_bufferOffset, _buffer.size());

    int left = bufferArea.left() <= allocArea.left() ? bufferArea.left() : allocArea.left() - CHUNK_SIZE;
    int right = bufferArea.right() >= allocArea.right() ? bufferArea.right() : allocArea.right() + CHUNK_SIZE;
    int top = bufferArea.top() <= allocArea.top() ? bufferArea.top() : allocArea.top() - CHUNK_SIZE;
    int bottom = bufferArea.bottom() >= allocArea.bottom() ? bufferArea.bottom() : allocArea.bottom() + CHUNK_SIZE;

    QPoint drawOffset = _bufferOffset - QPoint(left, top);
    bufferArea = QRect(left, top, right - left + 1, bottom - top + 1);

    const QImage oldBuffer = _buffer;
    _buffer = QImage(bufferArea.size(), _format);
    _buffer.fill(Qt::transparent);

    QPainter painter(&_buffer);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(drawOffset, oldBuffer);

    _area = _area.united(allocArea);
    _bufferOffset = QPoint(left, top);
}

void RasterSurface::merge(IRasterSurface& other)
{
    
}

// void RasterSurface::initialize(QImage image)
// {
//     image.convertTo(QImage::Format_ARGB32_Premultiplied);
//     _buffer.initialize(image);
// }

// void RasterSurface::render(QPainter& painter, QRect region)
// {
//     _buffer.render(painter, region);
// }

// BufferPainter RasterSurface::makeBufferPainter(QRect region)
// {
//     BufferPainter painter = _buffer.makeBufferPainter(region);
//     connect(
//         painter.getNotifier(),
//         &BufferPainterNotifier::painted,
//         this, 
//         &RasterSurface::onBufferPainted
//     );
//     return painter;
// }


// void RasterSurface::onBufferPainted(QRect region)
// {
//     emit changed(region);
// }

// #include "rasterbuffer.hpp"

// #include <QPainter>

// void RasterBuffer::initialize(QImage imageArg)
// {
//     _format = imageArg.format();
//     _region = imageArg.rect();
//     image = imageArg;
// }

// void RasterBuffer::initialize(QImage::Format format, QRect start)
// {
//     _format = format;

//     if (start.isNull())
//     {
//         image = QImage(QSize(_chunkSize, _chunkSize), format);
//     }
//     else 
//     {
//         image = QImage(start.size(), format);
//         _region = start;
//         _ontoBufferTransform = QTransform::fromTranslate(-_region.left(), -_region.top());
//     }

//     image.fill(Qt::transparent);
// }

// void RasterBuffer::grab(QRect area)
// {
//     if (image.isNull()) // This buffer has not been initialized
//     {
//         initialize(QImage::Format::Format_ARGB32_Premultiplied, area);
//         return;
//     }
//     else if (_region.isNull()) // This buffer was initialized without a starting area
//     {
//         QPoint center = area.center();
//         _region = QRect(center.x() - (_chunkSize / 2), center.y() - (_chunkSize / 2), _chunkSize, _chunkSize);
//         _ontoBufferTransform = QTransform::fromTranslate(-_region.left(), -_region.top());
//     }
    
//     if (_region.contains(area))
//     {
//         return;
//     }
    
//     int left = _region.left() < area.left() ? _region.left() : area.left() - _chunkSize;
//     int right = _region.right() > area.right() ? _region.right() : area.right() + _chunkSize;
//     int top = _region.top() < area.top() ? _region.top() : area.top() - _chunkSize;
//     int bottom = _region.bottom() > area.bottom() ? _region.bottom() : area.bottom() + _chunkSize;

//     QPoint offset(_region.left() - left, _region.top() - top);
//     _region = QRect(left, top, right - left + 1, bottom - top + 1);

//     const QImage oldBuffer = image;
//     image = QImage(_region.size(), _format);
//     image.fill(Qt::transparent);

//     QPainter painter(&image);

//     painter.setCompositionMode(QPainter::CompositionMode_Source);
//     painter.drawImage(offset, oldBuffer);

//     _ontoBufferTransform = QTransform::fromTranslate(-_region.left(), -_region.top());
// }

// void RasterBuffer::render(QPainter& painter, QRect paintRegion) const
// {
//     QRect intersection = _region.intersected(paintRegion);
//     if (!intersection.isValid())
//         return;

//     QRect source = _ontoBufferTransform.mapRect(intersection);
//     if (!source.isValid())
//         return;

//     painter.drawImage(intersection, image, source);
// }

// void RasterBuffer::clear()
// {
//     _format = QImage::Format_Invalid;
//     image = QImage();
//     _region = QRect();
//     _ontoBufferTransform = QTransform();
// }


// BufferPainter RasterBuffer::makeBufferPainter(QRect paintArea)
// {
//     return BufferPainter(image, paintArea, _region.topLeft());
// }
