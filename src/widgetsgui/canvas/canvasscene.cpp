#include "QtDebug"

#include <QGraphicsView>
#include "utilities/qtextensions/qobject.hpp"
#include "canvasscene.hpp"
#include "interfaces/presenters/tools/itoolpresenter.hpp"
#include <QGraphicsSceneMouseEvent>

#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "utilities/canvas/canvasmouseevent.hpp"

#include "widgetsgui/utilities/guiutils.hpp"

#include "docbackgrounditem.hpp"
#include "layeritem.hpp"

const double MINIMUM_LAYER_Z = 1;

CanvasScene::CanvasScene(ICanvasPresenter& presenter, QObject* parent)
    : QGraphicsScene(parent), _presenter(presenter)
{
    _presenter = presenter;
    
    IMainEditorPresenter& mainEditorPresenter = *_presenter.getMainEditorPresenter();

    IDocumentPresenter& documentPresenter = *mainEditorPresenter.getDocumentPresenter();
    DocBackgroundItem* background = new DocBackgroundItem(documentPresenter);

    addItem(background);

    double layerZ = MINIMUM_LAYER_Z;

    const auto layers = documentPresenter.layers();
    for (auto& node : layers)
    {
        if (!node.isValue()) continue;
        
        LayerItem* layerItem = new LayerItem(*node.asValue());
        layerItem->setZValue(layerZ);
        addItem(layerItem);

        layerZ += 1.0;
    }
}

void CanvasScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (mouseEvent->isAccepted())
        return;

    CanvasMouseEvent canvasMouseEvent = graphicsMouseToCanvasMouseEvent(mouseEvent);
    mouseEvent->setAccepted(
        qobject_interface_cast(&_presenter)->event(&canvasMouseEvent) && canvasMouseEvent.isAccepted()
    );
}

void CanvasScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (mouseEvent->isAccepted())
        return;

    CanvasMouseEvent canvasMouseEvent = graphicsMouseToCanvasMouseEvent(mouseEvent);
    mouseEvent->setAccepted(
        qobject_interface_cast(&_presenter)->event(&canvasMouseEvent) && canvasMouseEvent.isAccepted()
    );
}

void CanvasScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (mouseEvent->isAccepted())
        return;

    CanvasMouseEvent canvasMouseEvent = graphicsMouseToCanvasMouseEvent(mouseEvent);
    mouseEvent->setAccepted(
        qobject_interface_cast(&_presenter)->event(&canvasMouseEvent) && canvasMouseEvent.isAccepted()
    );
}

bool CanvasScene::event(QEvent* e)
{
    switch (e->type())
    {
    case QEvent::Enter:
        _presenter.setHasMouse(true);
        break;

    case QEvent::Leave:
        _presenter.setHasMouse(false);
        break;
    }

    return QGraphicsScene::event(e);
}
