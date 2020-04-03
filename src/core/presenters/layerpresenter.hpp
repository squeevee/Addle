#ifndef LAYERPRESENTER_HPP
#define LAYERPRESENTER_HPP

#include <QObject>
#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/rendering/irenderstep.hpp"
#include "utilities/initializehelper.hpp"

class LayerPresenter;
class LayerPresenterRenderStep : public QObject, public IRenderStep
{
    Q_OBJECT
    Q_INTERFACES(IRenderStep)
public:
    LayerPresenterRenderStep(LayerPresenter& owner) : _owner(owner) { }
    virtual ~LayerPresenterRenderStep() = default;

    void before(RenderData& data);
    void after(RenderData& data);

signals: 
    void changed(QRect area);

private:
    LayerPresenter& _owner;
};

class LayerPresenter: public QObject, public ILayerPresenter
{
    Q_OBJECT
public:
    LayerPresenter() : _initHelper(this) { }
    virtual ~LayerPresenter();

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

    QSharedPointer<IRenderStep> _renderStep;
    QSharedPointer<IRenderStep> _rasterSurfaceStep;

    InitializeHelper<LayerPresenter> _initHelper;

    friend class LayerPresenterRenderStep;
};


#endif // LAYERPRESENTER_HPP