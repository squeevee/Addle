/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <boost/range/adaptor/transformed.hpp>

#include "servicelocator.hpp"

#include "utilities/datatree/nestedobjectadapter.hpp"
#include "utilities/datatree/echo.hpp"

#include "document.hpp"
#include <QImage>
#include <QPainter>
#include <QStack>

#include <QtDebug>

using namespace Addle;

Document::Document(
        const DocumentBuilder& builder, 
        const IFactory<ILayer>& layerFactory,
        const IFactory<ILayerGroup>& layerGroupFactory
    )
    : _backgroundColor(builder.backgroundColor()),
    _url(builder.url()),
    _layerFactory(layerFactory), 
    _layerGroupFactory(layerGroupFactory)
{
    _layers.root().setValencyHint(aux_datatree::ValencyHint_Branch);
    aux_datatree::echo_tree(
            aux_datatree::make_root_range_nested_object_adapter(
                builder.layerNodes(),
                &LayerNodeBuilder::childLayerNodes
            ),
            _layers,
            aux_datatree::echo_default_node_value_tag {},
            std::bind(&Document::populateLayerNode_p, this, 
                    std::placeholders::_1, std::placeholders::_2)
        );
}

aux_datatree::NodeRange<Document::LayersTree> Document::insertLayerNodes(
        DataTreeNodeAddress startPos,
        QList<LayerNodeBuilder> layerNodeBuilders
    )
{
    auto lock = _layers.observer().lockForWrite();
    
    // make sure it exists and throw an exception
    LayersTree::Node& parent = _layers.root().descendantAt(startPos.parent());
    
    _layers.observer().startRecording();

    auto adapter = aux_datatree::make_root_range_nested_object_adapter(
            noDetach(layerNodeBuilders),
            &LayerNodeBuilder::childLayerNodes
        );
    
    auto result = aux_datatree::echo_subtree<builder_adapter_t, LayersTree>(
            aux_datatree::tree_root(adapter),
            &parent,
            parent.children().begin() + startPos.lastIndex(),
            aux_datatree::echo_default_node_value_tag {},
            std::bind(&Document::populateLayerNode_p, this, 
                    std::placeholders::_1, std::placeholders::_2)
        );

    auto e = _layers.observer().finishRecording();
    
    lock->unlock();
    
    emit layerNodesChanged(e);
    
    return result;
}
    
void Document::removeLayers(QList<DataTreeNodeChunk> chunks)
{
    if (chunks.isEmpty()) return;
    aux_datatree::cleanupChunkSet(chunks);
    
    auto lock = _layers.observer().lockForWrite();
        
    _layers.observer().startRecording();
    
    const DataTreeNodeEvent& pending = _layers.observer().pendingEvent();
    std::size_t progress = 0;
    for ( auto chunk : noDetach(chunks) )
    {
        for ( auto mapped : noDetach(pending.mapChunkBackward(std::move(chunk), progress)) )
        {
            _layers.root().descendantAt(mapped.address.parent())
                .removeChildren(mapped.address.lastIndex(), mapped.length);
            ++progress;
        }
    }
    
    auto e = _layers.observer().finishRecording();
    
    lock->unlock();
    
    emit layerNodesChanged(e);
}

void Document::populateLayerNode_p(
        aux_datatree::const_node_handle_t<builder_adapter_t> adapterNodeHandle, 
        ILayerNodeModel::LayerNode* node)
{
    if (aux_datatree::node_is_root(adapterNodeHandle)) return;
    
    if ((*adapterNodeHandle).value().isLayer())
    {
        node->setValencyHint(aux_datatree::ValencyHint_Leaf);
        node->setValue(_layerFactory.makeShared(
                *this, 
                node->nodeRef(), 
                (*adapterNodeHandle).value()
            ));
    }
    else if ((*adapterNodeHandle).value().isGroup())
    {
        node->setValencyHint(aux_datatree::ValencyHint_Branch);
        node->setValue(_layerGroupFactory.makeShared(
                *this, 
                node->nodeRef(), 
                (*adapterNodeHandle).value()
            ));
    }
    else
    {
        Q_UNREACHABLE();
    }
}
