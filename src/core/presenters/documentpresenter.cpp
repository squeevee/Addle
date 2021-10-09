/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QtDebug>

#include "documentpresenter.hpp"
#include "servicelocator.hpp"

#include "utilities/model/documentbuilder.hpp"

#include "utils.hpp"
#include "utilities/datatree/echo.hpp"

#include "interfaces/models/ilayergroup.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/presenters/ilayergrouppresenter.hpp"

using namespace Addle;


DocumentPresenter::DocumentPresenter(
        QSharedPointer<IDocument> model,
        LazyInjection<IDocument> defaultModel,
        const DocumentBuilder& modelBuilder,
        const IFactory<ILayerPresenter>& layerFactory,
        const IFactory<ILayerGroupPresenter>& layerGroupFactory
    )
    : _model(model),
    _layerFactory(layerFactory),
    _layerGroupFactory(layerGroupFactory)
{
    _topSelectedLayerNode.calculateBy(&DocumentPresenter::topSelectedLayerNode_p, this);
    _topSelectedLayerNode.onChange_Recalculate(_topSelectedLayer);
    
    _topSelectedLayer.calculateBy(&DocumentPresenter::topSelectedLayer_p, this);
    _topSelectedLayer.onChange.bind(&DocumentPresenter::topSelectedLayerChanged, this);
    
    if (_model)
    {
        defaultModel.clear();
    }
    else
    {
        _model = std::move(defaultModel.bind(modelBuilder)).asShared();
    }

    _layers.root().setValencyHint(aux_datatree::ValencyHint_Branch);
    
    {
        const auto lock = _model->layers().observer().lockForRead();
        aux_datatree::echo_tree(
                _model->layers(),
                _layers,
                aux_datatree::echo_default_node_value_tag {},
                [&] (const IDocument::LayersTree::Node* modelNode, 
                     LayersTree::Node* presenterNode) {
                    if (modelNode->isRoot()) return;
                    
                    if (modelNode->leafHint())
                    {
                        presenterNode->setValencyHint(aux_datatree::ValencyHint_Leaf);
                        presenterNode->setValue(_layerFactory.makeShared(
                                *this, 
                                presenterNode->nodeRef(), 
                                modelNode->value().staticCast<ILayer>()
                            ));
                    }
                    else if (modelNode->branchHint())
                    {   
                        presenterNode->setValencyHint(aux_datatree::ValencyHint_Branch);
                        presenterNode->setValue(_layerGroupFactory.makeShared(
                                *this, 
                                presenterNode->nodeRef(), 
                                modelNode->value().staticCast<ILayerGroup>()
                            ));
                    }
                    else
                    {
                        Q_UNREACHABLE();
                    }
                }
            );
    }
    
    if (_layers.root().hasChildren())
    {
        _layerSelection = { _layers.root()[0].nodeRef() };
    }
}

void DocumentPresenter::setLayerSelection(QSet<ILayerNodePresenter::LayerNodeRef> selection)
{
    if (selection != _layerSelection)
    {
        _layerSelection = selection;
        emit layerSelectionChanged(_layerSelection);
        
        _topSelectedLayerNode.recalculate();
    }
}

void DocumentPresenter::addLayerSelection(QSet<ILayerNodePresenter::LayerNodeRef> added)
{
    if (added.isEmpty())
        return;
    
    _layerSelection.unite(added);
    emit layerSelectionChanged(_layerSelection);
    
    _topSelectedLayerNode.recalculate();
}

void DocumentPresenter::subtractLayerSelection(QSet<ILayerNodePresenter::LayerNodeRef> removed)
{
    if (removed.isEmpty())
        return;
    
    _layerSelection.subtract(removed);
    emit layerSelectionChanged(_layerSelection);
    
    _topSelectedLayerNode.recalculate();
}

void DocumentPresenter::save(QSharedPointer<FileRequest> request)
{
}

void DocumentPresenter::addLayer()
{
    auto modelLock = _model->layers().observer().lockForWrite();
    auto lock = _layers.observer().lockForWrite();
    
    auto location = layerNodeInsertLocation();
    auto& newModelNode = *(_model->insertLayerNodes(
            location,
            {
                LayerNodeBuilder()
                    .setName("New Layer")
            }
        ).begin());
    
    _layers.observer().startRecording();
    auto& newNode = _layers.root().descendantAt(location.parent())
        .insertChild(location.lastIndex());
    
    newNode.setValencyHint(aux_datatree::ValencyHint_Leaf);
    newNode.setValue(_layerFactory.makeShared(
            *this,
            newNode.nodeRef(),
            newModelNode.value().staticCast<ILayer>()
        ));
    auto e = _layers.observer().finishRecording();
    
    _layerSelection = { newNode.nodeRef() };
    
    lock->unlock();
    modelLock->unlock();
    
    emit layerNodesChanged(e);
    emit layerSelectionChanged(_layerSelection);
    
    // TODO: async safety
    _topSelectedLayerNode.recalculate();
    _topSelectedLayer.recalculate();
}

void DocumentPresenter::addLayerGroup()
{
    auto modelLock = _model->layers().observer().lockForWrite();
    auto lock = _layers.observer().lockForWrite();
    
    auto location = layerNodeInsertLocation();
    auto& newModelNode = *(_model->insertLayerNodes(
            location,
            {
                LayerNodeBuilder()
                    .setName("New Layer Group")
                    .setIsGroup(true)
            }
        ).begin());
    
    _layers.observer().startRecording();
    auto& newNode = _layers.root().descendantAt(location.parent())
        .insertChild(location.lastIndex());
    
    newNode.setValencyHint(aux_datatree::ValencyHint_Branch);
    newNode.setValue(_layerGroupFactory.makeShared(
            *this,
            newNode.nodeRef(),
            newModelNode.value().staticCast<ILayerGroup>()
        ));
    auto e = _layers.observer().finishRecording();
    
    _layerSelection = { newNode.nodeRef() };
    
    lock->unlock();
    modelLock->unlock();
    
    emit layerNodesChanged(e);
    emit layerSelectionChanged(_layerSelection);
    
    // TODO: async safety
    _topSelectedLayerNode.recalculate();
    _topSelectedLayer.recalculate();
}

void DocumentPresenter::removeSelectedLayers()
{
    auto modelLock = _model->layers().observer().lockForWrite();
    auto lock = _layers.observer().lockForWrite();
    
    QList<DataTreeNodeChunk> selectedChunks;
    
    for (auto node : _layerSelection)
    {
        assert(node);
        selectedChunks.append({(*node).address(), 1});
    }
    aux_datatree::cleanupChunkSet(selectedChunks);
    
    _model->removeLayers(selectedChunks);
    
    _layers.observer().startRecording();
    
    const DataTreeNodeEvent& pending = _layers.observer().pendingEvent();
    std::size_t progress = 0;
    for ( auto chunk : selectedChunks )
    {
        for ( auto mapped : noDetach(pending.mapChunkForward(chunk)) )
        {
            _layers.root().descendantAt(mapped.address.parent())
                .removeChildren(mapped.address.lastIndex(), mapped.length);
            ++progress;
        }
    }
    
    auto e = _layers.observer().finishRecording();
    
    _layerSelection.clear();
    
    lock->unlock();
    modelLock->unlock();
    
    emit layerNodesChanged(e);
    emit layerSelectionChanged(_layerSelection);
    
    _topSelectedLayerNode.recalculate();
    _topSelectedLayer.recalculate();
}

ILayerNodePresenter::LayerNode* DocumentPresenter::topSelectedLayerNode_p() const
{
    if (noDetach(_layerSelection).isEmpty()) return {};
    
    auto lock = _layers.observer().lockForRead();
    
    ILayerNodePresenter::LayerNode* result = nullptr;
    for (const auto& ref : noDetach(_layerSelection))
    {
        if (Q_UNLIKELY(!ref.isValid())) continue;
        
        auto newAddress = (*ref).address();
        if (!result || newAddress < result->address())
            result = ref.get();
    }
    
    return result;
}

QSharedPointer<ILayerPresenter> DocumentPresenter::topSelectedLayer_p() const
{
    if (noDetach(_layerSelection).isEmpty()) return nullptr;
    
    auto lock = _layers.observer().lockForRead();
    
    auto i      = _topSelectedLayerNode.value()->begin();
    auto end    = _topSelectedLayerNode.value()->end();
    
    while ((*i).branchHint())
    {
        ++i;
        if (i == end) return {};
    }
    
    return (*i).value().staticCast<ILayerPresenter>();
}

DataTreeNodeAddress DocumentPresenter::layerNodeInsertLocation() const
{
    auto topSelectedNode = _topSelectedLayerNode.value();
    
    if (!topSelectedNode)
    {
        return DataTreeNodeAddressBuilder() << _layers.root().childCount();
    }
    else if (topSelectedNode->branchHint())
    {
        return DataTreeNodeAddressBuilder(topSelectedNode->address()) 
            << topSelectedNode->childCount();
    }
    else
    {
        return DataTreeNodeAddressBuilder(topSelectedNode->address())
            .offsetLastIndex(1);
    }
}
