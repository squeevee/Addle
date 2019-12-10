#include <QtDebug>

#include "interfaces/presenters/ilayerpresenter.hpp"

#include "utilities/unhandledexceptionrouter.hpp"

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
    // try
    // {
            
    //TODO

    ILayerPresenter* layer = _editorPresenter->getSelectedLayer();

    _operation = ServiceLocator::make<IRasterOperation>(
        _editorPresenter->getSelectedLayer()->getModel(),
        IRasterOperation::Mode::paint);

    layer->setRasterOperation(_operation);

    double brushSize = _brushHelper.getAbsoluteBrushSize();
    QPointF pos(_toolPathHelper.getLastCanvasPosition());

    BaseBrushRenderer& renderer = _brushHelper.getCurrentBrushTipRenderer();
    QRect bound = renderer.getBoundingRect(brushSize, pos);

    BufferPainter bufferPainter = _operation->getBufferPainter(bound);
    bufferPainter.getPainter().setPen(QPen());
    bufferPainter.getPainter().setBrush(Qt::black);
    renderer.render(brushSize, pos, bufferPainter.getPainter());

    _editorPresenter->getSelectedLayer()->renderChanged(_editorPresenter->getSelectedLayer()->getCanvasBounds());
    // }
    // ADDLE_FALLBACK_CATCH
}

void BrushToolPresenter::onPointerMove()
{
    // try
    // {
    double brushSize = _brushHelper.getAbsoluteBrushSize();
    QLineF line(_toolPathHelper.getPreviousCanvasPosition(), _toolPathHelper.getLastCanvasPosition());

    BaseBrushRenderer& renderer = _brushHelper.getCurrentBrushTipRenderer();
    QRect bound = renderer.getBoundingRect(brushSize, line);

    BufferPainter bufferPainter = _operation->getBufferPainter(bound);
    bufferPainter.getPainter().setPen(QPen());
    bufferPainter.getPainter().setBrush(Qt::black);
    renderer.render(brushSize, line, bufferPainter.getPainter());

    _editorPresenter->getSelectedLayer()->renderChanged(_editorPresenter->getSelectedLayer()->getCanvasBounds());
    // }
    // ADDLE_FALLBACK_CATCH
}

void BrushToolPresenter::onPointerDisengage()
{
    // try
    // {

    ILayerPresenter* layer = _editorPresenter->getSelectedLayer();

    layer->unsetRasterOperation();
    _editorPresenter->doOperation(QSharedPointer<IUndoableOperation>(_operation));
    
    layer->renderChanged(_editorPresenter->getSelectedLayer()->getCanvasBounds());
    // }
    // ADDLE_FALLBACK_CATCH
}
