#include <QtDebug>

#include "interfaces/presenters/ilayerpresenter.hpp"

#include "brushtoolpresenter.hpp"
#include "servicelocator.hpp"
#include "utilities/mathutils.hpp"

void BrushToolPresenter::initialize(IEditorPresenter* owner)
{
    _initHelper.initializeBegin();

    _editorPresenter = owner;
    ToolPresenterBase::initialize_p(owner);

    _translationContext = "BrushToolPresenter";
    
    initializeIdOptionText<BrushTipId>("brushTip", _brushHelper.getBrushTips().toList());

    _initHelper.initializeEnd();
}

void BrushToolPresenter::onPointerEngage()
{
    //TODO

    ILayerPresenter* layer = _editorPresenter->getSelectedLayer();

    _operation = ServiceLocator::make<IRasterOperation>(
        QWeakPointer<ILayer>(),
        IRasterOperation::Mode::paint);

    layer->setRasterOperation(_operation);

    double brushSize = _brushHelper.getAbsoluteBrushSize();
    QPointF pos(_toolPathHelper.getLastCanvasPosition());

    BaseBrushRenderer& renderer = _brushHelper.getCurrentBrushTipRenderer();
    QRect bound = renderer.getBoundingRect(brushSize, pos);
    
    auto painter = _operation->getBufferPainter(bound);
    renderer.render(brushSize, pos, *painter);

    _editorPresenter->getSelectedLayer()->renderChanged(bound);
}

void BrushToolPresenter::onPointerMove()
{
    double brushSize = _brushHelper.getAbsoluteBrushSize();
    QLineF line(_toolPathHelper.getPreviousCanvasPosition(), _toolPathHelper.getLastCanvasPosition());

    BaseBrushRenderer& renderer = _brushHelper.getCurrentBrushTipRenderer();
    QRect bound = renderer.getBoundingRect(brushSize, line);

    auto painter = _operation->getBufferPainter(bound);
    renderer.render(brushSize, line, *painter);

    _editorPresenter->getSelectedLayer()->renderChanged(bound);
}

void BrushToolPresenter::onPointerDisengage()
{
    //_operation->doOperation();

    ILayerPresenter* layer = _editorPresenter->getSelectedLayer();

    layer->unsetRasterOperation();

    layer->getModel()->applyRasterOperation(_operation);
    layer->renderChanged(layer->getCanvasBounds());

}
