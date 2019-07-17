#include "canvasitem.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>

void CanvasItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* widget)
{
    painter->drawPixmap(option->rect, _presenter->getRender());
}

QRectF CanvasItem::boundingRect() const
{
    return QRectF(QPointF(), _presenter->getSize());
}