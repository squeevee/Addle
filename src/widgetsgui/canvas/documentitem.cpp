#include <QStyleOptionGraphicsItem>
#include "documentitem.hpp"

#include "interfaces/presenters/idocumentpresenter.hpp"
#include "utilities/math.hpp"

using namespace Addle;

DocumentItem::DocumentItem(QSharedPointer<IDocumentPresenter> presenter, std::unique_ptr<IRenderer> renderer)
    : _presenter(std::move(presenter)),
    _renderer(std::move(renderer))
{
    assert(_presenter && _renderer);
    setFlags(QGraphicsItem::ItemUsesExtendedStyleOption);
}

QRectF DocumentItem::boundingRect() const
{
    return _presenter->rect();
}

void DocumentItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*)
{
    assert(painter);
    _renderer->render(*painter, coarseBoundRect(option->exposedRect));
}
