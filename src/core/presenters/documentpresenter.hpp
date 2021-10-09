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

    QSet<ILayerNodePresenter::LayerNodeRef> layerSelection() const override { return _layerSelection; }
    void setLayerSelection(QSet<ILayerNodePresenter::LayerNodeRef> selection) override;
    void addLayerSelection(QSet<ILayerNodePresenter::LayerNodeRef> added) override;
    void subtractLayerSelection(QSet<ILayerNodePresenter::LayerNodeRef> removed) override;

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
    void topSelectedLayerChanged(QSharedPointer<ILayerPresenter> layer) override;
    
    void layerSelectionChanged(QSet<ILayerNodePresenter::LayerNodeRef> selection) override;
    
public:
    RenderRoutine renderRoutine() const override { return _renderRoutine; }

signals:
    void renderRoutineChanged(RenderRoutineChangedEvent) override;
    void renderChanged(QRegion affected, DataTreeNodeAddress entity = {}) override;
    
private:        
    ILayerNodePresenter::LayerNode* topSelectedLayerNode_p() const;
    QSharedPointer<ILayerPresenter> topSelectedLayer_p() const;
    
    DataTreeNodeAddress layerNodeInsertLocation() const;
    
    IDocumentPresenter::LayersTree _layers;
    QSet<ILayerNodePresenter::LayerNodeRef> _layerSelection;
    
    LazyProperty<ILayerNodePresenter::LayerNode*> _topSelectedLayerNode;
    LazyProperty<QSharedPointer<ILayerPresenter>> _topSelectedLayer;
    
    QSize _size;
    QColor _backgroundColor;
    
    RenderRoutine _renderRoutine;
    
    QSharedPointer<IDocument> _model;
    
    const IFactory<ILayerPresenter>& _layerFactory;
    const IFactory<ILayerGroupPresenter>& _layerGroupFactory;
};

} // namespace Addle

#endif // DOCUMENTPRESENTER_HPP
