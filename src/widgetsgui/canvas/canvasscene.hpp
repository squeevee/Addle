/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef CANVASSCENE_HPP
#define CANVASSCENE_HPP

#include "compat.hpp"
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>

#include "interfaces/services/ifactory.hpp"
#include "interfaces/rendering/irenderer.hpp"

#include "layeritem.hpp"

//#include "interfaces/presenters/icanvaspresenter.hpp"

namespace Addle {

class IDocumentPresenter;
class ICanvasPresenter;
class DocumentItem;
class ADDLE_WIDGETSGUI_EXPORT CanvasScene : public QGraphicsScene
{
    Q_OBJECT
public:
    CanvasScene(ICanvasPresenter& presenter, 
        const IFactory<IRenderer>& rendererFactory, QObject* parent = nullptr);
    virtual ~CanvasScene() = default;

    bool event(QEvent* e) override;

public slots:
    void onDocumentChanged(QSharedPointer<Addle::IDocumentPresenter> document);
    
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;

private slots:
    void layersUpdated();
    
private:
    DocumentItem* _documentItem = nullptr;

    ICanvasPresenter& _presenter;
    const IFactory<IRenderer>& _rendererFactory;
};

} // namespace Addle

#endif // CANVASSCENE_HPP
