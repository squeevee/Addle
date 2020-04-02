#include "layerpresenter.hpp"
#include "servicelocator.hpp"
#include "interfaces/editing/surfaces/irastersurface.hpp"
#include "interfaces/rendering/irenderstack.hpp"

#include "utilities/qtextensions/qobject.hpp"

using namespace LayerPresenterAux;

RasterRenderStep::RasterRenderStep(LayerPresenter& owner)
    : QObject(&owner), _owner(owner)
{
}

void RasterRenderStep::before(RenderData& data)
{
}

void RasterRenderStep::after(RenderData& data)
{
    auto s_model = _owner._model.toStrongRef();

    s_model->getRasterSurface()->render(*data.getPainter(), data.getArea());
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

    _rasterRenderStep = QSharedPointer<RasterRenderStep>(new RasterRenderStep(*this));

    _initHelper.initializeEnd();
}

IRenderStack& LayerPresenter::getRenderStack()
{
    _initHelper.check();

    if (!_renderStack)
        _renderStack = ServiceLocator::make<IRenderStack>(_rasterRenderStep);

    return *_renderStack;
}

void LayerPresenter::onRasterChanged(QRect area)
{
    emit updated(area);
}
