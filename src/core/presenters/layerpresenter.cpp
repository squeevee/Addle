/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "layerpresenter.hpp"
#include "servicelocator.hpp"
#include "interfaces/editing/irastersurface.hpp"
#include "interfaces/rendering/irenderstack.hpp"

#include "utilities/qobject.hpp"
using namespace Addle;

void LayerPresenterRenderStep::render(RenderHandle& data) const
{
}

LayerPresenter::~LayerPresenter()
{
    delete _renderStack;
}

void LayerPresenter::initialize(IDocumentPresenter* documentPresenter, QSharedPointer<ILayer> model)
{
    const Initializer init(_initHelper);

    _documentPresenter = documentPresenter;
    if (model)
        _model = model;
    else
        _model = ServiceLocator::makeShared<ILayer>();

    connect_interface(
        _model->rasterSurface().data(),
        SIGNAL(changed(QRect)),
        this,
        SLOT(onRasterChanged(QRect)),
        Qt::QueuedConnection
    );

    _renderStep = QSharedPointer<IRenderStep>(new LayerPresenterRenderStep(*this));
    _rasterSurfaceStep = _model->rasterSurface()->renderStep();
}

IRenderStack& LayerPresenter::renderStack()
{
    ASSERT_INIT()

    if (!_renderStack)
    {
        _renderStack = ServiceLocator::make<IRenderStack>(QList<QWeakPointer<IRenderStep>>({
            _renderStep,
            _rasterSurfaceStep
        }));
    }

    return *_renderStack;
}

void LayerPresenter::setName(QString name)
{
    if (_name != name)
    {
        _name = name;
        emit nameChanged(_name);
    }
}

void LayerPresenter::onRasterChanged(QRect area)
{
    //emit updated(area);
}
