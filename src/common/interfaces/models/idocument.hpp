/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#pragma once

#include <QSize>
#include <QColor>
#include <QString>
#include <QSharedPointer>
#include <QUrl>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

#include "utilities/datatree/addledatatree.hpp"
#include "utilities/datatree/views.hpp"
#include "utilities/datatree/observer.hpp"

#include "utilities/model/documentbuilder.hpp"
#include "utilities/model/layernodebuilder.hpp"

#include "./ilayernodemodel.hpp"

namespace Addle {

class ILayer;
class ILayerGroup;
class IDocument : public virtual IAmQObject
{
public:
    using LayersTree                = ILayerNodeModel::LayersTree;
    using ConstLayersTree           = ILayerNodeModel::ConstLayersTree;
    
    virtual ~IDocument() = default;

    virtual QSize size() const = 0;
    
    virtual QColor backgroundColor() const = 0;
    virtual void setBackgroundColor(QColor color) = 0;

    virtual QUrl url() const = 0;
    virtual void setUrl(QUrl url) = 0;
    
    virtual const LayersTree& layers() = 0;
    virtual ConstLayersTree layers() const = 0;

    virtual aux_datatree::NodeRange<LayersTree> insertLayerNodes(
            DataTreeNodeAddress startPos,
            QList<LayerNodeBuilder> layerNodeBuilders
        ) = 0;
        
    virtual void removeLayers(QList<DataTreeNodeChunk>) = 0;
    
signals:
    virtual void backgroundColorChanged(QColor color) = 0;
    virtual void urlChanged(QUrl url) = 0;
    
    virtual void layerNodesChanged(DataTreeNodeEvent) = 0; 
};

namespace aux_IDocument {
    ADDLE_FACTORY_PARAMETER_NAME( builder )
}

ADDLE_DECL_MAKEABLE(IDocument)
ADDLE_DECL_FACTORY_PARAMETERS(
    IDocument,
    (optional (builder, (const DocumentBuilder&), DocumentBuilder {}))
)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IDocument, "org.addle.IDocument")
