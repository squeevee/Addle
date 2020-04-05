#include <QtDebug>

#include "interfaces/presenters/ilayerpresenter.hpp"

//#include "interfaces/editing/operations/ibrushoperation.hpp"

#include "utilities/unhandledexceptionrouter.hpp"
#include "utilities/qtextensions/qobject.hpp"

#include "brushtoolpresenter.hpp"
#include "servicelocator.hpp"
#include "utilities/mathutils.hpp"

#include "interfaces/rendering/irenderstack.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"

#include "interfaces/editing/ibrushpainter.hpp"

// #include "../helpers/brushsizepresethelper.hpp"
// const BrushSizePresetHelper::PxPresetHelper BrushSizePresetHelper::_instance_px = PxPresetHelper();
// const BrushSizePresetHelper::PercentPresetHelper BrushSizePresetHelper::_instance_percent = PercentPresetHelper();


void BrushToolPresenter::initialize(IMainEditorPresenter* owner)
{
    _initHelper.initializeBegin();

    _mainEditorPresenter = owner;
    ToolPresenterBase::initialize_p(owner);

    _assetsHelper.setAssetList({
        ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::Basic)
        //ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::AliasedCircle),
        //ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::Square)
    });

    _propertyDecorationHelper.initializeIdProperty<BrushId>(
        "brush",
        _assetsHelper.getAssetIds()
    );

    _hoveringBrushPreview = QSharedPointer<HoveringBrushPreview>(new HoveringBrushPreview(*this));

    connect_interface(_mainEditorPresenter, SIGNAL(selectedLayerChanged(QWeakPointer<ILayerPresenter>)), this, SLOT(onSelectedLayerChanged(QWeakPointer<ILayerPresenter>)));

    //_mainEditorPresenter->getSelectedLayer()->getRenderStack().push(_hoveringBrushRenderStep);

    _initHelper.initializeEnd();
}


bool BrushToolPresenter::event(QEvent* e)
{
    if (e->type() == CanvasMouseEvent::getType())
    {
        CanvasMouseEvent* canvasMouseEvent = static_cast<CanvasMouseEvent*>(e);

        if(canvasMouseEvent->action() == CanvasMouseEvent::Action::move
            && !_mouseHelper.isEngaged())
        {
            _hoveringBrushPreview->move(canvasMouseEvent->pos());
            return true;
        }
    }

    return ToolPresenterBase::event(e);
}

void BrushToolPresenter::onEngage()
{
    _hoveringBrushPreview->_visibleCache.recalculate();

    auto layer = _mainEditorPresenter->getSelectedLayer();
    auto brush = _assetsHelper.getSelectedAssetPresenter();

    _operation = ServiceLocator::makeShared<IBrushOperationPresenter>(
        layer,
        brush
    );

    // BrushPainterData painterData(
    //     Qt::blue, 10, _mouseHelper.getLatestPosition()
    // );
    // _operation->addPainterData(painterData);

    auto s_layer = layer.toStrongRef();
    s_layer->getRenderStack().push(_operation->getPreview());
}

void BrushToolPresenter::onMove()
{
    _hoveringBrushPreview->move(_mouseHelper.getLatestPosition());
    // BrushPainterData painterData(
    //     Qt::blue, 10,
    //     _mouseHelper.getPreviousPosition(),
    //     _mouseHelper.getLatestPosition()
    // );
    // _operation->addPainterData(painterData);
}

void BrushToolPresenter::onDisengage()
{
    _mainEditorPresenter->push(_operation);

    auto layer = _mainEditorPresenter->getSelectedLayer();
    auto s_layer = layer.toStrongRef();
    s_layer->getRenderStack().remove(_operation->getPreview());
    
    _hoveringBrushPreview->_visibleCache.recalculate();
}

void BrushToolPresenter::onSelected()
{
    _hoveringBrushPreview->_visibleCache.recalculate();
}

void BrushToolPresenter::onDeselected()
{
    _hoveringBrushPreview->_visibleCache.recalculate();
}

void BrushToolPresenter::onSelectedLayerChanged(QWeakPointer<ILayerPresenter> layer)
{
    auto s_layer = layer.toStrongRef();

    _hoveringBrushPreview->_visibleCache.recalculate();
    s_layer->getRenderStack().push(_hoveringBrushPreview);
}



bool HoveringBrushPreview::calculateVisible()
{
    return _owner.isSelected()
        && !_owner._mouseHelper.isEngaged();
}

void HoveringBrushPreview::onVisibleChanged()
{
    if (_visibleCache.getValue())
        updateBuffer();
    else
        emit changed(_previewRect);
}

void HoveringBrushPreview::move(QPointF to)
{
    //_previewData.setStartPoint(to);
    updateBuffer();
}

void HoveringBrushPreview::updateBuffer()
{
    // Possible optimizations:
    // - Don't let the buffer get bigger than the viewport
    // - Lower the resolution of the buffer when the viewport is zoomed out.
    // - Know when a brush painter doesn't need floating-point precision and
    //   don't redraw until the mouse has moved by int-precision.
    // - Similarly if the mouse is moving very quickly (for example), floating-
    //   point precision will not be necessary and the buffer can just be moved
    //   without being redrawn. (this may help it feel less laggy)

    // if (!_visibleCache.getValue()) return;

    // //auto brushPainter = _owner.getSelectedBrushPresenter()->getModel()->getPainter(); //streamline

    // auto brushPainter = ServiceLocator::makeUnique<IBrushPainter>(_owner.getSelectedBrush());
    // _previousPreviewRect = _previewRect;
    // _previewRect = brushPainter->boundingRect(_previewData);

    // // Determine if the buffer needs to be reallocated for a new size
    // if (_previewRect.width() != 0 && _previewRect.height() != 0)
    // {
    //     int width = _buffer.width();
    //     if (_previewRect.width() > width)
    //         width = _previewRect.width() * BUFFER_RESIZE_FACTOR;
    //     else if (width != 0 && _previewRect.width() / width < BUFFER_RESIZE_FACTOR)
    //         width /= BUFFER_RESIZE_FACTOR;

    //     int height = _buffer.height();

    //     if (_previewRect.height() > height)
    //         height = _previewRect.height() * BUFFER_RESIZE_FACTOR;
    //     else if (height != 0 && _previewRect.height() / height < BUFFER_RESIZE_FACTOR)
    //         height /= BUFFER_RESIZE_FACTOR;

    //     if (width > _buffer.width() || height > _buffer.height())
    //         _buffer = QImage(QSize(width, height), QImage::Format_ARGB32_Premultiplied);
    // }

    // _buffer.fill(Qt::transparent);

    // QRect bufferRect = _buffer.rect();
    // bufferRect.moveCenter(_previewRect.center());

    // _bufferOffset = bufferRect.topLeft();
    // _previewData.setOntoBufferTransform(QTransform::fromTranslate(-_bufferOffset.x(), -_bufferOffset.y()));

    // brushPainter->paint(_previewData, _buffer);

    // if (_previousPreviewRect.isValid())
    //     emit changed(_previewRect.united(_previousPreviewRect));
    // else 
    //     emit changed(_previewRect);
}

void HoveringBrushPreview::onPush(RenderData& data)
{

}

void HoveringBrushPreview::onPop(RenderData& data)
{
    if (!_visibleCache.getValue()) return;

    QPainter* painter = data.getPainter();
    
    QRect source = _previewRect.translated(-_bufferOffset);
    
    painter->drawImage(_previewRect, _buffer, source);
}