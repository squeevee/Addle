#include <QPainter>
#include <QBrush>
#include <QtDebug>

#include <QBitmap>

#include "brushoperation.hpp"
#include "servicelocator.hpp"

void BrushOperation::initialize(
        QWeakPointer<ILayer>& layer,
        QSharedPointer<IBrushRenderer> brushRenderer,
        Mode mode
    )
{
    _layer = layer;
    _mode = mode;

    switch (_mode)
    {
    case IBrushOperation::Mode::paint:
        _format = QImage::Format::Format_ARGB32_Premultiplied;
        break;
    case IBrushOperation::Mode::erase:
        _format = QImage::Format::Format_Alpha8;
        break;
    }

    _workingBuffer.initialize(_format);
}

void BrushOperation::addPathSegment(const BrushPathSegment& pathSegment)
{

}

void BrushOperation::render(QPainter& painter, QRect paintRegion)
{
    QRect intersection = _areaOfEffect.intersected(paintRegion);
    if (!intersection.isEmpty())
        _workingBuffer.render(painter, paintRegion);
}

void BrushOperation::doOperation()
{
    QImage forward;
    QImage mask;
    if (!_frozen)
    {
        // First time this operation is being done. Just gonna nick the forward
        // image and mask while they're still uncompressed 

        freeze(&forward, nullptr, &mask);
    }

    // assert _layer not null
    QSharedPointer<ILayer> layer = _layer.toStrongRef();


    if (forward.isNull())
    {
        forward = _forwardStored.blockingGetUncompressed();
    }
        
    BufferPainter bufferPainter = layer->getBufferPainter(_areaOfEffect);
    bufferPainter.getPainter().drawImage(_areaOfEffect, forward);
    
    emit layer->renderChanged(_areaOfEffect);
}

void BrushOperation::undoOperation()
{
    // assert _layer not null
    QSharedPointer<ILayer> layer = _layer.toStrongRef();

    QImage reverse = _reverseStored.blockingGetUncompressed();
    QImage mask = _maskStored.blockingGetUncompressed();

    BufferPainter bufferPainter = layer->getBufferPainter(_areaOfEffect);
    bufferPainter.getPainter().drawImage(_areaOfEffect.topLeft(), reverse);

    emit layer->renderChanged(_areaOfEffect);
}

void BrushOperation::freeze(QImage* forwardPtr, QImage* reversePtr, QImage* maskPtr)
{
    // assert not frozen
    // assert workingbuffer is not null
    // assert layer is not null

    QSharedPointer<ILayer> layer = _layer.toStrongRef();

    QImage forward = _workingBuffer.image.copy(_workingBuffer.getOntoBufferTransform().mapRect(_areaOfEffect));
    _workingBuffer.clear();

    // apparently the threshold is fixed at 128 so I might need to make my own
    // implementation
    QImage mask = forward.createAlphaMask();

    QTransform ontoFrozenBuffer = QTransform::fromTranslate(-_areaOfEffect.x(), -_areaOfEffect.y());

    QImage reverse = QImage(_areaOfEffect.size(), _format);

    QPainter reversePainter(&reverse);
    reversePainter.setTransform(ontoFrozenBuffer);
    layer->render(reversePainter, _areaOfEffect);

    //reversePainter.setCompositionMode(QPainter::RasterOp_SourceAndDestination);
    //reversePainter.drawImage(QPoint(), mask);
    reversePainter.end();

    _forwardStored.setImage(forward);
    _forwardStored.compress();

    _reverseStored.setImage(reverse);
    _reverseStored.compress();

    _maskStored.setImage(mask);
    _maskStored.compress();

    if (forwardPtr)
        *forwardPtr = forward;
    if (reversePtr)
        *reversePtr = reverse;
    if (maskPtr)
        *maskPtr = mask;

    _frozen = true;
}

void BrushOperation::beforeBufferPainted(QRect region)
{ 
    _areaOfEffect = _areaOfEffect.united(region);
    _workingBuffer.grab(region);
}