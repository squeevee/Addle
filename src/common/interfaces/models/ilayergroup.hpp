#pragma once

#include <QString>
#include <QPainter>
#include "interfaces/iamqobject.hpp"

#include "./ilayernodemodel.hpp"

namespace Addle {
    
class ILayerGroup : public ILayerNodeModel, public virtual IAmQObject
{
public:
    virtual ~ILayerGroup() = default;
    
    virtual bool passThroughMode() const = 0;
    virtual void setPassThroughMode(bool mode) = 0;
    
//     inline auto childLayerNodes() {
//         using namespace boost::adaptors;
//         return this->layerNode().children()
//             | transformed(
//                 [] (const LayerNode& node) -> QSharedPointer<ILayerNodeModel> {
//                     return node.value();
//                 }
//             );
//     }
    
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
        (layerNode, (ILayerGroup::LayerNode&)) 
    )
    (optional (builder, (const LayerNodeBuilder&), LayerNodeBuilder()) )
)
}

Q_DECLARE_INTERFACE(Addle::ILayerGroup, "org.addle.ILayerGroup")
