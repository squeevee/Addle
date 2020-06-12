#include <QtDebug>

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/models/ibrushmodel.hpp"

//#include "interfaces/editing/operations/ibrushoperation.hpp"

#include "utilities/unhandledexceptionrouter.hpp"
#include "utilities/qtextensions/qobject.hpp"
#include "utilities/qtextensions/qlist.hpp"

#include "brushtoolpresenter.hpp"
#include "servicelocator.hpp"
#include "utilities/mathutils.hpp"

#include "interfaces/rendering/irenderstack.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"

#include "utilities/editing/brushstroke.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "interfaces/editing/irastersurface.hpp"

#include "interfaces/editing/ibrushengine.hpp"

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
        ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::Basic),
        ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::Soft)
        //ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::AliasedCircle),
        //ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::Square)
    });

    _sizeSelection = ServiceLocator::make<ISizeSelectionPresenter>();

    _hoverPreview = new HoverPreview(*this);

    updateSizeSelection();

    connect_interface(_mainEditorPresenter, SIGNAL(selectedLayerChanged(QWeakPointer<ILayerPresenter>)), this, SLOT(onSelectedLayerChanged(QWeakPointer<ILayerPresenter>)));
    connect_interface(_sizeSelection, SIGNAL(changed(double)), this, SLOT(onSizeChanged(double)));
    connect_interface(_canvasPresenter, SIGNAL(hasMouseChanged(bool)), this, SLOT(onCanvasHasMouseChanged(bool)));

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
            if (_grace)
            {
                _grace = false;
                _hoverPreview->isVisible_cache.recalculate();
            }
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

    _grace = false;

    _hoverPreview->isVisible_cache.recalculate();
    _hoverPreview->setPosition(_mouseHelper.getLatestPosition());

    _brushStroke = QSharedPointer<BrushStroke>(new BrushStroke(
        _brushAssetsHelper.getSelectedAsset(),
        Qt::blue,
        size,
        ServiceLocator::makeShared<IRasterSurface>()
    ));

    auto s_layerPresenter = _mainEditorPresenter->getSelectedLayer().toStrongRef();
    s_layerPresenter->getRenderStack().push(_brushStroke->getBuffer()->getRenderStep());

    _brushStroke->moveTo(_mouseHelper.getFirstPosition());
    _brushStroke->paint();
}

void BrushToolPresenter::onMove()
{
    _brushStroke->moveTo(_mouseHelper.getLatestPosition());
    _brushStroke->paint();
    _hoverPreview->setPosition(_mouseHelper.getLatestPosition());
}

void BrushToolPresenter::onDisengage()
{
    auto s_layerPresenter = _mainEditorPresenter->getSelectedLayer().toStrongRef();
    s_layerPresenter->getRenderStack().remove(_brushStroke->getBuffer()->getRenderStep());
    
    auto operation = ServiceLocator::makeShared<IBrushOperationPresenter>(
        _brushStroke,
        _mainEditorPresenter->getSelectedLayer()
    );

    _mainEditorPresenter->push(operation);

    _brushStroke.clear();

    _grace = true;
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
    if (_brushStroke)
        _brushStroke->setSize(size);

    _sizeSelection->setIcon(_iconHelper.icon(
        _brushAssetsHelper.getSelectedAsset(),
        Qt::blue,
        size
    ));
    
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

    auto presets = ServiceLocator::get<IBrushModel>(_brushAssetsHelper.getSelectedAsset()).info().getPreferredSizes();
    // streamline this

    if (presets.isEmpty())
    {
        presets = arrayToQList(IBrushToolPresenterAux::DEFAULT_SIZES);
    }

    _sizeSelection->setPresets(presets);
    _sizeSelection->selectPreset(0); // temp

    _iconHelper.setBackground(Qt::white);

    QList<QIcon> presetIcons;

    for (double preset : presets)
    {
        presetIcons.append(_iconHelper.icon(
            _brushAssetsHelper.getSelectedAsset(),
            Qt::blue,
            preset
        ));
    }

    _sizeSelection->setPresetIcons(presetIcons);

    _sizeSelection->set(60);
}

void BrushToolPresenter::onCanvasHasMouseChanged(bool hasMouse)
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
    // if (!isVisible_cache.getValue())
    // {
    //     _brushStroke.clear();
    //     return;
    // }

    _brushStroke = QSharedPointer<BrushStroke>(new BrushStroke(
        _owner._brushAssetsHelper.getSelectedAsset(),
        Qt::blue, 
        _owner._sizeSelection->get(),
        _surface
    ));
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
        && _owner._sizeSelection->get() != 0

        && _owner._canvasPresenter->hasMouse()
        && !_owner._grace;
        /*&& _owner._viewPortPresenter->hasFocus()*/
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
    if (!_brushStroke) return;
    
    _brushStroke->clear();
    _brushStroke->moveTo(_position);
    _surface->clear();

    _brushStroke->paint();
}