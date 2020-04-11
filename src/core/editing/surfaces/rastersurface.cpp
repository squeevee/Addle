#include "rastersurface.hpp"

void RasterSurface::initialize(
        QRect area,
        QPainter::CompositionMode compositionMode,
        InitFlags flags
    )
{
    const QWriteLocker lock(&_lock);

    _format = QImage::Format_ARGB32_Premultiplied;

    if (_area.isValid())
    {
        _buffer = QImage(area.size(), _format);
        _bufferOffset = _area.topLeft();
        _area = area;
    }
}

void RasterSurface::initialize(
        QImage image,
        QPoint offset,
        QPainter::CompositionMode compositionMode,
        InitFlags flags
    )
{
    const QWriteLocker lock(&_lock);

    _format = QImage::Format_ARGB32_Premultiplied;

    if (image.format() != _format)
        image.convertTo(_format);
    _buffer = image;
    _area = QRect(offset, image.size());
}

QSharedPointer<IRenderStep> RasterSurface::getRenderStep()
{
    const QReadLocker lock(&_lock);
    if (!_renderStep)
    {
        _renderStep = QSharedPointer<IRenderStep>(new RasterSurfaceRenderStep(*this));
    }
    return _renderStep;
}

void RasterSurface::clear()
{
    QRect oldArea;
    {
        const QWriteLocker lock(&_lock);

        oldArea = _area;
        _area = QRect();
    }

    emit changed(oldArea);
    if (_renderStep)
    {
        emit _renderStep->changed(oldArea);
    }
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
        if (_buffer.size().isNull())
        {
            // The buffer has not yet been initialized.

            _area = allocArea;
            _bufferOffset = _area.topLeft();

            _buffer = QImage(_area.size(), _format);
            _buffer.fill(Qt::transparent);
            return;
        }
        else
        {
            // The buffer has already been initialized, and the surface was
            // recently cleared.

            QRect bufferArea = QRect(QPoint(), _buffer.size());
            bufferArea.moveCenter(allocArea.center());

            if (bufferArea.contains(allocArea))
            {
                // TODO: resize if allocArea is significantly smaller than
                // the buffer

                _area = allocArea;
                _bufferOffset = _area.topLeft();
                _buffer.fill(Qt::transparent);
                return;
            }
            else 
            {
                _area = bufferArea;
                _bufferOffset = _area.topLeft();
                // Proceed as below to resize the buffer
            }
        }
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
    _lock.unlock();

    {
        const QReadLocker lock(&_lock);

        emit changed(handle.getArea());
        if (_renderStep)
        {
            emit _renderStep->changed(handle.getArea());
        }
    }
}

void RasterSurfaceRenderStep::onPush(RenderData& data)
{
    // if _owner's composition mode is CompositionMode_Source, then add a mask
    // to RenderData covering _owner._area
}

void RasterSurfaceRenderStep::onPop(RenderData& data)
{
    const QReadLocker lock(&_owner._lock);

    if (!_owner._area.isValid()) return;

    QRect intersection = _owner._area.intersected(data.getArea());

    data.getPainter()->setCompositionMode(_owner._compositionMode);
    data.getPainter()->drawImage(
        intersection, 
        _owner._buffer,
        intersection.translated(-_owner._bufferOffset)
    );
}
