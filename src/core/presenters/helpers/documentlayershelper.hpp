#ifndef DOCUMENTLAYERSHELPER_HPP
#define DOCUMENTLAYERSHELPER_HPP

#include <QMap>
#include <QSet>
#include <set>

#include "utils.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "utilities/heirarchylist.hpp"

#include "utilities/model/layergroupinfo.hpp"
#include "utilities/presenter/propertycache.hpp"
#include "utilities/helpercallback.hpp"
#include "servicelocator.hpp"
namespace Addle {

class DocumentLayersHelper
{
    typedef IDocumentPresenter::LayerList LayerList;
    typedef IDocumentPresenter::LayerNode LayerNode;
    typedef IDocumentPresenter::LayerNodeRemoved LayerNodeRemoved;
public:
    DocumentLayersHelper(IDocumentPresenter* document)
        : _document(document)
    {
        _topSelectedLayer.calculateBy(&DocumentLayersHelper::calculate_topLayer, this);
        _topSelectedLayer.onChange += onTopSelectedLayerChanged;
    }

    void initialize(const QList<QSharedPointer<ILayer>>& layerModels)
    {
        for (auto layerModel : layerModels)
        {
            auto layer = ServiceLocator::makeShared<ILayerPresenter>(_document, layerModel);
            _layers.addValue(layer);
            layer->setName(QString("Layer %1").arg(_layerLabelNumber));
            _layerLabelNumber++;
        }
                
        _layerSelection.insert(&_layers[0]);
        _layerSelection_ordered.insert(&_layers[0]);
    }

    const LayerList& layers() const { return _layers; }

    QSet<LayerNode*> layerSelection() const { return _layerSelection; }
    
    void setLayerSelection(QSet<LayerNode*> selection)
    {
        auto topSelected = topSelectedLayer();

        _layerSelection = selection;

        _layerSelection_ordered.clear();
        for (LayerNode* node : qAsConst(selection))
        {
            _layerSelection_ordered.insert(node);
        }

        onLayerSelectionChanged(_layerSelection);

        _topSelectedLayer.recalculate();
        if (topSelectedLayer() != topSelected)
            onTopSelectedLayerChanged(topSelectedLayer());
    }

    void addLayerSelection(QSet<LayerNode*> added)
    {
        auto topSelected = topSelectedLayer();

        _layerSelection = _layerSelection.unite(added);
        for (LayerNode* node : noDetach(added))
        {
            _layerSelection_ordered.insert(node);
        }

        onLayerSelectionChanged(_layerSelection);

        _topSelectedLayer.recalculate();
        if (topSelectedLayer() != topSelected)
            onTopSelectedLayerChanged(topSelectedLayer());
    }

    void removeLayerSelection(QSet<LayerNode*> subtracted)
    {
        auto topSelected = topSelectedLayer();

        _layerSelection = _layerSelection.subtract(subtracted);
        
        onLayerSelectionChanged(_layerSelection);
        for (LayerNode* node : noDetach(subtracted))
        {
            _layerSelection_ordered.erase(node);
        }

        _topSelectedLayer.recalculate();
        if (topSelectedLayer() != topSelected)
            onTopSelectedLayerChanged(topSelectedLayer());
    }

    QSharedPointer<ILayerPresenter> topSelectedLayer() const { return _topSelectedLayer.value(); }

    LayerNode& addLayer()
    {
        LayerNode* cursor = getCursor();

        int index = cursor->isGroup() ? 0 : cursor->index();
        LayerNode* parent = cursor->isGroup() ? cursor : cursor->parent();
        // verify cursor

        auto layer = ServiceLocator::makeShared<ILayerPresenter>(_document);
        layer->setName(QString("Layer %1").arg(_layerLabelNumber)); //todo: i18n
        LayerNode& result = parent->addValue(layer, index);
        _layerLabelNumber++;

        onLayersAdded({ &result });
        onLayersChanged();

        setLayerSelection({ &result });

        return result;
    }

    LayerNode& addLayerGroup()
    {
        LayerNode* cursor = getCursor();

        int index = cursor->isGroup() ? 0 : cursor->index();
        LayerNode* parent = cursor->isGroup() ? cursor : cursor->parent();
        // verify cursor

        LayerNode& result = parent->addGroup(index);
        QString name = QString("Layer Group %1").arg(_layerGroupLabelNumber);
        result.setMetaData(QVariant::fromValue(LayerGroupInfo(name)));
        _layerGroupLabelNumber++;

        emit onLayersAdded({ &result });
        emit onLayersChanged();

        setLayerSelection({ &result });

        return result;   
    }
    
    void removeSelectedLayers()
    {
        if (_layerSelection.count() == _layers.nodeCount())
        {
            // Deleting all layers is not allowed.
            removeLayerSelection({ *_layerSelection_ordered.rbegin() });
        }

        if (_layerSelection.isEmpty())
            return; // warning?

        auto result = _layers.removeNodes(_layerSelection);

        emit onLayersRemoved(result);
        emit onLayersChanged();
    }

    void moveSelectedLayers(int destination) { }
    void mergeSelectedLayers() { }

    HelperArgCallback<QList<LayerNode*>> onLayersAdded;
    HelperArgCallback<QList<LayerNodeRemoved>> onLayersRemoved;
    HelperArgCallback<QList<LayerNode*>> onLayersMoved;

    HelperCallback onLayersChanged;
    HelperArgCallback<QSet<LayerNode*>> onLayerSelectionChanged;
    HelperArgCallback<QSharedPointer<ILayerPresenter>> onTopSelectedLayerChanged;

private:
    QSharedPointer<ILayerPresenter> calculate_topLayer() const
    {
        if (_layerSelection_ordered.empty())
            return nullptr;
        else
            return (*_layerSelection_ordered.begin())->takeFirstValue();
    }

    LayerNode* getCursor() 
    {
        if (_layers.isEmpty())
            return nullptr;
        else if (_layerSelection.isEmpty())
            return &_layers[0];
        else
            return *_layerSelection_ordered.begin();
    }

    LayerList _layers;

    PropertyCache<QSharedPointer<ILayerPresenter>> _topSelectedLayer;

    QSet<LayerNode*> _layerSelection;
    std::set<LayerNode*, LayerList::Node::Comparator> _layerSelection_ordered;

    QSet<LayerNode*> _stashedLayerSelection;

    int _layerLabelNumber = 1;
    int _layerGroupLabelNumber = 1;

    IDocumentPresenter* _document;
};
} // namespace Addle

#endif // DOCUMENTLAYERSHELPER_HPP