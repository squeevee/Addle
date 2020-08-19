/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef CANVASITEM_HPP
#define CANVASITEM_HPP

#include "compat.hpp"
#include <QHash>
#include <QObject>
#include <QPixmap>
#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>

namespace Addle {

class IDocumentPresenter;
class ADDLE_WIDGETSGUI_EXPORT DocBackgroundItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    DocBackgroundItem(IDocumentPresenter& presenter);
    virtual ~DocBackgroundItem() = default;

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPixmap _texture;

    IDocumentPresenter& _presenter;
};

} // namespace Addle

#endif // CANVASITEM_HPP