#include "layergroup.hpp"

using namespace Addle;

LayerGroup::LayerGroup(
        IDocument& document,
        LayerNodeRef layerNode,
        const LayerNodeBuilder& builder,
        const IFactory<ILayer>& layerFactory,
        const IFactory<ILayerGroup>& layerGroupFactory
    )
    : _document(document),
    _layerNode(std::move(layerNode)),
    _name(builder.name()),
    _passThroughMode(builder.passThroughMode()),
    _compositionMode(builder.compositionMode()),
    _opacity(builder.opacity()),
    _layerFactory(layerFactory),
    _layerGroupFactory(layerGroupFactory)
{
    // TODO: validate naive builder values
}
