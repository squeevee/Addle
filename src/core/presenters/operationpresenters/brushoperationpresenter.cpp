#include "brushoperationpresenter.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"

#include "servicelocator.hpp"

void BrushOperationPreview::before(RenderData& data)
{

}

void BrushOperationPreview::after(RenderData& data)
{
    data.getPainter()->drawImage(QPoint(), _owner._buffer);
}

void BrushOperationPresenter::initialize(QWeakPointer<ILayerPresenter> layer, QSharedPointer<IBrushPresenter> brush)
{
    _layer = layer;
    _brush = brush;

    _preview = QSharedPointer<BrushOperationPreview>(new BrushOperationPreview(*this));

    _buffer = QImage(QSize(1280,1024), QImage::Format_ARGB32_Premultiplied);

    _brushPainter = ServiceLocator::makeUnique<IBrushPainter>(brush->getBrushId());
}

void BrushOperationPresenter::addPainterData(BrushPainterData data)
{
    //auto brushPainter = _brush->getModel()->getPainter();

    QRect rect = _brushPainter->boundingRect(data);

    _brushPainter->paint(data, _buffer);

    emit _preview->changed(rect);
}

void BrushOperationPresenter::finalize()
{
    _isFinalized = false;
    emit isFinalizedChanged(_isFinalized);
}

void BrushOperationPresenter::do_()
{

}

void BrushOperationPresenter::undo()
{

}