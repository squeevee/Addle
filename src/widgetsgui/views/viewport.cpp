#include "viewport.hpp"

#include <QPaintEngine>

#include <QLayout>
#include <QGridLayout>
#include <QGraphicsPixmapItem>

#include <QResizeEvent>

#include "common/servicelocator.hpp"
#include "common/utilities/qt_extensions/qobject.hpp"

#include "common/interfaces/views/icanvasview.hpp"

void ViewPort::initialize(IViewPortPresenter* presenter)
{
    _initHelper.initializeBegin();

    QGraphicsView::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QGraphicsView::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QFrame::setFrameShape(QFrame::Shape::StyledPanel);

    _presenter = presenter;
    _canvasPresenter = _presenter->getCanvasPresenter();

    QGraphicsScene* scene = dynamic_cast<QGraphicsScene*>(_canvasPresenter->getView());
    QGraphicsView::setScene(scene);

    connect_interface(_presenter, SIGNAL(transformsChanged()), this, SLOT(onPresenterTransformsChanged()));

    _initHelper.initializeEnd();
}

void ViewPort::resizeEvent(QResizeEvent *event)
{
    _initHelper.assertInitialized();
    _presenter->setSize(QWidget::contentsRect().size());
}

void ViewPort::onPresenterTransformsChanged()
{
    _initHelper.assertInitialized();
    QTransform fromCanvas = _presenter->getFromCanvasTransform();

    MouseEventBlocker blocker;
    QGraphicsView::scene()->installEventFilter(&blocker);
    QGraphicsView::setTransform(fromCanvas);
    QGraphicsView::setSceneRect(_presenter->getSceneRect());
    QGraphicsView::scene()->removeEventFilter(&blocker);
}

bool MouseEventBlocker::eventFilter(QObject* object, QEvent* event)
{
    return event->type() == QEvent::Type::GraphicsSceneMouseMove;
}