/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "documentpresenter.hpp"
#include "servicelocator.hpp"

#include "utilities/model/documentbuilder.hpp"

#include "utils.hpp"

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
    
//     datatree_echo(
//             _model->layers(),
//             _layers,
//             aux_datatree::echo_default_make_leaf_tag {},
//             aux_datatree::echo_default_make_branch_tag {},
//             [&] (LayersTree::Node* layerNode, const IDocument::LayersTree::Node* layerModelNode) {
//                 layerNode->setLeafValue(
//                         _layerFactory.makeShared(
//                             *this,
//                             *layerNode,
//                             layerModelNode->leafValue()
//                         )
//                     );
//             },
//             [&] (LayersTree::Node* layerNode, const IDocument::LayersTree::Node* layerModelNode) {
//                 layerNode->setBranchValue(
//                         _layerGroupFactory.makeShared(
//                             *this,
//                             *layerNode,
//                             layerModelNode->branchValue()
//                         )
//                     );
//             }
//         );
}
    
void DocumentPresenter::save(QSharedPointer<FileRequest> request)
{
}

void DocumentPresenter::addLayer()
{
}

void DocumentPresenter::addLayerGroup()
{
}

void DocumentPresenter::removeSelectedLayers()
{
}

QSharedPointer<ILayerPresenter> DocumentPresenter::topSelectedLayer_p() const
{
    auto i = _layerSelection.begin();
//     for (const auto& layer : _layers.leaves())
//     {
//         while (i != _layerSelection.end() && *i < layer.address())
//         {
//             if (*i == layer.address() || (*i).isAncestorOf(layer.address()))
//                 return layer.leafValue();
//             ++i;
//         }
//     }
    return nullptr;
}
