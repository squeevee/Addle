#ifndef DOCUMENTLAYERSHELPER_HPP
#define DOCUMENTLAYERSHELPER_HPP

#include <QMap>
#include <QSet>
#include <set>

#include "utilities/qtextensions/qhash.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "utilities/heirarchylist.hpp"

#include "utilities/presenter/propertycache.hpp"
#include "utilities/helpercallback.hpp"
#include "servicelocator.hpp"

class DocumentLayersHelper
{
    typedef IDocumentPresenter::LayerList LayerList;
    typedef IDocumentPresenter::LayerNode LayerNode;
public:
    DocumentLayersHelper(IDocumentPresenter* document)
        : _document(document)
    {
        _topSelectedLayer.calculateBy(std::bind(&DocumentLayersHelper::calculate_topLayer, this));
        _topSelectedLayer.onChange([&](QSharedPointer<ILayerPresenter> layer) { onTopSelectedLayerChanged(layer); });
    }

    void initialize(const QList<QSharedPointer<ILayer>>& layerModels)
    {
        for (auto layer : layerModels)
            _layers.addValue(ServiceLocator::makeShared<ILayerPresenter>(_document, layer));
                
        _layerSelection.insert(&_layers[0]);
        _layerSelection_ordered.insert(&_layers[0]);
    }

    HeirarchyList<QSharedPointer<ILayerPresenter>> layers() const { return _layers; }

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
        for (LayerNode* node : qAsConst(added))
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
        for (LayerNode* node : qAsConst(subtracted))
        {
            _layerSelection_ordered.erase(node);
        }

        _topSelectedLayer.recalculate();
        if (topSelectedLayer() != topSelected)
            onTopSelectedLayerChanged(topSelectedLayer());
    }

    QSharedPointer<ILayerPresenter> topSelectedLayer() const { return _topSelectedLayer.getValue(); }

    LayerNode& addLayer()
    {
        static int num = 0;
        LayerNode* cursor = getCursor();

        int index = cursor->isGroup() ? 0 : cursor->index();
        LayerNode* parent = cursor->isGroup() ? cursor : cursor->parent();
        // verify cursor

        auto layer = ServiceLocator::makeShared<ILayerPresenter>(_document);
        LayerNode& result = parent->addValue(layer, index);
        result.metaData() = QString::number(num++);

        onLayersAdded({ &result });
        onLayersChanged(_layers);

        return result;
    }

    LayerNode& addLayerGroup()
    {
        LayerNode* cursor = getCursor();

        int index = cursor->isGroup() ? 0 : cursor->index();
        LayerNode* parent = cursor->isGroup() ? cursor : cursor->parent();
        // verify cursor

        LayerNode& result = parent->addGroup(index);

        onLayersAdded({ &result });
        onLayersChanged(_layers);

        return result;   
    }
    
    void removeSelectedLayers() { }
    void moveSelectedLayers(int destination) { }
    void mergeSelectedLayers() { }

    HelperArgCallback<QList<LayerNode*>> onLayersAdded;
    HelperArgCallback<QList<LayerNode*>> onLayersRemoved;
    HelperArgCallback<QList<LayerNode*>> onLayersMoved;

    HelperArgCallback<LayerList> onLayersChanged;
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

    IDocumentPresenter* _document;
};

#endif // DOCUMENTLAYERSHELPER_HPP