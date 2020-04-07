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

QSharedPointer<IRenderStep> RasterSurface::getRenderStep()
{
    if (!_renderStep)
        _renderStep = QSharedPointer<IRenderStep>(new RasterSurfaceRenderStep(*this));
    return _renderStep;
}

QImage RasterSurface::copy(QRect copyArea, QPoint* offset) const
{
    QRect copyRect = copyArea.isValid() ? _area.intersected(copyArea) : _area;

    if (offset) *offset = copyRect.topLeft();
    return _buffer.copy(copyRect.translated(_bufferOffset));
}

void RasterSurface::allocate(QRect allocArea)
{
    if (!_area.isValid())
    {
        _area = allocArea;
        _buffer = QImage(_area.size(), _format);
        _buffer.fill(Qt::transparent);
        _bufferOffset = _area.topLeft();
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

void RasterSurface::onHandleDestroyed(const RasterPaintHandle& handle)
{
    emit changed(handle.getArea());
    if (_renderStep)
    {
        _renderStep->changed(handle.getArea());
    }
}

void RasterSurfaceRenderStep::onPop(RenderData& data)
{
    QRect intersection = _owner._area.intersected(data.getArea());

    data.getPainter()->drawImage(
        intersection, 
        _owner._buffer,
        intersection.translated(-_owner._bufferOffset)
    );
}
