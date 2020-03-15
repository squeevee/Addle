#include "QtDebug"

#include <QGraphicsView>
#include "utilities/qtextensions/qobject.hpp"
#include "canvasscene.hpp"
#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"
#include <QGraphicsSceneMouseEvent>

#include "interfaces/presenters/icanvaspresenter.hpp"

#include "canvasitem.hpp"

CanvasScene::CanvasScene(ICanvasPresenter& presenter, QObject* parent)
    : QGraphicsScene(parent), _presenter(presenter)
{
    _presenter = presenter;
    _canvasItem = new CanvasItem(presenter);

    addItem(_canvasItem);
    _canvasItem->setPos(QPoint(0,0));

    update();
}

void CanvasScene::update()
{
}

void CanvasScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (mouseEvent->isAccepted())
        return;

    // IToolPresenter* tool = _presenter->getCurrentToolPresenter();
    // if (tool)
    // {
    //     mouseEvent->accept();
    //     tool->pointerEngage(mouseEvent->scenePos());
    // }
}

void CanvasScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (mouseEvent->isAccepted())
        return;

    // IToolPresenter* tool = _presenter->getCurrentToolPresenter();
    // if (tool)
    // {
    //     mouseEvent->accept();
    //     tool->pointerMove(mouseEvent->scenePos());
    // }
}

void CanvasScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (mouseEvent->isAccepted())
        return;

    // IToolPresenter* tool = _presenter->getCurrentToolPresenter();
    // if (tool)
    // {
    //     mouseEvent->accept();
    //     tool->pointerDisengage(mouseEvent->scenePos());
    // }
}
