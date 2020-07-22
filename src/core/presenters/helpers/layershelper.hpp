#ifndef LAYERSHELPER_HPP
#define LAYERSHELPER_HPP

#include <QMap>
#include <QSet>
#include <set>

#include "utilities/qtextensions/qhash.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "utilities/heirarchylist.hpp"

#include "utilities/helpercallback.hpp"
#include "servicelocator.hpp"

class LayersHelper
{
public:

    LayersHelper(IDocumentPresenter* document) : _document(document) { }

    void initialize(const QList<QSharedPointer<ILayer>>& layerModels)
    {
        for (auto layer : layerModels)
            _layers.addValue(ServiceLocator::makeShared<ILayerPresenter>(_document, layer));
                
        _layerSelection.insert(&_layers[0]);
        _layerSelection_ordered.insert(&_layers[0]);
    }

    HeirarchyList<QSharedPointer<ILayerPresenter>> layers() const { return _layers; }

    QSet<IDocumentPresenter::LayerNode> layerSelection() const { return _layerSelection; }
    
    void setLayerSelection(QSet<IDocumentPresenter::LayerNode> layer);
    void addLayerSelection(QSet<IDocumentPresenter::LayerNode> layer);
    void removeLayerSelection(QSet<IDocumentPresenter::LayerNode> layer);

    QSharedPointer<ILayerPresenter> topSelectedLayer() const
    {
        if (_layerSelection_ordered.empty())
            return nullptr;
        else
            //very cachable
            return (*_layerSelection_ordered.begin())->takeFirstValue();
    }

    HelperArgCallback<IDocumentPresenter::LayersList> onLayersChanged;
    HelperArgCallback<QSet<IDocumentPresenter::LayerNode>> onLayerSelectionChanged;

private:
    IDocumentPresenter::LayersList _layers;

    QSet<IDocumentPresenter::LayerNode> _layerSelection;
    std::set<IDocumentPresenter::LayerNode> _layerSelection_ordered;

    QSet<IDocumentPresenter::LayerNode> _stashedLayerSelection;

    IDocumentPresenter* _document;
};

#endif // LAYERSHELPER_HPP