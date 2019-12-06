#include "viewport.hpp"
#include <QtDebug>

#include <QPaintEngine>

#include <QLayout>
#include <QGridLayout>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

#include <QPolygonF>
#include <QResizeEvent>

#include "servicelocator.hpp"
#include "utilities/qt_extensions/qobject.hpp"

#include "interfaces/views/icanvasview.hpp"
#include "interfaces/presenters/tools/itoolpresenter.hpp"

void ViewPort::initialize(IViewPortPresenter* presenter)
{
    _initHelper.initializeBegin();

    QGraphicsView::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QGraphicsView::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QFrame::setFrameShape(QFrame::Shape::StyledPanel);
    QGraphicsView::setTransformationAnchor(ViewportAnchor::AnchorViewCenter);

    _presenter = presenter;

    QGraphicsScene* scene = dynamic_cast<QGraphicsScene*>(_presenter->getDocumentPresenter()->getCanvasView());
    QGraphicsView::setScene(scene);

    connect_interface(
        _presenter,
        SIGNAL(transformsChanged()),
        this,
        SLOT(onPresenterTransformsChanged()),
        // QueuedConnection is important for avoiding jitter with tools that move
        // the viewport, on X11. X11 mouse events are asynchronous so changing
        // the transforms inside the mouse event handler causes Qt to calculate
        // positions based on mismatched mouse positions and transforms.
        Qt::ConnectionType::QueuedConnection
    );

    _initHelper.initializeEnd();
}

void ViewPort::resizeEvent(QResizeEvent *event)
{
    _initHelper.assertInitialized();
    _presenter->setViewPortSize(QWidget::contentsRect().size());
}

void ViewPort::moveEvent(QMoveEvent *event)
{
    _presenter->setGlobalOffset(QWidget::mapToGlobal(QWidget::contentsRect().topLeft()));
}

void ViewPort::onPresenterTransformsChanged()
{
    _initHelper.assertInitialized();

    QRectF bound = _presenter->getOntoCanvasTransform().mapRect(QRectF(QPointF(), _presenter->getViewPortSize()));

    // Moving the scene while the mouse is pressed can cause additional mouse
    // events to be sent with positions based on outdated transforms, and this
    // causes jitter when a tool is moving the viewport. We block these events
    // until the transform is done.
    MouseEventBlocker blocker;
    QGraphicsView::scene()->installEventFilter(&blocker);

    QGraphicsView::setSceneRect(bound);
    QGraphicsView::setTransform(_presenter->getFromCanvasTransform());
    QGraphicsView::centerOn(_presenter->getPosition());

    QGraphicsView::scene()->removeEventFilter(&blocker);
}

bool MouseEventBlocker::eventFilter(QObject* object, QEvent* event)
{
    return event->type() == QEvent::Type::GraphicsSceneMouseMove;
}