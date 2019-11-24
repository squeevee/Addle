#ifndef CANVASVIEW_HPP
#define CANVASVIEW_HPP

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include "common/interfaces/presenters/icanvaspresenter.hpp"
#include "common/interfaces/views/icanvasview.hpp"

#include "common/utilities/initializehelper.hpp"

class CanvasView : public QGraphicsScene, public ICanvasView
{
    Q_OBJECT
public:
    CanvasView() : _initHelper(this)
    {
    }
    virtual ~CanvasView() = default;

    void initialize(ICanvasPresenter* presenter);

public slots:
    void update();

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);

private:
    ICanvasPresenter* _presenter;
    QGraphicsPixmapItem* _render = nullptr;
    InitializeHelper<CanvasView> _initHelper;
};

#endif // CANVASVIEW_HPP