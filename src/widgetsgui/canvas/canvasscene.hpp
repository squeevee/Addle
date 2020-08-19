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

#include "layeritem.hpp"

//#include "interfaces/presenters/icanvaspresenter.hpp"

namespace Addle {

class ICanvasPresenter;
class CanvasItem;
class ADDLE_WIDGETSGUI_EXPORT CanvasScene : public QGraphicsScene
{
    Q_OBJECT
public:
    CanvasScene(ICanvasPresenter& presenter, QObject* parent = nullptr);
    virtual ~CanvasScene() = default;

    bool event(QEvent* e) override;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);

private slots:
    void layersUpdated();
    
private:
    CanvasItem* _canvasItem;

    ICanvasPresenter& _presenter;
};

} // namespace Addle

#endif // CANVASSCENE_HPP