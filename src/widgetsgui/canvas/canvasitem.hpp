#ifndef CANVASITEM_HPP
#define CANVASITEM_HPP

#include <QHash>
#include <QObject>
#include <QGraphicsItemGroup>

class ICanvasPresenter;
class ILayerPresenter;
class LayerItem;
class CanvasItem : public QObject, public QGraphicsItemGroup 
{
    Q_OBJECT
public:
    CanvasItem(ICanvasPresenter& presenter);
    virtual ~CanvasItem() = default;

private:

    QHash<ILayerPresenter*, LayerItem*> _layers;

    QGraphicsItemGroup* _layerGroup;
    QGraphicsRectItem* _background;

    ICanvasPresenter& _presenter;
};

#endif // CANVASITEM_HPP