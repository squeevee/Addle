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
    _topSelectedLayer.calculateBy(&DocumentPresenter::topSelectedLayer_p, this);
    _topSelectedLayer.onChange.bind(&DocumentPresenter::topSelectedLayerChanged, this);
    
    if (_model)
        defaultModel.clear();
    else
        _model = std::move(defaultModel.bind(modelBuilder)).asShared();

    {
        const auto lock = _model->layers().observer().lockForRead();
        aux_datatree::echo_tree(
                _model->layers(),
                _layers,
                aux_datatree::echo_default_node_value_tag {},
                [&] (const IDocument::LayersTree::Node* modelNode, 
                     LayersTree::Node* presenterNode) {
                    if (modelNode->isRoot()) return;
                    
                    if (modelNode->isBranch())
                    {
                        presenterNode->setValue(_layerGroupFactory.makeShared(
                                *this, 
                                presenterNode->nodeRef(), 
                                modelNode->value().staticCast<ILayerGroup>()
                            ));
                    }
                    else
                    {
                        presenterNode->setValue(_layerFactory.makeShared(
                                *this, 
                                presenterNode->nodeRef(), 
                                modelNode->value().staticCast<ILayer>()
                            ));
                    }
                }
            );
    }
}

void DocumentPresenter::setLayerSelection(QList<DataTreeNodeAddress> selection)
{
    QList<DataTreeNodeAddress> oldSelection;
    
    if ( !std::is_sorted(selection.begin(), selection.end()) )
        std::sort(_layerSelection.begin(), _layerSelection.end());
    
    if (selection != _layerSelection)
    {
        _layerSelection = selection;
        emit layerSelectionChanged(_layerSelection);
    }
}

void DocumentPresenter::addLayerSelection(QList<DataTreeNodeAddress> added)
{
    if (added.isEmpty())
        return;
    
    if ( !std::is_sorted(added.cbegin(), added.cend()) )
        std::sort(added.begin(), added.end());
    
    std::size_t oldSize = _layerSelection.size();
    _layerSelection.append(added);
    std::inplace_merge(
            _layerSelection.begin(), 
            _layerSelection.begin() + oldSize,
            _layerSelection.end()
        );
    
    emit layerSelectionChanged(_layerSelection);
}

void DocumentPresenter::subtractLayerSelection(QList<DataTreeNodeAddress> removed)
{
    if (removed.isEmpty())
        return;
    
    if ( !std::is_sorted(removed.cbegin(), removed.cend()) )
        std::sort(removed.begin(), removed.end());
    
    QList<DataTreeNodeAddress> selection;
    selection.reserve(_layerSelection.size() - removed.size());
    
    std::set_difference(
            _layerSelection.cbegin(), _layerSelection.cend(),
            removed.cbegin(), removed.cend(),
            std::back_inserter(selection)
        );
    
    _layerSelection = selection;
    emit layerSelectionChanged(_layerSelection);
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
