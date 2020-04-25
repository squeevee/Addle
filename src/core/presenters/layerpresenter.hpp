#ifndef LAYERPRESENTER_HPP
#define LAYERPRESENTER_HPP

#include "compat.hpp"
#include <QObject>
#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/rendering/irenderstep.hpp"
#include "utilities/initializehelper.hpp"

class LayerPresenterRenderStep;
class ADDLE_CORE_EXPORT LayerPresenter: public QObject, public ILayerPresenter
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

class LayerPresenterRenderStep : public QObject, public IRenderStep
{
    Q_OBJECT
    Q_INTERFACES(IRenderStep)
public:
    LayerPresenterRenderStep(LayerPresenter& owner) : _owner(owner) { }
    virtual ~LayerPresenterRenderStep() = default;

    void onPush(RenderData& data);
    void onPop(RenderData& data);

    QRect getAreaHint() { return QRect(); }

signals: 
    void changed(QRect area);

private:
    LayerPresenter& _owner;
};

#endif // LAYERPRESENTER_HPP