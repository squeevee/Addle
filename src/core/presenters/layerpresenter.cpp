/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "layerpresenter.hpp"
#include "servicelocator.hpp"
#include "interfaces/presenters/ilayergrouppresenter.hpp"
// #include "interfaces/editing/irastersurface.hpp"
#include "interfaces/rendering/irenderer.hpp"

#include "utilities/qobject.hpp"
using namespace Addle;

LayerPresenter::LayerPresenter(
    IDocumentPresenter& document,
    ILayerNodePresenter::LayerNodeRef layerNode,
    QSharedPointer<ILayer> model
)
    : _document(document), 
    _layerNode(std::move(layerNode)),
    _model(model)
{    
//     if (model)
//         _model = model;
//     else
//         _model = ServiceLocator::makeShared<ILayer>();

//     connect_interface(
//         _model->rasterSurface().data(),
//         SIGNAL(changed(QRect)),
//         this,
//         SLOT(onRasterChanged(QRect)),
//         Qt::QueuedConnection
//     );

//     _renderable = QSharedPointer<IRenderable>(new LayerPresenterRenderable(*this));
//     _rasterSurfaceStep = _model->rasterSurface()->renderable();
    
    HelperBase::initialize();
}

// IRenderer& LayerPresenter::renderer()
// {
//     ASSERT_INIT()
// 
//     if (!_renderer)
//     {
//         _renderer = ServiceLocator::make<IRenderer>(QList<QWeakPointer<IRenderable>>({
//             _renderable,
//             _rasterSurfaceStep
//         }));
//     }
// 
//     return *_renderer;
// }
