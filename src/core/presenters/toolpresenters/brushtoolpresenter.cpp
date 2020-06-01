#include <QtDebug>

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/models/ibrushmodel.hpp"

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

#include "interfaces/editing/irastersurface.hpp"

// #include "../helpers/brushsizepresethelper.hpp"
// const BrushSizePresetHelper::PxPresetHelper BrushSizePresetHelper::_instance_px = PxPresetHelper();
// const BrushSizePresetHelper::PercentPresetHelper BrushSizePresetHelper::_instance_percent = PercentPresetHelper();

using namespace IBrushToolPresenterAux;

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

    _sizeSelection = ServiceLocator::make<ISizeSelectionPresenter>();

    _hoverPreview = new HoverPreview(*this);

    updateSizeSelection();

    connect_interface(_mainEditorPresenter, SIGNAL(selectedLayerChanged(QWeakPointer<ILayerPresenter>)), this, SLOT(onSelectedLayerChanged(QWeakPointer<ILayerPresenter>)));
    connect_interface(_sizeSelection, SIGNAL(changed(double)), this, SLOT(onSizeChanged(double)));

    _initHelper.initializeEnd();
}

ISizeSelectionPresenter& BrushToolPresenter::sizeSelection()
{
    //assert initialized
    return *_sizeSelection;
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
    double size = qQNaN();
    if (_sizeSelection)
        size = _sizeSelection->get();

    if (qIsNaN(size) || size == 0)
        return;

    _hoverPreview->isVisible_cache.recalculate();
    _hoverPreview->setPosition(_mouseHelper.getLatestPosition());

    _brushPainter = ServiceLocator::makeShared<IBrushPainter>(
        _brushAssetsHelper.getSelectedAsset(),
        Qt::blue,
        size
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
    auto s_layerPresenter = _mainEditorPresenter->getSelectedLayer().toStrongRef();
    s_layerPresenter->getRenderStack().remove(_brushPainter->getBuffer()->getRenderStep());
    
    auto operation = ServiceLocator::makeShared<IBrushOperationPresenter>(
        _brushPainter,
        _mainEditorPresenter->getSelectedLayer()
    );

    _mainEditorPresenter->push(operation);

    _brushPainter.clear();
    _hoverPreview->isVisible_cache.recalculate();
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

void BrushToolPresenter::onSizeChanged(double size)
{
    if (_brushPainter)
        _brushPainter->setSize(size);
    
    _hoverPreview->isVisible_cache.recalculate();
    _hoverPreview->updateBrush();
}

void BrushToolPresenter::updateSizeSelection()
{
    if (_brushAssetsHelper.getSelectedAsset().isNull())
    {
        if(!_sizeSelection->presets().isEmpty())
            _sizeSelection->setPresets(QList<double>());
        
        return;
    }

    auto presets = ServiceLocator::get<IBrushModel>(_brushAssetsHelper.getSelectedAsset()).getPainterInfo().getPreferredSizes();
    // streamline this

    if (presets.isEmpty())
    {
        presets = { 4.0, 7.0, 12.0, 25.0, 50.0, 100.0 };
    }

    _sizeSelection->setPresets(presets);
    _sizeSelection->selectPreset(0); // temp

    {
        QSharedPointer<IBrushPainter> iconPainter = ServiceLocator::makeShared<IBrushPainter>(
            _brushAssetsHelper.getSelectedAsset()
        );

        QList<QIcon> presetIcons;

        for (double preset : presets)
        {
            iconPainter->setSize(preset);

            BrushIconHelper iconHelper(iconPainter);
            presetIcons.append(iconHelper.icon());
        }

        _sizeSelection->setPresetIcons(presetIcons);
    }
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
    if (!isVisible_cache.getValue())
    {
        _brushPainter.clear();
        return;
    }

    _brushPainter = ServiceLocator::makeShared<IBrushPainter>(
        _owner._brushAssetsHelper.getSelectedAsset(),
        Qt::blue, 
        _owner._sizeSelection->get(),
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
        && _owner._mainEditorPresenter->getSelectedLayer()
        && !qIsNaN(_owner._sizeSelection->get())
        && _owner._sizeSelection->get() != 0;
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
    if (!_brushPainter) return;
    
    _surface->clear();
    _brushPainter->startFrom(_position);
}