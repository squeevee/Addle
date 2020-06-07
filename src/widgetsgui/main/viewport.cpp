#include "viewport.hpp"
#include <QtDebug>

#include <QPaintEngine>
#include <QtGlobal>

#include <QLayout>
#include <QGridLayout>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

#include <QPolygonF>
#include <QResizeEvent>

#include "servicelocator.hpp"
#include "utilities/qtextensions/qobject.hpp"
#include "utilities/mathutils.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"

#include "widgetsgui/utilities/graphicsmouseeventblocker.hpp"
#include "utilities/presenter/propertybinding.hpp"
#include "widgetsgui/utilities/guiutils.hpp"
#include "widgetsgui/canvas/canvasscene.hpp"

#include <QScrollBar>

ViewPort::ViewPort(IViewPortPresenter* presenter)
{
    _presenter = presenter;
    _mainEditorPresenter = _presenter->getMainEditorPresenter();
    _canvasPresenter = _mainEditorPresenter->getCanvasPresenter();

    QGraphicsView::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QGraphicsView::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QGraphicsView::setTransformationAnchor(ViewportAnchor::AnchorViewCenter);

    verticalScrollBar()->disconnect(this);
    horizontalScrollBar()->disconnect(this);

    QFrame::setFrameShape(QFrame::Shape::StyledPanel);

    setMouseTracking(true);

    connect_interface(
        _presenter,
        SIGNAL(transformsChanged()),
        this,
        SLOT(onTransformsChanged()),
#ifdef Q_OS_LINUX
        // QueuedConnection is important for avoiding jitter with tools that move
        // the viewport, on X11. X11 mouse events are asynchronous so changing
        // the transforms inside the mouse event handler causes Qt to calculate
        // positions based on mismatched mouse positions and transforms.
        Qt::ConnectionType::QueuedConnection
#else
        Qt::ConnectionType::AutoConnection
#endif
    );

    connect_interface(
        _mainEditorPresenter,
        SIGNAL(documentPresenterChanged(IDocumentPresenter*)),
        this,
        SLOT(setDocument(IDocumentPresenter*))
    );

    setDocument(_mainEditorPresenter->getDocumentPresenter());

    _presenter->setHasFocus(hasFocus());
}

void ViewPort::resizeEvent(QResizeEvent *event)
{
    //auto s_presenter = _presenter.toStrongRef();
    _presenter->setSize(QAbstractScrollArea::viewport()->contentsRect().size());
}

void ViewPort::moveEvent(QMoveEvent *event)
{
    //auto s_presenter = _presenter.toStrongRef();
    _presenter->setGlobalOffset(QWidget::mapToGlobal(QWidget::contentsRect().topLeft()));
}

void ViewPort::onTransformsChanged()
{

    //auto s_presenter = _presenter.toStrongRef();

    if (!scene())
        return;

    //QPointF vptl = _cache_ontoCanvasTransform.map()

    QRectF bound = _presenter->getOntoCanvasTransform().mapRect(QRectF(QPointF(), _presenter->getSize()));

    // Moving the scene while the mouse is pressed can cause additional mouse
    // events to be sent with positions based on outdated transforms, and this
    // causes jitter when a tool is moving the viewport. We block these events
    // until the transform is done.
    GraphicsMouseEventBlocker blocker;
    QGraphicsView::scene()->installEventFilter(&blocker);

    QGraphicsView::setSceneRect(bound);
    QGraphicsView::setTransform(_presenter->getFromCanvasTransform());
    QGraphicsView::centerOn(_presenter->getPosition());

    QGraphicsView::scene()->removeEventFilter(&blocker);
}

void ViewPort::setDocument(IDocumentPresenter* documentPresenter)
{
    _documentPresenter = documentPresenter;

    if (!documentPresenter)
        return;

    _canvasScene = new CanvasScene(*_canvasPresenter, this);
    setScene(_canvasScene);
}

void ViewPort::focusInEvent(QFocusEvent* focusEvent)
{
    _presenter->setHasFocus(true);
}

void ViewPort::focusOutEvent(QFocusEvent* focusEvent)
{
    _presenter->setHasFocus(false);
}