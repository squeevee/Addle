#include "layer.hpp"

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

    int bufferWidth = 2 * DEFAULT_BUFFER_MARGIN + _boundary.width();
    int bufferHeight = 2 * DEFAULT_BUFFER_MARGIN + _boundary.height();

    _buffer = QImage(QSize(bufferWidth, bufferHeight), QImage::Format::Format_ARGB32_Premultiplied);
    _buffer.fill(Qt::transparent);

    _ontoBuffer = QTransform();
    _ontoBuffer.translate(DEFAULT_BUFFER_MARGIN, DEFAULT_BUFFER_MARGIN);
    _fromBuffer = _ontoBuffer.inverted();

    QImage image = builder.getImage();
    if (!image.isNull())
    {
        _empty = false;

        QPainter painter(&_buffer);
        painter.drawImage(_ontoBuffer.map(_boundary.topLeft()), image);
        painter.end();
    }

    _initHelper.initializeEnd();
}

void Layer::render(QRect area, QPaintDevice* device)
{
    _initHelper.assertInitialized();

    if (!device)
        return; //throw?

    QPainter painter(device);

    QRect intersection = _boundary.intersected(area);

    painter.drawImage(intersection, _buffer, _ontoBuffer.mapRect(intersection));
    painter.end();
}

