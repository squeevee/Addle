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

    void initialize(IDocumentPresenter* documentPresenter, QWeakPointer<ILayer> model);
    IDocumentPresenter* getDocumentPresenter() { _initHelper.assertInitialized(); return _documentPresenter; }

    QWeakPointer<ILayer> getModel() { _initHelper.assertInitialized(); return _model; }

    QRect getCanvasBounds() { _initHelper.assertInitialized(); return QRect(QPoint(), _documentPresenter->getCanvasSize()); }
    void render(QPainter& painter, QRect area);

signals: 
    void renderChanged(QRect area);

private slots:
    void onRasterChanged(QRect area);

private:
    IDocumentPresenter* _documentPresenter;
    
    QWeakPointer<ILayer> _model;

    InitializeHelper<LayerPresenter> _initHelper;
};

#endif // LAYERPRESENTER_HPP