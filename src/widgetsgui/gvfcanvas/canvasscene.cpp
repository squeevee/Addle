#include "QtDebug"

#include <QGraphicsView>
#include "utilities/qt_extensions/qobject.hpp"
#include "canvasscene.hpp"
#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"
#include <QGraphicsSceneMouseEvent>

using namespace GVFCanvas;

CanvasScene::CanvasScene(ICanvasPresenter& presenter, QObject* parent)
    : QGraphicsScene(parent), _presenter(presenter)
{
    _presenter = presenter;

    // connect_interface(_presenter, SIGNAL(documentChanged(QSharedPointer<IDocument>)), this, SLOT(update()));

    update();
}

void CanvasScene::update()
{
    _layers.clear(); //leak?
    if (_layersContainer)
        QGraphicsScene::destroyItemGroup(_layersContainer);
    _layersContainer = new QGraphicsItemGroup();
    // for (ILayerPresenter* layerPresenter : _presenter->getLayerPresenters())
    // {
    //     //ILayerView* layerView = layerPresenter->getView();
    //     _layers.append(layerView);
    //     _layersContainer->addToGroup(dynamic_cast<QGraphicsItem*>(layerView));
    // }

    QGraphicsScene::addItem(_layersContainer);
    _layersContainer->setPos(QPointF());
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
