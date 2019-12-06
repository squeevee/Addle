#ifndef CANVASVIEW_HPP
#define CANVASVIEW_HPP

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>

#include "interfaces/presenters/idocumentpresenter.hpp"
#include "interfaces/views/icanvasview.hpp"
#include "interfaces/views/ilayerview.hpp"

#include "utilities/initializehelper.hpp"

class CanvasView : public QGraphicsScene, public ICanvasView
{
    Q_OBJECT
public:
    CanvasView() : _initHelper(this) { }
    virtual ~CanvasView() = default;

    void initialize(IDocumentPresenter* presenter);

public slots:
    void update();

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);

private:
    IDocumentPresenter* _presenter;
    InitializeHelper<CanvasView> _initHelper;

    QList<ILayerView*> _layers;
    QGraphicsItemGroup* _layersContainer = nullptr;
};

#endif // CANVASVIEW_HPP