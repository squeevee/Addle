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
#include "interfaces/presenters/imaineditorpresenter.hpp"

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

    _previewSurface = ServiceLocator::makeShared<IRasterSurface>();
    _previewBrushPainter = ServiceLocator::makeShared<IBrushPainter>(
        _assetsHelper.getSelectedAsset(),
        Qt::blue,
        20,
        _previewSurface
    );

    connect_interface(_mainEditorPresenter, SIGNAL(selectedLayerChanged(QWeakPointer<ILayerPresenter>)), this, SLOT(onSelectedLayerChanged(QWeakPointer<ILayerPresenter>)));

    // _brushPaintTask = new BrushPaintTask(this);

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
            //_hoveringBrushPreview->move(canvasMouseEvent->pos());
            return true;
        }
    }

    return ToolPresenterBase::event(e);
}

void BrushToolPresenter::onEngage()
{
    _brushPainter = ServiceLocator::makeShared<IBrushPainter>(
        _assetsHelper.getSelectedAsset(),
        Qt::blue,
        5
    );

    auto s_layerPresenter = _mainEditorPresenter->getSelectedLayer().toStrongRef();
    s_layerPresenter->getRenderStack().push(_brushPainter->getBuffer()->getRenderStep());

    // _brushPaintTask->setBrushPainter(_brushPainter);
    // _brushPaintTask->enqueue(_mouseHelper.getFirstPosition());
    _brushPainter->startFrom(_mouseHelper.getFirstPosition());
}

void BrushToolPresenter::onMove()
{
    _brushPainter->moveTo(_mouseHelper.getLatestPosition());
    // _brushPaintTask->enqueue(_mouseHelper.getLatestPosition());
}

void BrushToolPresenter::onDisengage()
{
    auto s_layerPresenter = _mainEditorPresenter->getSelectedLayer().toStrongRef();
    s_layerPresenter->getRenderStack().remove(_brushPainter->getBuffer()->getRenderStep());
    
    _brushPainter.clear();
}

void BrushToolPresenter::onSelected()
{
    //_hoveringBrushPreview->_visibleCache.recalculate();
}

void BrushToolPresenter::onDeselected()
{
    //_hoveringBrushPreview->_visibleCache.recalculate();
}

void BrushToolPresenter::onSelectedLayerChanged(QWeakPointer<ILayerPresenter> layer)
{
    auto s_layer = layer.toStrongRef();

    //_hoveringBrushPreview->_visibleCache.recalculate();
}

// void BrushPaintTask::enqueue(QPointF point)
// {
//     {
//         const auto lock = lockIO();
//         _queue.enqueue(point);
//     }

//     start();
// }

// void BrushPaintTask::doTask()
// {
//     while (true)
//     {
//         bool brushStarted;
//         QPointF point;
//         QSharedPointer<IBrushPainter> brushPainter;
//         {
//             const auto lock = lockIO();
//             if (!_brushPainter) return;
//             if (_queue.isEmpty()) return;

//             brushPainter = _brushPainter;
//             brushStarted = _brushStarted;
//             _brushStarted = true;
//             point = _queue.dequeue();
//         }
//         if (!brushStarted)
//             brushPainter->startFrom(point);
    
//         brushPainter->moveTo(point);
//     }
// }