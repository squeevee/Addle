#ifndef LAYERPRESENTER_HPP
#define LAYERPRESENTER_HPP

#include "compat.hpp"
#include <QObject>
#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/rendering/irenderstep.hpp"
#include "utilities/initializehelper.hpp"
namespace Addle {

class LayerPresenterRenderStep;
class ADDLE_CORE_EXPORT LayerPresenter: public QObject, public ILayerPresenter
{
    Q_OBJECT
public:
    LayerPresenter() : _initHelper(this) { }
    virtual ~LayerPresenter();

    void initialize(IDocumentPresenter* documentPresenter, QSharedPointer<ILayer> model);
    IDocumentPresenter* getDocumentPresenter() { _initHelper.check(); return _documentPresenter; }

    QSharedPointer<ILayer> getModel() { _initHelper.check(); return _model; }

    QString name() const { _initHelper.check(); return _name; }
    void setName(QString name);

    IRenderStack& getRenderStack();

signals:
    void nameChanged(QString name);
    void updated(QRect area);

private slots:
    void onRasterChanged(QRect area);

private:
    IDocumentPresenter* _documentPresenter;
    IRenderStack* _renderStack = nullptr;
    
    QSharedPointer<ILayer> _model;

    QString _name;

    QSharedPointer<IRenderStep> _renderStep;
    QSharedPointer<IRenderStep> _rasterSurfaceStep;

    InitializeHelper<LayerPresenter> _initHelper;

    friend class LayerPresenterRenderStep;
};

class LayerPresenterRenderStep : public QObject, public IRenderStep
{
    Q_OBJECT
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
} // namespace Addle

#endif // LAYERPRESENTER_HPP