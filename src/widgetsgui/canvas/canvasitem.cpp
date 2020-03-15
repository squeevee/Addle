#include "canvasitem.hpp"

#include <QGraphicsRectItem>

CanvasItem::CanvasItem(ICanvasPresenter& presenter)
    : _presenter(presenter)
{
    _background = new QGraphicsRectItem(this);

    // _background->setRect(QRect(QPoint(), _presenter.getDocumentPresenter()->getCanvasSize()));
    // _background->setBrush(_presenter.getDocumentPresenter()->getBackgroundColor());
}