#include "brushoperationpresenter.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"

#include "servicelocator.hpp"

void BrushOperationPresenter::initialize(QWeakPointer<ILayerPresenter> layer, QSharedPointer<IBrushPresenter> brushPresenter)
{
    _layer = layer;
    _brushPresenter = brushPresenter;

    _buffer = ServiceLocator::makeShared<IRasterSurface>();
    _preview = QSharedPointer<IRenderStep>(_buffer->makeRenderStep());

    //_brushPainter = ServiceLocator::makeUnique<IBrushPainter>(brush->getBrushId());
}

void BrushOperationPresenter::finalize()
{
    _isFinalized = false;
    emit finalized();
}

void BrushOperationPresenter::do_()
{

}

void BrushOperationPresenter::undo()
{

}