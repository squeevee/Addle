#include "views/editorviewport.h"

#include <QPaintEngine>

#include <QLayout>
#include <QGridLayout>
#include <QGraphicsPixmapItem>

#include <QResizeEvent>

#include "servicelocator.h"

#include "interfaces/presenters/ieditorviewportpresenter.h"

#include "./graphicsitems/canvasitem.h"

EditorViewPort::EditorViewPort(QWidget *parent)
    :QGraphicsView(parent)
{
}

EditorViewPort::~EditorViewPort()
{
}

void EditorViewPort::initialize(IEditorViewPortPresenter* presenter)
{
    _presenter = presenter;
    _editorPresenter = _presenter->getEditorPresenter();
    _canvasPresenter = _presenter->getCanvasPresenter();

    _scene = new QGraphicsScene(this);
    setScene(_scene);

    _scene->setSceneRect(QRect(QPoint(0,0), size()));

    CanvasItem* canvasGraphicsItem = new CanvasItem(_canvasPresenter);
    _scene->addItem(canvasGraphicsItem);
}

void EditorViewPort::resizeEvent(QResizeEvent* event)
{
    _scene->setSceneRect(QRect(QPoint(0,0), event->size()));
}