#include <QPainter>
#include <QBrush>
#include <QtDebug>
#include "rasteroperation.hpp"

void RasterOperation::initialize(
        const QWeakPointer<ILayer>& layer,
        Mode mode
    )
{
    _layer = layer;
    _mode = mode;

    switch (_mode)
    {
    case IRasterOperation::Mode::paint:
        _format = QImage::Format::Format_ARGB32_Premultiplied;
        break;
    case IRasterOperation::Mode::erase:
        _format = QImage::Format::Format_Alpha8;
        break;
    }

    _buffer = QImage(QSize(BUFFER_CHUNK_SIZE, BUFFER_CHUNK_SIZE), _format);
    _buffer.fill(Qt::transparent);
}

std::unique_ptr<QPainter> RasterOperation::getBufferPainter(QRect region)
{
    if (_areaOfEffect.isNull())
        _areaOfEffect = region;
    else
        _areaOfEffect = _areaOfEffect.united(region);

    autoSizeBuffer(region);

    QPainter* painter = new QPainter(&_buffer);
    QPoint offset = _areaOfEffect.topLeft() - _bufferRegion.topLeft();
    painter->translate(offset);

    return std::unique_ptr<QPainter>(painter);
}

void RasterOperation::render(QPainter& painter, QRect region)
{
    QPoint offset = _areaOfEffect.topLeft() - _bufferRegion.topLeft();
    QRect intersection = _areaOfEffect.intersected(region);

    painter.drawImage(intersection, _buffer, intersection.translated(offset));
}

void RasterOperation::doOperation()
{

}

void RasterOperation::undoOperation()
{

}

void RasterOperation::autoSizeBuffer(QRect criticalArea)
{
    if (_bufferRegion.isNull())
    {
        QPoint center = criticalArea.center();
        _bufferRegion = QRect(center.x() - BUFFER_CHUNK_SIZE / 2, center.y() - BUFFER_CHUNK_SIZE / 2, BUFFER_CHUNK_SIZE, BUFFER_CHUNK_SIZE);
    }
    
    if (_bufferRegion.contains(criticalArea))
    {
        return;
    }

    int left = _bufferRegion.left() < criticalArea.left() ? _bufferRegion.left() : criticalArea.left() - BUFFER_CHUNK_SIZE;
    int right = _bufferRegion.right() > criticalArea.right() ? _bufferRegion.right() : criticalArea.right() - BUFFER_CHUNK_SIZE;
    int top = _bufferRegion.top() < criticalArea.top() ? _bufferRegion.top() : criticalArea.top() - BUFFER_CHUNK_SIZE;
    int bottom = _bufferRegion.bottom() > criticalArea.bottom() ? _bufferRegion.bottom() : criticalArea.bottom() - BUFFER_CHUNK_SIZE;

    QPoint offset(_bufferRegion.left() - left, _bufferRegion.top() - top);
    _bufferRegion = QRect(left, top, right - left, bottom - top);

    const QImage oldBuffer = _buffer;
    _buffer = QImage(_bufferRegion.size(), _format);
    _buffer.fill(Qt::transparent);

    QPainter painter(&_buffer);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(offset, oldBuffer);
}