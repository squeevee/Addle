#ifndef CANVASSCENE_HPP
#define CANVASSCENE_HPP

#include "compat.hpp"
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>

#include "layeritem.hpp"

//#include "interfaces/presenters/icanvaspresenter.hpp"

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
    
private:
    CanvasItem* _canvasItem;

    ICanvasPresenter& _presenter;
};

#endif // CANVASSCENE_HPP