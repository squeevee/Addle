#pragma once

#include <QString>
#include <QPainter>
#include "interfaces/iamqobject.hpp"

#include "utilities/model/layernodebuilder.hpp"

#include "./ilayernodemodel.hpp"

namespace Addle {
    
class ILayerGroup : public ILayerNodeModel, public virtual IAmQObject
{
public:
    virtual ~ILayerGroup() = default;
    
    virtual bool passThroughMode() const = 0;
    virtual void setPassThroughMode(bool mode) = 0;
    
signals:
    virtual void passThroughModeChanged(bool mode) = 0;
};

namespace aux_ILayerGroup {
    ADDLE_FACTORY_PARAMETER_NAME( document )
    ADDLE_FACTORY_PARAMETER_NAME( layerNode )
    ADDLE_FACTORY_PARAMETER_NAME( builder )
}
 
ADDLE_DECL_MAKEABLE(ILayerGroup)
ADDLE_DECL_FACTORY_PARAMETERS(
    ILayerGroup,
    (required 
        (document,  (IDocument&))
        (layerNode, (ILayerGroup::LayerNodeRef)) 
    )
    (optional (builder, (const LayerNodeBuilder&), LayerNodeBuilder()) )
)
}

Q_DECLARE_INTERFACE(Addle::ILayerGroup, "org.addle.ILayerGroup")
