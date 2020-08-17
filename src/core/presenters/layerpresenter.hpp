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
    Q_INTERFACES(Addle::ILayerPresenter)
    IAMQOBJECT_IMPL
public:
    virtual ~LayerPresenter();

    void initialize(IDocumentPresenter* documentPresenter, QSharedPointer<ILayer> model);
    IDocumentPresenter* documentPresenter() { _initHelper.check(); return _documentPresenter; }

    QSharedPointer<ILayer> model() { _initHelper.check(); return _model; }

    QString name() const { _initHelper.check(); return _name; }
    void setName(QString name);

    IRenderStack& renderStack();

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

    InitializeHelper _initHelper;

    friend class LayerPresenterRenderStep;
};

class LayerPresenterRenderStep : public QObject, public IRenderStep
{
    Q_OBJECT
    Q_INTERFACES(Addle::IRenderStep)
    IAMQOBJECT_IMPL
public:
    LayerPresenterRenderStep(LayerPresenter& owner) : _owner(owner) { }
    virtual ~LayerPresenterRenderStep() = default;

    void onPush(RenderData& data);
    void onPop(RenderData& data);

    QRect areaHint() { return QRect(); }

signals: 
    void changed(QRect area);

private:
    LayerPresenter& _owner;
};
} // namespace Addle

#endif // LAYERPRESENTER_HPP