#include "QtDebug"

#include <QGraphicsView>
#include "utilities/qt_extensions/qobject.hpp"
#include "canvasview.hpp"
#include "interfaces/presenters/tools/itoolpresenter.hpp"
#include <QGraphicsSceneMouseEvent>

void CanvasView::initialize(IDocumentPresenter* presenter)
{
    _initHelper.initializeBegin();

    _presenter = presenter;

    connect_interface(_presenter, SIGNAL(documentChanged(QSharedPointer<IDocument>)), this, SLOT(update()));

    _initHelper.initializeEnd();
}

void CanvasView::update()
{
    _initHelper.assertInitialized();

    _layers.clear(); //leak?
    if (_layersContainer)
        QGraphicsScene::destroyItemGroup(_layersContainer);
    _layersContainer = new QGraphicsItemGroup();
    for (ILayerPresenter* layerPresenter : _presenter->getLayerPresenters())
    {
        ILayerView* layerView = layerPresenter->getView();
        _layers.append(layerView);
        _layersContainer->addToGroup(dynamic_cast<QGraphicsItem*>(layerView));
    }

    QGraphicsScene::addItem(_layersContainer);
    _layersContainer->setPos(QPointF());
}

void CanvasView::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    _initHelper.assertInitialized();

    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (mouseEvent->isAccepted())
        return;

    IToolPresenter* tool = _presenter->getCurrentToolPresenter();
    if (tool)
    {
        mouseEvent->accept();
        tool->pointerEngage(mouseEvent->scenePos());
    }
}

void CanvasView::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    _initHelper.assertInitialized();
    
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (mouseEvent->isAccepted())
        return;

    IToolPresenter* tool = _presenter->getCurrentToolPresenter();
    if (tool)
    {
        mouseEvent->accept();
        tool->pointerMove(mouseEvent->scenePos());
    }
}

void CanvasView::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    _initHelper.assertInitialized();

    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (mouseEvent->isAccepted())
        return;

    IToolPresenter* tool = _presenter->getCurrentToolPresenter();
    if (tool)
    {
        mouseEvent->accept();
        tool->pointerDisengage(mouseEvent->scenePos());
    }
}
