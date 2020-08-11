#include "layeritem.hpp"

#include "utilities/qtextensions/qobject.hpp"
#include "utils.hpp"

#include <QStyleOptionGraphicsItem>

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/rendering/irenderstack.hpp"

using namespace Addle;

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
        &_presenter.renderStack(),
        SIGNAL(changed(QRect)),
        this, SLOT(onRenderChanged(QRect))
    );
}

QRectF LayerItem::boundingRect() const
{
    return _presenter.documentPresenter()->rect();
}

void LayerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //assert painter
    
    RenderData data(coarseBoundRect(option->exposedRect), painter);
    _presenter.renderStack().render(data);
}

void LayerItem::onRenderChanged(QRect area)
{
    update(area);
}