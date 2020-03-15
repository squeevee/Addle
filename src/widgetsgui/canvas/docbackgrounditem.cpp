#include "docbackgrounditem.hpp"

#include "interfaces/presenters/idocumentpresenter.hpp"

#include "widgetsgui/utilities/guiutils.hpp"

#include <QGraphicsRectItem>

DocBackgroundItem::DocBackgroundItem(IDocumentPresenter& presenter)
    : _presenter(presenter)
{
    _texture = checkerBoardTexture(35, Qt::lightGray, Qt::gray);
    
    _rect = QRectF(QPointF(), presenter.getSize());
    _backgroundColor = presenter.getBackgroundColor();
}

QRectF DocBackgroundItem::boundingRect() const
{
    return _rect;
}

void DocBackgroundItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (_backgroundColor.alpha() < 255)
    {
        QPainterPath path;
        path.addRect(_rect);
        path = painter->transform().map(path);
        
        painter->save();

        painter->setTransform(QTransform());
        painter->fillPath(path, _texture);

        painter->restore();
    }

    painter->fillRect(_rect, _backgroundColor);
}