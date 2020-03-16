#include "docbackgrounditem.hpp"

#include "interfaces/presenters/idocumentpresenter.hpp"

#include "widgetsgui/utilities/guiutils.hpp"

#include <QStyleOptionGraphicsItem>

DocBackgroundItem::DocBackgroundItem(IDocumentPresenter& presenter)
    : _presenter(presenter)
{
    setFlags(
        {
            QGraphicsItem::ItemUsesExtendedStyleOption,
            QGraphicsItem::ItemSendsGeometryChanges
        }
    );

    _texture = checkerBoardTexture(35, Qt::lightGray, Qt::gray);
}

QRectF DocBackgroundItem::boundingRect() const
{
    return _presenter.getRect();
}

void DocBackgroundItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPainterPath path;
    path.addRect(QRectF(_presenter.getRect()).intersected(option->exposedRect));
    path = painter->transform().map(path);

    QColor backgroundColor = _presenter.getBackgroundColor();
    
    painter->save();
    painter->setTransform(QTransform());

    if (backgroundColor.alpha() < 255)
        painter->fillPath(path, _texture);

    painter->fillPath(path, backgroundColor);
    painter->restore();
}