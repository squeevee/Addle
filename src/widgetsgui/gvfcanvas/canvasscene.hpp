#ifndef CANVASSCENE_HPP
#define CANVASSCENE_HPP

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>

#include "layeritem.hpp"

#include "interfaces/presenters/icanvaspresenter.hpp"

namespace GVFCanvas
{
    class CanvasScene : public QGraphicsScene
    {
        Q_OBJECT
    public:
        CanvasScene(ICanvasPresenter& presenter, QObject* parent = nullptr);
        virtual ~CanvasScene() = default;

    public slots:
        void update();

    protected:
        void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
        void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);

    private:
        ICanvasPresenter& _presenter;

        QList<LayerItem*> _layers;
        QGraphicsItemGroup* _layersContainer = nullptr;
    };
}

#endif // CANVASSCENE_HPP