#ifndef CANVASITEM_H
#define CANVASITEM_H

#include <QGraphicsItem>

#include "interfaces/presenters/icanvaspresenter.h"

class CanvasItem : public QGraphicsItem
{
public:
    CanvasItem(ICanvasPresenter* presenter) : _presenter(presenter) {}
    virtual ~CanvasItem() {}

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* widget);
    QRectF boundingRect() const;

private:
    ICanvasPresenter* _presenter;

};

#endif //CANVASITEM_H