#include "layerview.hpp"

#include "utilities/qt_extensions/qobject.hpp"
#include "utilities/mathutils.hpp"

#include <QStyleOptionGraphicsItem>

void LayerView::initialize(ILayerPresenter* presenter)
{
    _initHelper.initializeBegin();

    _presenter = presenter;

    connect_interface(_presenter, SIGNAL(renderChanged(QRect)), this, SLOT(onRenderChanged(QRect)));

    _initHelper.initializeEnd();
}

QRectF LayerView::boundingRect() const
{
    return _presenter->getCanvasBounds();
}

void LayerView::paint(QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    _presenter->render(*painter, coarseBoundRect(option->exposedRect));
}

void LayerView::onRenderChanged(QRect area)
{
    update(area);
}