#include "QtDebug"

#include <QGraphicsView>
#include "utilities/qtextensions/qobject.hpp"
#include "canvasscene.hpp"
#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"
#include <QGraphicsSceneMouseEvent>

#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "docbackgrounditem.hpp"

CanvasScene::CanvasScene(ICanvasPresenter& presenter, QObject* parent)
    : QGraphicsScene(parent), _presenter(presenter)
{
    _presenter = presenter;
    
    IMainEditorPresenter& mainEditorPresenter = *_presenter.getMainEditorPresenter();

    IDocumentPresenter& documentPresenter = *mainEditorPresenter.getDocumentPresenter();
    DocBackgroundItem* background = new DocBackgroundItem(documentPresenter);

    addItem(background);
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
