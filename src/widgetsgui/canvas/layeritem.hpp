#ifndef LAYERITEM_HPP
#define LAYERITEM_HPP

#include <QObject>
#include <QGraphicsItem>

class ILayerPresenter;
class LayerItem: public QObject, public QGraphicsItem
{
    Q_OBJECT 
public:
    LayerItem(ILayerPresenter& presenter);
    virtual ~LayerItem() = default; 

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots: 
    void onPresenterUpdated(QRect area);

private: 

    ILayerPresenter& _presenter;
};

#endif // LAYERITEM_HPP