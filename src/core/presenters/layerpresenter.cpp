#include "layerpresenter.hpp"
#include "servicelocator.hpp"
#include "interfaces/editing/surfaces/irastersurface.hpp"
#include "interfaces/rendering/irenderstack.hpp"

#include "utilities/qtextensions/qobject.hpp"

void LayerPresenterRenderStep::before(RenderData& data)
{
}

void LayerPresenterRenderStep::after(RenderData& data)
{
}

LayerPresenter::~LayerPresenter()
{
    delete _renderStack;
}

void LayerPresenter::initialize(IDocumentPresenter* documentPresenter, QWeakPointer<ILayer> model)
{
    _initHelper.initializeBegin();

    _documentPresenter = documentPresenter;
    _model = model;

    auto s_model = _model.toStrongRef();

    connect_interface(
        s_model->getRasterSurface(),
        SIGNAL(changed(QRect)),
        this,
        SLOT(onRasterChanged(QRect)),
        Qt::QueuedConnection
    );

    _renderStep = QSharedPointer<IRenderStep>(new LayerPresenterRenderStep(*this));
    _rasterSurfaceStep = QSharedPointer<IRenderStep>(s_model->getRasterSurface()->makeRenderStep());

    _initHelper.initializeEnd();
}

IRenderStack& LayerPresenter::getRenderStack()
{
    _initHelper.check();


    if (!_renderStack)
    {
        auto s_model = _model.toStrongRef();
        _renderStack = ServiceLocator::make<IRenderStack>(QList<QWeakPointer<IRenderStep>>({
            _renderStep,
            _rasterSurfaceStep
        }));
    }

    return *_renderStack;
}

void LayerPresenter::onRasterChanged(QRect area)
{
    emit updated(area);
}
