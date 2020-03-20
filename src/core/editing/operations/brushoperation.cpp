#include <QPainter>
#include <QBrush>
#include <QtDebug>

#include <QBitmap>

#include "interfaces/editing/surfaces/irastersurface.hpp"

#include "brushoperation.hpp"
#include "servicelocator.hpp"

void BrushOperation::initialize(
        QWeakPointer<ILayer>& layer,
        QSharedPointer<IBrushPainter> brushPainter,
        Mode mode
    )
{
    _layer = layer;
    _mode = mode;
    _brushPainter = brushPainter;

    switch (_mode)
    {
    case IBrushOperation::Mode::paint:
        _format = QImage::Format::Format_ARGB32_Premultiplied;
        break;
    case IBrushOperation::Mode::erase:
        _format = QImage::Format::Format_Alpha8;
        break;
    }

    _workingSurface = ServiceLocator::makeShared<IRasterSurface>();
}

void BrushOperation::addPainterData(BrushPainterData& painterData)
{
    QRect bound = _brushPainter->boundingRect(painterData);

    RasterPaintHandle paintHandle = _workingSurface->getPaintHandle(bound);
    _brushPainter->paint(painterData, paintHandle.getPainter());

    _areaOfEffect = _areaOfEffect.united(bound);
}

void BrushOperation::render(QPainter& painter, QRect paintRegion)
{
    QRect intersection = _areaOfEffect.intersected(paintRegion);
    if (!intersection.isEmpty())
        _workingSurface->render(painter, paintRegion);
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
    // assert layer has surface
    IRasterSurface* surface = layer->getRasterSurface();

    if (forward.isNull())
    {
        forward = _forwardStored.blockingGetUncompressed();
    }
    
    RasterPaintHandle paintHandle = surface->getPaintHandle(_areaOfEffect);
    paintHandle.getPainter().drawImage(_areaOfEffect, forward);
}

void BrushOperation::undoOperation()
{
    // assert _layer not null
    QSharedPointer<ILayer> layer = _layer.toStrongRef();
    // assert layer has surface
    IRasterSurface* surface = layer->getRasterSurface();

    QImage reverse = _reverseStored.blockingGetUncompressed();
    QImage mask = _maskStored.blockingGetUncompressed();

    RasterPaintHandle paintHandle = surface->getPaintHandle(_areaOfEffect);
    paintHandle.getPainter().drawImage(_areaOfEffect.topLeft(), reverse);
}

void BrushOperation::freeze(QImage* forwardPtr, QImage* reversePtr, QImage* maskPtr)
{
    // assert not frozen
    // assert workingbuffer is not null
    // assert layer is not null

    QSharedPointer<ILayer> layer = _layer.toStrongRef();

    QImage forward = _workingSurface->copy(_areaOfEffect);
    _workingSurface.clear();

    // apparently the threshold is fixed at 128 so I might need to make my own
    // implementation
    QImage mask = forward.createAlphaMask();

    QTransform ontoFrozenBuffer = QTransform::fromTranslate(-_areaOfEffect.x(), -_areaOfEffect.y());

    QImage reverse = QImage(_areaOfEffect.size(), _format);

    QPainter reversePainter(&reverse);
    reversePainter.setTransform(ontoFrozenBuffer);
    //layer->render(reversePainter, _areaOfEffect);

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