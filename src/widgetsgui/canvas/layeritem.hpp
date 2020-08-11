#ifndef LAYERITEM_HPP
#define LAYERITEM_HPP

#include "compat.hpp"
#include <QObject>
#include <QGraphicsItem>

namespace Addle {

class ILayerPresenter;
class ADDLE_WIDGETSGUI_EXPORT LayerItem: public QObject, public QGraphicsItem
{
    Q_OBJECT 
public:
    LayerItem(ILayerPresenter& presenter);
    virtual ~LayerItem() = default; 

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private slots: 
    void onRenderChanged(QRect area);

private: 

    ILayerPresenter& _presenter;
};

} // namespace Addle

#endif // LAYERITEM_HPP