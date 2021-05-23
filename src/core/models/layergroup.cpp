#include "layergroup.hpp"

using namespace Addle;

LayerGroup::LayerGroup(
        IDocument& document,
        LayerNode& layerNode,
        const LayerNodeBuilder& builder,
        const IFactory<ILayer>& layerFactory,
        const IFactory<ILayerGroup>& layerGroupFactory
    )
    : _document(document),
    _layerNode(layerNode),
    _name(builder.name()),
    _passThroughMode(builder.passThroughMode()),
    _compositionMode(builder.compositionMode()),
    _opacity(builder.opacity()),
    _layerFactory(layerFactory),
    _layerGroupFactory(layerGroupFactory)
{
    // TODO: validate naive builder values
    
    for (const LayerNodeBuilder& childBuilder : builder.childLayerNodes())
    {
        if (childBuilder.isLayer())
        {
            LayerNode& childNode = _layerNode.addLeaf();
            auto childLayer = _layerFactory.makeShared(_document, childNode, childBuilder);
            childNode.setLeafValue(childLayer);
        }
        else
        {
            LayerNode& childNode = _layerNode.addBranch();
            auto childLayerGroup = _layerGroupFactory.makeShared(_document, childNode, childBuilder);
            childNode.setBranchValue(childLayerGroup);
        }
    }
}
