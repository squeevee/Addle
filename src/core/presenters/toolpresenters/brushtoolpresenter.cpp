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

    _brushAssetsHelper.setAssetList({
        ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::Basic)
        //ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::AliasedCircle),
        //ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::Square)
    });

    _propertyDecorationHelper.initializeIdProperty<BrushId>(
        "brush",
        _brushAssetsHelper.getAssetIds()
    );

    _hoverPreview = new HoverPreview(*this);

    connect_interface(_mainEditorPresenter, SIGNAL(selectedLayerChanged(QWeakPointer<ILayerPresenter>)), this, SLOT(onSelectedLayerChanged(QWeakPointer<ILayerPresenter>)));

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
            _hoverPreview->setPosition(canvasMouseEvent->pos());
            return true;
        }
    }

    return ToolPresenterBase::event(e);
}

void BrushToolPresenter::onEngage()
{
    _hoverPreview->isVisible_cache.recalculate();
    _hoverPreview->setPosition(_mouseHelper.getLatestPosition());

    _brushPainter = ServiceLocator::makeShared<IBrushPainter>(
        _brushAssetsHelper.getSelectedAsset(),
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
    _hoverPreview->setPosition(_mouseHelper.getLatestPosition());
}

void BrushToolPresenter::onDisengage()
{
    _hoverPreview->isVisible_cache.recalculate();

    auto s_layerPresenter = _mainEditorPresenter->getSelectedLayer().toStrongRef();
    s_layerPresenter->getRenderStack().remove(_brushPainter->getBuffer()->getRenderStep());
    
    _brushPainter.clear();
}

void BrushToolPresenter::onSelected()
{
    _hoverPreview->isVisible_cache.recalculate();
}

void BrushToolPresenter::onDeselected()
{
    _hoverPreview->isVisible_cache.recalculate();
}

void BrushToolPresenter::onSelectedLayerChanged(QWeakPointer<ILayerPresenter> layer)
{
    _hoverPreview->isVisible_cache.recalculate();
}

BrushToolPresenter::HoverPreview::HoverPreview(BrushToolPresenter& owner)
    : isVisible_cache(std::bind(&BrushToolPresenter::HoverPreview::calc_visible, this)),
    _owner(owner)
{
    isVisible_cache.onChange(&BrushToolPresenter::HoverPreview::onVisibleChanged, this);

    _surface = ServiceLocator::makeShared<IRasterSurface>();
    updateBrush();
}

BrushToolPresenter::HoverPreview::~HoverPreview()
{
}

void BrushToolPresenter::HoverPreview::updateBrush()
{
    _brushPainter = ServiceLocator::makeShared<IBrushPainter>(
        _owner._brushAssetsHelper.getSelectedAsset(),
        Qt::blue, 
        5,
        _surface
    );
    paint();
}

void BrushToolPresenter::HoverPreview::setPosition(QPointF position)
{
    _position = position;
    paint();
}

bool BrushToolPresenter::HoverPreview::calc_visible()
{
    return _owner.isSelected()
        && !_owner._mouseHelper.isEngaged()
        && _owner._mainEditorPresenter->getSelectedLayer();
        // TODO: and viewport has focus
}

void BrushToolPresenter::HoverPreview::onVisibleChanged(bool visible)
{
    if (visible)
    {
        auto layerPresenter = _owner._mainEditorPresenter->getSelectedLayer();
        if (layerPresenter)
        {
            auto s_layerPresenter = layerPresenter.toStrongRef();

            s_layerPresenter->getRenderStack().push(_surface->getRenderStep());
        }
        paint();
    }
    else 
    {
        auto layerPresenter = _owner._mainEditorPresenter->getSelectedLayer();
        if (layerPresenter)
        {
            auto s_layerPresenter = layerPresenter.toStrongRef();

            s_layerPresenter->getRenderStack().remove(_surface->getRenderStep());
        }
    }
}

void BrushToolPresenter::HoverPreview::paint()
{
    if (!isVisible_cache.getValue()) return;
    
    _surface->clear();
    _brushPainter->startFrom(_position);
}