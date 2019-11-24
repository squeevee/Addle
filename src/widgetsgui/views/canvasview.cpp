#include "canvasview.hpp"
#include "common/interfaces/presenters/tools/itoolpresenter.hpp"
#include <QGraphicsSceneMouseEvent>

void CanvasView::initialize(ICanvasPresenter* presenter)
{
    _initHelper.initializeBegin();

    _presenter = presenter;

    _initHelper.initializeEnd();
}

void CanvasView::update()
{
    _initHelper.assertInitialized();

    if (!_render)
    {
        _render = new QGraphicsPixmapItem(_presenter->getRender());
        QGraphicsScene::addItem(_render);
    }
    else
    {
        _render->setPixmap(_presenter->getRender());
    }

}

void CanvasView::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    _initHelper.assertInitialized();

    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (mouseEvent->isAccepted())
        return;

    IToolPresenter* tool = _presenter->getDocumentPresenter()->getCurrentTool();
    if (tool)
    {
        mouseEvent->accept();
        tool->pointerEngage(mouseEvent->screenPos());
    }
}

void CanvasView::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    _initHelper.assertInitialized();

    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (mouseEvent->isAccepted())
        return;

    IToolPresenter* tool = _presenter->getDocumentPresenter()->getCurrentTool();
    if (tool)
    {
        mouseEvent->accept();
        tool->pointerMove(mouseEvent->screenPos());
    }
}

void CanvasView::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    _initHelper.assertInitialized();

    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (mouseEvent->isAccepted())
        return;

    IToolPresenter* tool = _presenter->getDocumentPresenter()->getCurrentTool();
    if (tool)
    {
        mouseEvent->accept();
        tool->pointerDisengage(mouseEvent->screenPos());
    }
}
