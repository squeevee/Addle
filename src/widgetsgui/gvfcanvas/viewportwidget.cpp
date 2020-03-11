#include "viewportwidget.hpp"
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

#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

#include "canvasscene.hpp"

using namespace GVFCanvas;

void ViewPortWidget::initialize(IViewPortPresenter* presenter, ICanvasPresenter* canvasPresenter)
{
    _initHelper.initializeBegin();

    _presenter = presenter;

    QGraphicsView::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QGraphicsView::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QFrame::setFrameShape(QFrame::Shape::StyledPanel);
    QGraphicsView::setTransformationAnchor(ViewportAnchor::AnchorViewCenter);

    _canvasScene = new CanvasScene(*canvasPresenter, this);
    setScene(_canvasScene);

    //QGraphicsScene* scene = dynamic_cast<QGraphicsScene*>(_presenter.getDocumentPresenter()->getCanvasView());
    //QGraphicsView::setScene(scene);

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

void ViewPortWidget::resizeEvent(QResizeEvent *event)
{
    _initHelper.assertInitialized();

    //auto s_presenter = _presenter.toStrongRef();
    _presenter->setSize(QWidget::contentsRect().size());
}

void ViewPortWidget::moveEvent(QMoveEvent *event)
{
    _initHelper.assertInitialized();

    //auto s_presenter = _presenter.toStrongRef();
    _presenter->setGlobalOffset(QWidget::mapToGlobal(QWidget::contentsRect().topLeft()));
}

void ViewPortWidget::onPresenterTransformsChanged()
{
    _initHelper.assertInitialized();

    //auto s_presenter = _presenter.toStrongRef();

    if (!scene())
        return;

    QRectF bound = _presenter->getOntoCanvasTransform().mapRect(QRectF(QPointF(), _presenter->getSize()));

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