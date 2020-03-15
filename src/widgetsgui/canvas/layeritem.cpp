#include "layeritem.hpp"

#include "utilities/qtextensions/qobject.hpp"
#include "utilities/mathutils.hpp"

#include <QStyleOptionGraphicsItem>

LayerItem::LayerItem(QWeakPointer<ILayerPresenter> presenter)
    : _presenter(presenter)
{
    auto s_presenter = _presenter.toStrongRef();

    connect_interface(
        s_presenter.data(),
        SIGNAL(renderChanged(QRect)),
        this, SLOT(onRenderChanged(QRect))
    );
}

QRectF LayerItem::boundingRect() const
{
    auto s_presenter = _presenter.toStrongRef();
    //boo
    return QRectF();//QRect(QPoint(), s_presenter->getDocumentPresenter()->getCanvasSize());
}

void LayerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto s_presenter = _presenter.toStrongRef();

    s_presenter->render(*painter, coarseBoundRect(option->exposedRect));
}

void LayerItem::onRenderChanged(QRect area)
{
    update(area);
}