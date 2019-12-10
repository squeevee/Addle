#include "layerpresenter.hpp"
#include "servicelocator.hpp"
#include "interfaces/views/ilayerview.hpp"

#include "utilities/qt_extensions/qobject.hpp"

void LayerPresenter::initialize(IDocumentPresenter* documentPresenter, QWeakPointer<ILayer> model)
{
    _initHelper.initializeBegin();

    _documentPresenter = documentPresenter;
    _model = model;

    auto s_model = _model.toStrongRef();

    connect_interface(
        s_model.data(),
        SIGNAL(renderChanged(QRect)),
        this,
        SLOT(onModelRenderChanged(QRect)),
        Qt::QueuedConnection
    );

    _initHelper.initializeEnd();
}


ILayerView* LayerPresenter::getView()
{
    _initHelper.assertInitialized();
    if (!_view)
    {
        _view = ServiceLocator::make<ILayerView>(this);
    }
    return _view;
}

void LayerPresenter::render(QPainter& painter, QRect area)
{
    if (_model.isNull())
        return;

    auto s_model = _model.toStrongRef();
    s_model->render(painter, area);

    if (_rasterOperation)
    {
        _rasterOperation->render(painter, area);
    }
}

void LayerPresenter::onModelRenderChanged(QRect area)
{
    emit renderChanged(area);
}