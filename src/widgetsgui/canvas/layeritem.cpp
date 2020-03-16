#include "layeritem.hpp"

#include "utilities/qtextensions/qobject.hpp"
#include "utilities/mathutils.hpp"

#include <QStyleOptionGraphicsItem>

#include "interfaces/presenters/ilayerpresenter.hpp"

LayerItem::LayerItem(ILayerPresenter& presenter)
    : _presenter(presenter)
{
    setFlags(
        {
            QGraphicsItem::ItemUsesExtendedStyleOption,
            QGraphicsItem::ItemSendsGeometryChanges
        }
    );
    connect_interface(
        &_presenter,
        SIGNAL(renderChanged(QRect)),
        this, SLOT(onRenderChanged(QRect))
    );
}

QRectF LayerItem::boundingRect() const
{
    return _presenter.getDocumentPresenter()->getRect();
}

void LayerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    _presenter.render(*painter, coarseBoundRect(option->exposedRect));
}

void LayerItem::onRenderChanged(QRect area)
{
    update(area);
}