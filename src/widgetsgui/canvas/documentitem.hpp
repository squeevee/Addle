#pragma once

#include <memory>

#include "compat.hpp"
#include <QGraphicsItem>

#include "interfaces/rendering/irenderer.hpp"

namespace Addle {
 
class IDocumentPresenter;
class ADDLE_WIDGETSGUI_EXPORT DocumentItem : public QGraphicsItem
{
public:
    DocumentItem(QSharedPointer<IDocumentPresenter>, std::unique_ptr<IRenderer> renderer);
    virtual ~DocumentItem() = default;
    
    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) override;
    
private:
    QSharedPointer<IDocumentPresenter> _presenter;
    std::unique_ptr<IRenderer> _renderer;
};
    
}
