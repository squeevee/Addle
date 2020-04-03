#include "layeritem.hpp"

#include "utilities/qtextensions/qobject.hpp"
#include "utilities/mathutils.hpp"

#include <QStyleOptionGraphicsItem>

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/rendering/irenderstack.hpp"

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
        &_presenter.getRenderStack(),
        SIGNAL(changed(QRect)),
        this, SLOT(onRenderChanged(QRect))
    );
}

QRectF LayerItem::boundingRect() const
{
    return _presenter.getDocumentPresenter()->getRect();
}

void LayerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //assert painter
    
    RenderData data(coarseBoundRect(option->exposedRect), painter);
    _presenter.getRenderStack().render(data);
}

void LayerItem::onRenderChanged(QRect area)
{
    update(area);
}