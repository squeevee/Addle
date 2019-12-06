#ifndef LAYERPRESENTER_HPP
#define LAYERPRESENTER_HPP

#include <QObject>
#include "interfaces/presenters/ilayerpresenter.hpp"
#include "utilities/initializehelper.hpp"

class LayerPresenter: public QObject, public ILayerPresenter
{
    Q_OBJECT
public:
    LayerPresenter() : _initHelper(this) { }
    virtual ~LayerPresenter() = default;

    void initialize(IDocumentPresenter* documentPresenter, ILayer* layer);
    IDocumentPresenter* getDocumentPresenter() { _initHelper.assertInitialized(); return _documentPresenter; }

    ILayerView* getView();

    ILayer* getModel() { _initHelper.assertInitialized(); return _layer; }

    void setRasterOperation(IRasterOperation* operation)  { _rasterOperation = operation; }
    void unsetRasterOperation() { _rasterOperation = nullptr; }

    QRect getCanvasBounds() { _initHelper.assertInitialized(); return QRect(QPoint(), _documentPresenter->getCanvasSize()); }
    void render(QPainter& painter, QRect area);

signals: 
    void renderChanged(QRect area);

private:
    IDocumentPresenter* _documentPresenter;

    ILayerView* _view = nullptr;
    
    ILayer* _layer;

    IRasterOperation* _rasterOperation = nullptr;

    InitializeHelper<LayerPresenter> _initHelper;
};

#endif // LAYERPRESENTER_HPP