#ifndef CANVASITEM_HPP
#define CANVASITEM_HPP

#include <QHash>
#include <QObject>
#include <QPixmap>
#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>

class IDocumentPresenter;
class DocBackgroundItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    DocBackgroundItem(IDocumentPresenter& presenter);
    virtual ~DocBackgroundItem() = default;

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPixmap _texture;
    QColor _backgroundColor;
    QRectF _rect;

    IDocumentPresenter& _presenter;
};

#endif // CANVASITEM_HPP