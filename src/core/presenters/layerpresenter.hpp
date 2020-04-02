#ifndef LAYERPRESENTER_HPP
#define LAYERPRESENTER_HPP

#include <QObject>
#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/rendering/irenderstep.hpp"
#include "utilities/initializehelper.hpp"

class LayerPresenter;
namespace LayerPresenterAux
{

class RasterRenderStep : public QObject, public IRenderStep
{
    Q_OBJECT
    Q_INTERFACES(IRenderStep)
public:
    RasterRenderStep(LayerPresenter& owner);
    virtual ~RasterRenderStep() = default;

    void before(RenderData& data);
    void after(RenderData& data);

signals: 
    void changed(QRect area);

private:
    LayerPresenter& _owner;
};

} //namespace LayerPresenterRenderSteps

class LayerPresenter: public QObject, public ILayerPresenter
{
    Q_OBJECT
public:
    LayerPresenter() : _initHelper(this) { }
    virtual ~LayerPresenter() = default;

    void initialize(IDocumentPresenter* documentPresenter, QWeakPointer<ILayer> model);
    IDocumentPresenter* getDocumentPresenter() { _initHelper.check(); return _documentPresenter; }

    QWeakPointer<ILayer> getModel() { _initHelper.check(); return _model; }

    IRenderStack& getRenderStack();

signals:
    void updated(QRect area);

private slots:
    void onRasterChanged(QRect area);

private:
    IDocumentPresenter* _documentPresenter;
    IRenderStack* _renderStack = nullptr;
    
    QWeakPointer<ILayer> _model;

    QSharedPointer<LayerPresenterAux::RasterRenderStep> _rasterRenderStep;

    InitializeHelper<LayerPresenter> _initHelper;

    friend class LayerPresenterAux::RasterRenderStep;
};


#endif // LAYERPRESENTER_HPP