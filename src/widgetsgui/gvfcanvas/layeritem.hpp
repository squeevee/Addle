#ifndef LAYERITEM_HPP
#define LAYERITEM_HPP

#include <QObject>
#include <QGraphicsItem>

#include "interfaces/presenters/ilayerpresenter.hpp"

namespace GVFCanvas
{
    class LayerItem: public QObject, public QGraphicsItem
    {
        Q_OBJECT 
    public:
        LayerItem(QWeakPointer<ILayerPresenter> presenter);
        virtual ~LayerItem() = default; 

        QWeakPointer<ILayerPresenter> getPresenter() { return _presenter; }

        QRectF boundingRect() const;

        void paint(QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    public slots: 
        void onRenderChanged(QRect area);

    private: 

        QWeakPointer<ILayerPresenter> _presenter;
    };
}

#endif // LAYERITEM_HPP