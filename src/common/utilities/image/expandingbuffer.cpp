#include "expandingbuffer.hpp"

#include <QPainter>

void ExpandingBuffer::initialize(QImage imageArg)
{
    _format = imageArg.format();
    _region = imageArg.rect();
    image = imageArg;
}

void ExpandingBuffer::initialize(QImage::Format format, QRect start)
{
    _format = format;

    if (start.isNull())
    {
        image = QImage(QSize(_chunkSize, _chunkSize), format);
    }
    else 
    {
        image = QImage(start.size(), format);
        _region = start;
        _ontoBufferTransform = QTransform::fromTranslate(-_region.left(), -_region.top());
    }

    image.fill(Qt::transparent);
}

void ExpandingBuffer::grab(QRect area)
{
    if (image.isNull()) // This buffer has not been initialized
    {
        initialize(QImage::Format::Format_ARGB32_Premultiplied, area);
        return;
    }
    else if (_region.isNull()) // This buffer was initialized without a starting area
    {
        QPoint center = area.center();
        _region = QRect(center.x() - (_chunkSize / 2), center.y() - (_chunkSize / 2), _chunkSize, _chunkSize);
        _ontoBufferTransform = QTransform::fromTranslate(-_region.left(), -_region.top());
    }
    
    if (_region.contains(area))
    {
        return;
    }
    
    int left = _region.left() < area.left() ? _region.left() : area.left() - _chunkSize;
    int right = _region.right() > area.right() ? _region.right() : area.right() + _chunkSize;
    int top = _region.top() < area.top() ? _region.top() : area.top() - _chunkSize;
    int bottom = _region.bottom() > area.bottom() ? _region.bottom() : area.bottom() + _chunkSize;

    QPoint offset(_region.left() - left, _region.top() - top);
    _region = QRect(left, top, right - left + 1, bottom - top + 1);

    const QImage oldBuffer = image;
    image = QImage(_region.size(), _format);
    image.fill(Qt::transparent);

    QPainter painter(&image);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(offset, oldBuffer);

    _ontoBufferTransform = QTransform::fromTranslate(-_region.left(), -_region.top());
}

void ExpandingBuffer::render(QPainter& painter, QRect paintRegion) const
{
    QRect intersection = _region.intersected(paintRegion);
    if (!intersection.isValid())
        return;

    QRect source = _ontoBufferTransform.mapRect(intersection);
    if (!source.isValid())
        return;

    painter.drawImage(intersection, image, source);
}

void ExpandingBuffer::clear()
{
    _format = QImage::Format_Invalid;
    image = QImage();
    _region = QRect();
    _ontoBufferTransform = QTransform();
}


BufferPainter ExpandingBuffer::makeBufferPainter(QRect paintArea)
{
    return BufferPainter(image, paintArea, _region.topLeft());
}