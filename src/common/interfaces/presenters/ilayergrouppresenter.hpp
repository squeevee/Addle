#pragma once

#include <boost/range/adaptor/transformed.hpp>

#include "interfaces/traits.hpp"

#include "utilities/qobject.hpp"

#include "./ilayerpresenter.hpp"
#include "./ilayernodepresenter.hpp"

namespace Addle {

class ILayerGroup;
class LayerNodeBuilder;

class ILayerGroupPresenter : public ILayerNodePresenter, public virtual IAmQObject
{
public:    
    virtual ~ILayerGroupPresenter() = default;
    
    virtual QSharedPointer<ILayerGroup> model() = 0;
    virtual QSharedPointer<const ILayerGroup> model() const = 0;
};
    
namespace aux_ILayerGroupPresenter {
    ADDLE_FACTORY_PARAMETER_NAME( document )
    ADDLE_FACTORY_PARAMETER_NAME( layerNode )
    ADDLE_FACTORY_PARAMETER_NAME( model )
}

ADDLE_DECL_MAKEABLE(ILayerGroupPresenter)
ADDLE_DECL_FACTORY_PARAMETERS(
    ILayerGroupPresenter,
    (required
        (document,  (IDocumentPresenter&))
        (layerNode, (ILayerGroupPresenter::LayerNodeRef))
    )
    (optional
        (model,     (QSharedPointer<ILayerGroup>),  nullptr)
    )
)
}

Q_DECLARE_INTERFACE(Addle::ILayerGroupPresenter, "org.addle.ILayerGroupPresenter")
