/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#pragma once

#include "compat.hpp"
#include <QObject>
#include <QList>
#include <QString>
#include <QPaintDevice>

#include "interfaces/models/idocument.hpp"
#include "interfaces/models/ilayer.hpp"
#include "interfaces/models/ilayergroup.hpp"

#include "interfaces/services/ifactory.hpp"

#include "utilities/datatree/nestedobjectadapter.hpp"

namespace Addle {
class ADDLE_CORE_EXPORT Document : public QObject, public IDocument
{
    Q_OBJECT
    Q_INTERFACES(Addle::IDocument)
    IAMQOBJECT_IMPL
public:
    
    Document(
            const DocumentBuilder& builder, 
            const IFactory<ILayer>& layerFactory,
            const IFactory<ILayerGroup>& layerGroupFactory
        );
    virtual ~Document() = default;

//     void render(QRect area, QPaintDevice* device) const override;

//     bool isEmpty() const override { return _empty; }

    QSize size() const override { return _size; }

    QColor backgroundColor() const override { return _backgroundColor; }
    void setBackgroundColor(QColor color) override
    {
        if (_backgroundColor != color)
        {
            _backgroundColor = color;
            emit backgroundColorChanged(_backgroundColor);
        }
    }

    QUrl url() const override { return _url; }
    void setUrl(QUrl url) override
    { 
        if (_url != url)
        {
            _url = url;
            emit urlChanged(_url);
        }
    }

    const LayersTree& layers() override { return _layers; }
    ConstLayersTree layers() const override { return _layers; }

    aux_datatree::NodeRange<LayersTree> insertLayerNodes(
            DataTreeNodeAddress startPos,
            QList<LayerNodeBuilder> layerNodeBuilders
        ) override;
        
    void removeLayers(QList<DataTreeNodeChunk>) override;
        
signals:
//     void boundaryChanged(QRect change) override;
    void backgroundColorChanged(QColor color) override;
    void urlChanged(QUrl url) override;
    
    void layerNodesChanged(DataTreeNodeEvent) override; 

private:
    using builder_adapter_t = aux_datatree::NestedObjectAdapterImpl<   
            LayerNodeBuilder,
            QList<LayerNodeBuilder> (LayerNodeBuilder::*)() const,
            QList<LayerNodeBuilder>
        >;
        
    void populateLayerNode_p(
            aux_datatree::const_node_handle_t<builder_adapter_t>, 
            ILayerNodeModel::LayerNode*);
    
    LayersTree _layers;
    
    QSize _size;
    QColor _backgroundColor;
    QUrl _url;

//     void layersChanged(QList<ILayer*> layers);
//     QRect unitedBoundary();
    
    const IFactory<ILayer>& _layerFactory;
    const IFactory<ILayerGroup>& _layerGroupFactory;
};

} // namespace Addle
