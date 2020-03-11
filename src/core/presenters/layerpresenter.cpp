#include "layerpresenter.hpp"
#include "servicelocator.hpp"
#include "interfaces/editing/surfaces/irastersurface.hpp"

#include "utilities/qt_extensions/qobject.hpp"

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

    _initHelper.initializeEnd();
}

void LayerPresenter::render(QPainter& painter, QRect area)
{
    if (_model.isNull())
        return;

    auto s_model = _model.toStrongRef();
    //assert s_model

    s_model->getRasterSurface()->render(painter, area);
}

void LayerPresenter::onRasterChanged(QRect area)
{
    emit renderChanged(area);
}