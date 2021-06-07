/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef DOCUMENTPRESENTER_HPP
#define DOCUMENTPRESENTER_HPP

#include "compat.hpp"

#include <memory>
#include <QList>
#include <QObject>

#include "utils.hpp"

#include "interfaces/models/idocument.hpp"
#include "interfaces/presenters/idocumentpresenter.hpp"

#include "utilities/lazyvalue.hpp"
#include "utilities/config/lazyinjection.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT DocumentPresenter : public QObject, public IDocumentPresenter
{
    Q_OBJECT
    Q_INTERFACES(Addle::IDocumentPresenter)
    IAMQOBJECT_IMPL
    
public:
    DocumentPresenter(
            QSharedPointer<IDocument> model,
            LazyInjection<IDocument> defaultModel,
            const DocumentBuilder& modelBuilder,
            const IFactory<ILayerPresenter>& layerFactory,
            const IFactory<ILayerGroupPresenter>& layerGroupFactory
        );
    
    virtual ~DocumentPresenter() = default;

    QSharedPointer<IDocument> model() override { return _model; }

    QSize size() override { return _model ? _model->size() : QSize(); }
    QRect rect() override { return QRect(QPoint(), size()); }
    QColor backgroundColor() override { return _model ? _model->backgroundColor() : QColor(); }

    const LayersTree& layers() override { return _layers; }
    ConstLayersTree layers() const override { return _layers; }

    QList<DataTreeNodeAddress> layerSelection() const override { return _layerSelection; }
    void setLayerSelection(QList<DataTreeNodeAddress> selection) override
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
    
    void addLayerSelection(QList<DataTreeNodeAddress> added) override
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
    
    void subtractLayerSelection(QList<DataTreeNodeAddress> removed) override
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

    QSharedPointer<ILayerPresenter> topSelectedLayer() const override
    {
        return _topSelectedLayer.value();
    }

    bool isLocalFile() const override { return _model && _model->url().isLocalFile(); }

public slots:
    void addLayer() override;
    void addLayerGroup() override;
    void removeSelectedLayers() override;
    void moveSelectedLayers(int destination) override { }
    void mergeSelectedLayers() override { }
    
    void save(QSharedPointer<FileRequest> request) override;

signals:
    void layerNodesChanged(DataTreeNodeEvent) override;
    //void layersMoved(QList<IDocumentPresenter::LayerNode*> moved) override;

    void layersChanged() override;
    void topSelectedLayerChanged(QSharedPointer<ILayerPresenter> layer) override;
    
    void layerSelectionChanged(QList<DataTreeNodeAddress> selection) override;
private:        
    QSharedPointer<ILayerPresenter> topSelectedLayer_p() const;
    
    IDocumentPresenter::LayersTree _layers;
    QList<DataTreeNodeAddress> _layerSelection;
    
    LazyProperty<QSharedPointer<ILayerPresenter>> _topSelectedLayer;
    
    QSize _size;
    QColor _backgroundColor;
    
    QSharedPointer<IDocument> _model;
    
    const IFactory<ILayerPresenter>& _layerFactory;
    const IFactory<ILayerGroupPresenter>& _layerGroupFactory;
};

} // namespace Addle

#endif // DOCUMENTPRESENTER_HPP
