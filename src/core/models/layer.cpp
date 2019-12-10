#include "layer.hpp"
#include "interfaces/editing/irasteroperation.hpp"

#include <QPainter>

void Layer::initialize()
{
    _initHelper.initializeBegin();
    LayerBuilder builder;
    initialize(builder);
    _initHelper.initializeEnd();
}

void Layer::initialize(LayerBuilder& builder)
{
    _initHelper.initializeBegin();

    _boundary = builder.getBoundary();
    _empty = true;

    _buffer.initialize(
        QImage::Format_ARGB32_Premultiplied,
        _boundary
    );
    
    QImage image = builder.getImage();
    if (!image.isNull())
    {
        _empty = false;

        QPainter painter(&_buffer.image);
        painter.setTransform(_buffer.getOntoBufferTransform());
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawImage(QPoint(), image);
    }

    _initHelper.initializeEnd();
}

void Layer::render(QPainter& painter, QRect area)
{
    _initHelper.assertInitialized();

    QRect intersection = _boundary.intersected(area);
    painter.drawImage(intersection, _buffer.image, _buffer.getOntoBufferTransform().mapRect(intersection));
}

BufferPainter Layer::getBufferPainter(QRect area)
{
    return _buffer.createBufferPainter(area, this);
}