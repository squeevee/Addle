/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QtDebug>

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/models/ibrush.hpp"

#include "interfaces/models/ilayer.hpp"

//#include "interfaces/editing/operations/ibrushoperation.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

#include "utilities/qobject.hpp"
#include "utils.hpp"

#include "brushtoolpresenter.hpp"
#include "servicelocator.hpp"

#include "interfaces/rendering/irenderer.hpp"
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
using namespace Addle;

BrushToolPresenter::BrushToolPresenter(ToolId id, IMainEditorPresenter& editor)
    : _mode([id] {
            switch(id)
            {
                case CoreTools::Brush:
                    return Mode::Brush;
                case CoreTools::Eraser:
                    return Mode::Eraser;
                default:
                    Q_UNREACHABLE();
            }
        }()),
    _mainEditor(editor),
    _selectHelper(*this)
{
    _mouseHelper.onEngage.bind(&BrushToolPresenter::onEngage, this);  
    _mouseHelper.onMove.bind(&BrushToolPresenter::onMove, this);
    _mouseHelper.onDisengage.bind(&BrushToolPresenter::onDisengage, this);
    _selectHelper.onIsSelectedChanged.bind(&BrushToolPresenter::onSelectedChanged, this);
}
    
void BrushToolPresenter::initialize(IMainEditorPresenter* owner, Mode mode)
{
    const Initializer init(_initHelper);

    //_mainEditor = owner;
    //_canvas = &_mainEditor->canvasPresenter();
    //_viewPort = &_mainEditor->viewPortPresenter();
    //_colorSelection = &_mainEditor->colorSelection();
    
    //_mode = mode;

    _previewProvider = QSharedPointer<const IBrushPresenter::PreviewInfoProvider>(
        new BrushPreviewProvider(this)
    );

    switch(_mode)
    {
    case Mode::Brush:
        _brushSelection = ServiceLocator::makeUnique<IAssetSelectionPresenter>(
            QList<QSharedPointer<IAssetPresenter>>({
                ServiceLocator::makeShared<IBrushPresenter>(CoreBrushes::BasicBrush, _previewProvider),
                ServiceLocator::makeShared<IBrushPresenter>(CoreBrushes::SoftBrush, _previewProvider)
            }),
            false
        );
        _brushSelection->setFavorites({ 
            CoreBrushes::BasicBrush, 
            CoreBrushes::SoftBrush 
        });

        _brushSelection->select(CoreBrushes::BasicBrush);
        break;

    case Mode::Eraser:
        _brushSelection = ServiceLocator::makeUnique<IAssetSelectionPresenter>(
            QList<QSharedPointer<IAssetPresenter>>({
                ServiceLocator::makeShared<IBrushPresenter>(CoreBrushes::BasicEraser, _previewProvider)
            }),
            false
        );
        _brushSelection->setFavorites({ 
            CoreBrushes::BasicEraser
        });

        _brushSelection->select(CoreBrushes::BasicEraser);
        break;
    }

    _hoverPreview = std::unique_ptr<HoverPreview>(new HoverPreview(*this));

    //connect_interface(_mainEditor, SIGNAL(documentPresenterChanged(QSharedPointer<Addle::IDocumentPresenter>)), this, SLOT(onDocumentChanged(QSharedPointer<Addle::IDocumentPresenter>)));
    //connect_interface(_canvas, SIGNAL(hasMouseChanged(bool)), this, SLOT(onCanvasHasMouseChanged(bool)));
    connect_interface(_brushSelection.get(), SIGNAL(selectionChanged(QList<AddleId>)), this, SLOT(onBrushSelectionChanged()));
    connect_interface(_viewPort, SIGNAL(zoomChanged(double)), this, SLOT(onViewPortZoomChanged(double)));
    connect_interface(_colorSelection, SIGNAL(color1Changed(ColorInfo)), this, SLOT(onColorChanged(ColorInfo)));
    //connect_interface(_mainEditor, SIGNAL(topSelectedLayerChanged(QSharedPointer<ILayerPresenter>)), this, SLOT(onSelectedLayerChanged()));
}

ToolId BrushToolPresenter::id() const
{
    ASSERT_INIT();
    switch(_mode)
    {
    case Mode::Brush:
        return CoreTools::Brush;
    case Mode::Eraser:
        return CoreTools::Eraser;
    default:
        return ToolId();
    }
}

void BrushToolPresenter::onSelectedChanged(bool selected)
{
    try
    {
        ASSERT_INIT();
        _hoverPreview->isVisible_cache.recalculate();
    } 
    ADDLE_SLOT_CATCH
}

bool BrushToolPresenter::event(QEvent* e)
{
    try
    {
        if (e->type() == CanvasMouseEvent::type())
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
    }
    ADDLE_EVENT_CATCH(e)

    return e->isAccepted() || QObject::event(e);
}

void BrushToolPresenter::onBrushSelectionChanged()
{
    try 
    {
        if (_connection_onSizeChanged)
            QObject::disconnect(_connection_onSizeChanged);

        _connection_onSizeChanged = connect_interface(
            &selectedBrushPresenter()->sizeSelection(),
            SIGNAL(changed(double)),
            this,
            SLOT(onSizeChanged(double))
        );

        _hoverPreview->isVisible_cache.recalculate();
        emit brushChanged(selectedBrush());
    } 
    ADDLE_SLOT_CATCH
}

void BrushToolPresenter::onSizeChanged(double size)
{
    try 
    {
        if (_brushStroke)
            _brushStroke->setSize(size);

        _hoverPreview->isVisible_cache.recalculate();
    } 
    ADDLE_SLOT_CATCH
}

void BrushToolPresenter::onColorChanged(ColorInfo info)
{
    try 
    {
        if (_brushStroke)
            _brushStroke->setColor(info.color());

        refreshPreviews();
        _hoverPreview->isVisible_cache.recalculate();
    }
    ADDLE_SLOT_CATCH
}

void BrushToolPresenter::onCanvasHasMouseChanged(bool hasMouse)
{
    try 
    { 
        _hoverPreview->isVisible_cache.recalculate(); 
    } 
    ADDLE_SLOT_CATCH
}

void BrushToolPresenter::onSelectedLayerChanged()
{
    try 
    {
        _hoverPreview->isVisible_cache.recalculate();
        _hoverPreview->update();
    }
    ADDLE_SLOT_CATCH
}

void BrushToolPresenter::onViewPortZoomChanged(double zoom)
{
    try 
    {
        if (selectedBrushPresenter())
            selectedBrushPresenter()->sizeSelection().refreshPreviews();
        
        refreshPreviews();
    } 
    ADDLE_SLOT_CATCH
}

void BrushToolPresenter::onDocumentChanged(QSharedPointer<IDocumentPresenter> document)
{
    try 
    {
        _document = document;

        _hoverPreview->isVisible_cache.recalculate();
        if (!_document)
            _hoverPreview->update();

    } 
    ADDLE_SLOT_CATCH
}

void BrushToolPresenter::onEngage()
{
    try 
    {

        if (!selectedBrush()) return;

        auto brush = selectedBrushPresenter();
        auto layer = _document->topSelectedLayer();
        _operatingLayer = layer;

        if (!layer) return;

        double size = brush->size();
        if (qIsNaN(size) || size == 0) return;

        _grace = false;

        _hoverPreview->isVisible_cache.recalculate();
        _hoverPreview->setPosition(_mouseHelper.latestPosition());

        QColor color = _colorSelection->color1().color();

        auto brushSurface = ServiceLocator::makeShared<IRasterSurface>();
        _brushStroke = QSharedPointer<BrushStroke>(new BrushStroke(
            brush->brushId(),
            color,
            size,
            brushSurface
        ));

//         if (brush->model().copyMode())
//         {
//             brushSurface->link(layer->model()->rasterSurface());
//             //brushSurface->setCompositionMode(QPainter::CompositionMode_Source);
//         }

        //layer->renderer().push(brushSurface->renderable());

        _brushStroke->moveTo(_mouseHelper.firstPosition());
        _brushStroke->paint();

    } 
    ADDLE_SLOT_CATCH
}

void BrushToolPresenter::onMove()
{
    try 
    {
        if (!_brushStroke) return;

        _brushStroke->moveTo(_mouseHelper.latestPosition());
        _brushStroke->paint();
        _hoverPreview->setPosition(_mouseHelper.latestPosition());
    } 
    ADDLE_SLOT_CATCH
}

void BrushToolPresenter::onDisengage()
{
    try 
    {
        if (!_brushStroke) return;

        _brushStroke->buffer()->unlink();    
        auto layer = _operatingLayer.toStrongRef();
        if (!layer) return;

        {
//             QSharedPointer<IRasterSurface> previewBuffer;
//             QSharedPointer<IRenderable> previewRenderable;
//             if ((previewBuffer = _brushStroke->buffer())
//                 && (previewRenderable = previewBuffer->renderable())
//             )
//             {
//                 //layer->renderer().remove(previewRenderable);
//             }
        }
        
        auto operation = ServiceLocator::makeShared<IBrushOperationPresenter>(
            _brushStroke,
            layer
        );

        //_mainEditor->push(operation);

        _brushStroke.clear();

        _grace = true;
        _hoverPreview->isVisible_cache.recalculate();
        _operatingLayer.clear();
    } 
    ADDLE_SLOT_CATCH
}

BrushToolPresenter::HoverPreview::HoverPreview(BrushToolPresenter& owner)
    : _owner(owner)
{
    isVisible_cache.calculateBy(&BrushToolPresenter::HoverPreview::calc_visible, this),
    isVisible_cache.onChange.bind(&BrushToolPresenter::HoverPreview::update, this);
    isVisible_cache.initialize(false);
}

void BrushToolPresenter::HoverPreview::update()
{
    _ASSERT_INIT(_owner._initHelper);

    BrushId id = _owner.selectedBrush();
    if (!id) return;

    QColor color = _owner._colorSelection->color1().color();
    double size = _owner.selectedBrushPresenter()->size();

    if (_brushStroke && _brushStroke->id() == id)
    {
        _brushStroke->setSize(size);
        _brushStroke->setColor(color);
    }
    else if (isVisible_cache.value())
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
        if (_layer && (!isVisible_cache.value() || _layer != _owner._document->topSelectedLayer()))
        {
            //_layer->renderer().remove(_surface->renderable());
            if (_owner.selectedBrushPresenter()->model().eraserMode())
            {
                _surface->unlink();
            }
            _layer = nullptr;
        }

//         if (isVisible_cache.value() && _layer != _owner._document->topSelectedLayer())
//         {
//             _layer = _owner._document->topSelectedLayer();
//             if (_owner.selectedBrushPresenter()->model().eraserMode())
//             {
//                 _surface->link(_layer->model()->rasterSurface());
//             }
//             //_layer->renderer().push(_surface->renderable());
//         }   
    }

    paint();
}

void BrushToolPresenter::HoverPreview::setPosition(QPointF position)
{
    _ASSERT_INIT(_owner._initHelper);

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

        && _owner._document
        && _owner._document->topSelectedLayer()

        && !qIsNaN(_owner.selectedBrushPresenter()->size())
        && _owner.selectedBrushPresenter()->size() > 0

        //&& _owner._canvas->hasMouse()

        && !_owner._grace;
}

void BrushToolPresenter::HoverPreview::paint()
{
    _ASSERT_INIT(_owner._initHelper);

    if (!isVisible_cache.value()) return;
    
    _brushStroke->clear();
    _brushStroke->moveTo(_position);
    _surface->clear();

    _brushStroke->paint();
}

double BrushPreviewProvider::scale() const 
{
    if (!_presenter || !_presenter->_initHelper.isInitialized()) return 1;
    else return _presenter->_viewPort->zoom();
}

QColor BrushPreviewProvider::color() const
{
    return QColor();
//     if (!_presenter || !_presenter->_initHelper.isInitialized()) return QColor();
//     else return _presenter->_mainEditor->colorSelection().color1().color();
}
