#include <QtDebug>

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/models/ibrushmodel.hpp"

//#include "interfaces/editing/operations/ibrushoperation.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

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

#include "interfaces/presenters/assets/ibrushpresenter.hpp"
#include "interfaces/presenters/tools/isizeselectionpresenter.hpp"
#include "interfaces/presenters/icolorselectionpresenter.hpp"

#include "interfaces/presenters/operations/ibrushoperationpresenter.hpp"

#include <QSharedPointer>

using namespace IBrushToolPresenterAux;

void BrushToolPresenter::initialize(IMainEditorPresenter* owner,
        ICanvasPresenter* canvasPresenter,
        IViewPortPresenter* viewPortPresenter,
        IColorSelectionPresenter* colorSelection,
        Mode mode
    )
{
    _initHelper.initializeBegin();

    _mainEditor = owner;
    _canvas = canvasPresenter;
    _viewPort = viewPortPresenter;
    _colorSelection = colorSelection;

    _brushSelection = ServiceLocator::makeUnique<IAssetSelectionPresenter>(
        QList<QSharedPointer<IAssetPresenter>>({
            ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::Basic),
            ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::Soft)
        }),
        false
    );

    _hoverPreview = std::unique_ptr<HoverPreview>(new HoverPreview(*this));

    connect_interface(_mainEditor, SIGNAL(selectedLayerChanged(QWeakPointer<ILayerPresenter>)), this, SLOT(onSelectedLayerChanged(QWeakPointer<ILayerPresenter>)));
    connect_interface(_canvas, SIGNAL(hasMouseChanged(bool)), this, SLOT(onCanvasHasMouseChanged(bool)));
    connect_interface(_brushSelection.get(), SIGNAL(selectionChanged(QList<PersistentId>)), this, SLOT(onBrushSelectionChanged()));
    connect_interface(_viewPort, SIGNAL(zoomChanged(double)), this, SLOT(onViewPortZoomChanged(double)));
    connect_interface(_colorSelection, SIGNAL(color1Changed(ColorInfo)), this, SLOT(onColorChanged(ColorInfo)));


    _initHelper.initializeEnd();
}

ToolId BrushToolPresenter::getId()
{
    _initHelper.check();
    switch(_mode)
    {
    case Mode::Brush:
        return IBrushToolPresenterAux::BRUSH_ID;
    case Mode::Eraser:
        return IBrushToolPresenterAux::ERASER_ID;
    default:
        return ToolId();
    }
}

void BrushToolPresenter::onSelectedChanged(bool selected)
{
    _initHelper.check();
    _hoverPreview->isVisible_cache.recalculate();
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
            e->accept();
            return true;
        }
    }

    _mouseHelper.event(e);
    return e->isAccepted() || QObject::event(e);
}

void BrushToolPresenter::onBrushSelectionChanged()
{
    if (_connection_onSizeChanged)
        QObject::disconnect(_connection_onSizeChanged);

    _connection_onSizeChanged = connect_interface(
        &selectedBrushPresenter()->sizeSelection(),
        SIGNAL(changed(double)),
        this,
        SLOT(onSizeChanged(double))
    );

    selectedBrushPresenter()->setPreviewScale(_mainEditor->getViewPortPresenter()->getZoom());
    selectedBrushPresenter()->setPreviewColor(_mainEditor->colorSelection().color1().color());

    _hoverPreview->isVisible_cache.recalculate();
    emit brushChanged(selectedBrush());
}

void BrushToolPresenter::onSizeChanged(double size)
{
    if (_brushStroke)
        _brushStroke->setSize(size);

    _hoverPreview->isVisible_cache.recalculate();
}

void BrushToolPresenter::onColorChanged(ColorInfo info)
{
    if (_brushStroke)
        _brushStroke->setColor(info.color());

    if (selectedBrushPresenter())
        selectedBrushPresenter()->setPreviewColor(info.color());

    _hoverPreview->isVisible_cache.recalculate();
}

void BrushToolPresenter::onCanvasHasMouseChanged(bool hasMouse)
{
    _hoverPreview->isVisible_cache.recalculate();
}

void BrushToolPresenter::onSelectedLayerChanged(QWeakPointer<ILayerPresenter> layer)
{
    _hoverPreview->isVisible_cache.recalculate();
}

void BrushToolPresenter::onViewPortZoomChanged(double zoom)
{
    if (selectedBrushPresenter())
        selectedBrushPresenter()->setPreviewScale(zoom);
}

void BrushToolPresenter::onEngage()
{
    if (!selectedBrush()) return;

    auto brush = selectedBrushPresenter();
    auto layer = _mainEditor->getSelectedLayer().toStrongRef();

    if (!layer) return;

    double size = brush->size();
    if (qIsNaN(size) || size == 0) return;

    _grace = false;

    _hoverPreview->isVisible_cache.recalculate();
    _hoverPreview->setPosition(_mouseHelper.getLatestPosition());

    QColor color = _colorSelection->color1().color();

    auto brushSurface = ServiceLocator::makeShared<IRasterSurface>();
    _brushStroke = QSharedPointer<BrushStroke>(new BrushStroke(
        brush->getBrushId(),
        color,
        size,
        brushSurface
    ));

    layer->getRenderStack().push(brushSurface->getRenderStep());

    _brushStroke->moveTo(_mouseHelper.getFirstPosition());
    _brushStroke->paint();
}

void BrushToolPresenter::onMove()
{
    if (!_brushStroke) return;

    _brushStroke->moveTo(_mouseHelper.getLatestPosition());
    _brushStroke->paint();
    _hoverPreview->setPosition(_mouseHelper.getLatestPosition());
}

void BrushToolPresenter::onDisengage()
{
    if (!_brushStroke) return;
    auto layer = _mainEditor->getSelectedLayer().toStrongRef();
    if (!layer) return;

    {
        QSharedPointer<IRasterSurface> previewBuffer;
        QSharedPointer<IRenderStep> previewRenderStep;
        if ((previewBuffer = _brushStroke->getBuffer())
             && (previewRenderStep = previewBuffer->getRenderStep())
        )
        {
            layer->getRenderStack().remove(previewRenderStep);
        }
    }
    
    auto operation = ServiceLocator::makeShared<IBrushOperationPresenter>(
        _brushStroke,
        layer
    );

    _mainEditor->push(operation);

    _brushStroke.clear();

    _grace = true;
    _hoverPreview->isVisible_cache.recalculate();
}

BrushToolPresenter::HoverPreview::HoverPreview(BrushToolPresenter& owner)
    : isVisible_cache(std::bind(&BrushToolPresenter::HoverPreview::calc_visible, this)),
    _owner(owner)
{
    isVisible_cache.onChange(&BrushToolPresenter::HoverPreview::update, this);
    isVisible_cache.initialize(false);
}

void BrushToolPresenter::HoverPreview::update()
{
    _owner._initHelper.check();

    BrushId id = _owner.selectedBrush();
    if (!id) return;

    QColor color = _owner._colorSelection->color1().color();
    double size = _owner.selectedBrushPresenter()->size();

    if (_brushStroke && _brushStroke->id() == id)
    {
        _brushStroke->setSize(size);
        _brushStroke->setColor(color);
    }
    else if (isVisible_cache.getValue())
    {
        if (!_surface) _surface = ServiceLocator::makeShared<IRasterSurface>();

        _brushStroke = std::unique_ptr<BrushStroke>(new BrushStroke(
            id,
            color, 
            size,
            _surface
        ));
    }

    if (_surface)
    {
        if (_layer && (!isVisible_cache.getValue() || _layer != _owner._mainEditor->getSelectedLayer()))
        {
            _layer->getRenderStack().remove(_surface->getRenderStep());
            _layer = nullptr;
        }

        if (isVisible_cache.getValue() && _layer != _owner._mainEditor->getSelectedLayer())
        {
            _layer = _owner._mainEditor->getSelectedLayer().toStrongRef();
            _layer->getRenderStack().push(_surface->getRenderStep());
        }   
    }

    paint();
}

void BrushToolPresenter::HoverPreview::setPosition(QPointF position)
{
    _owner._initHelper.check();

    _position = position;
    paint();
}

bool BrushToolPresenter::HoverPreview::calc_visible()
{
    return _owner._initHelper.isInitialized()
    
        && _owner._selectHelper.isSelected()
        && _owner.selectedBrush()
        && _owner.selectedBrushPresenter()

        && !_owner._mouseHelper.isEngaged()

        && _owner._mainEditor->getSelectedLayer()

        && !qIsNaN(_owner.selectedBrushPresenter()->size())
        && _owner.selectedBrushPresenter()->size() > 0

        && _owner._canvas->hasMouse()

        && !_owner._grace;
}

void BrushToolPresenter::HoverPreview::paint()
{
    _owner._initHelper.check();

    if (!isVisible_cache.getValue()) return;
    
    _brushStroke->clear();
    _brushStroke->moveTo(_position);
    _surface->clear();

    _brushStroke->paint();
}