/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ILAYERPRESENTER_HPP
#define ILAYERPRESENTER_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

#include "./ilayernodepresenter.hpp"

#include "utilities/model/layernodebuilder.hpp"

#include "interfaces/models/ilayer.hpp"

#include <QWeakPointer>
#include <QPainter>
#include <QRectF>

namespace Addle {

class IRenderable;
class IDocumentPresenter;
class ILayerPresenter : public ILayerNodePresenter, public virtual IAmQObject
{
public:    
    virtual ~ILayerPresenter() = default;
    
    virtual QSharedPointer<ILayer> model() = 0;
    virtual QSharedPointer<const ILayer> model() const = 0;

    virtual QSharedPointer<IRenderable> renderable() const = 0;
};

namespace aux_ILayerPresenter {
    ADDLE_FACTORY_PARAMETER_NAME( document )
    ADDLE_FACTORY_PARAMETER_NAME( layerNode )
    ADDLE_FACTORY_PARAMETER_NAME( model )
    ADDLE_FACTORY_PARAMETER_NAME( modelBuilder )
}

ADDLE_DECL_MAKEABLE(ILayerPresenter)
ADDLE_DECL_FACTORY_PARAMETERS(
    ILayerPresenter,
    (required
        (document,  (IDocumentPresenter&))
        (layerNode, (ILayerNodePresenter::LayerNodeRef))
    )
    (optional
        (model,         (QSharedPointer<ILayer>),   nullptr)
        (modelBuilder,  (const LayerNodeBuilder&),  LayerNodeBuilder())
    )
)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::ILayerPresenter, "org.addle.ILayerPresenter")

#endif // ILAYERPRESENTER_HPP
