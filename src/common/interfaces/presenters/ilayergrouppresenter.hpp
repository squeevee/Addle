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
    
//     inline auto layerNodeChildren()
//     {
//         using namespace boost::adaptors;
//         return layerNode().children() | transformed(
//             [] (const LayerNode& node) -> QSharedPointer<ILayerNodePresenter> {
//                 if (node.isLeaf())
//                     return qSharedPointerCast<ILayerNodePresenter>(node.leafValue());
//                 else 
//                     return qSharedPointerCast<ILayerNodePresenter>(node.branchValue());
//             });
//     }
//     
//     inline auto layerNodeChildren() const
//     {
//         using namespace boost::adaptors;
//         return layerNode().children() | transformed(
//             [] (const ConstLayerNode& node) -> QSharedPointer<const ILayerNodePresenter> {
//                 if (node.isLeaf())
//                     return qSharedPointerCast<const ILayerNodePresenter>(node.leafValue());
//                 else 
//                     return qSharedPointerCast<const ILayerNodePresenter>(node.branchValue());
//             });
//     }
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
        (layerNode, (ILayerGroupPresenter::LayerNode&))
    )
    (optional
        (model,     (QSharedPointer<ILayerGroup>),  nullptr)
    )
)
}

Q_DECLARE_INTERFACE(Addle::ILayerGroupPresenter, "org.addle.ILayerGroupPresenter")
