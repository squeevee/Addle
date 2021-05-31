/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IDOCUMENTPRESENTER_HPP
#define IDOCUMENTPRESENTER_HPP

#include <QList>
#include <QSet>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"
#include "interfaces/metaobjectinfo.hpp"

#include "icansavepresenter.hpp"

#include "./ilayernodepresenter.hpp"

#include "utilities/model/documentbuilder.hpp"

namespace Addle {

class IDocument;
class ICanvasPresenter;
class ILayerPresenter;
class IDocumentPresenter : public ICanSavePresenter, public virtual IAmQObject
{
public:
    using LayersTree                = ILayerNodePresenter::LayersTree;
    using ConstLayersTree           = ILayerNodePresenter::ConstLayersTree;
//     using LayerNodesAddedEvent      = DataTreeNodesAddedEvent<LayersTree>;
//     using LayerNodesRemovedEvent    = DataTreeNodesRemovedEvent<LayersTree>;
    
    enum EmptyInitOptions
    {
        initNoModel,
        initEmptyModel,
        initBlankDefaults
    };

    virtual ~IDocumentPresenter() = default;

    virtual QSharedPointer<IDocument> model() = 0;
    
    virtual QSize size() = 0;
    virtual QRect rect() = 0;
    virtual QColor backgroundColor() = 0;

    virtual const LayersTree& layers() = 0;
    virtual ConstLayersTree layers() const = 0;

    virtual QList<DataTreeNodeAddress> layerSelection() const = 0;
    virtual void setLayerSelection(QList<DataTreeNodeAddress> selection) = 0;
    virtual void addLayerSelection(QList<DataTreeNodeAddress> added) = 0;
    virtual void subtractLayerSelection(QList<DataTreeNodeAddress> removed) = 0;

    virtual QSharedPointer<ILayerPresenter> topSelectedLayer() const = 0;

    virtual bool isLocalFile() const = 0;

public slots:
    virtual void addLayer() = 0;
    virtual void addLayerGroup() = 0;
    
    virtual void removeSelectedLayers() = 0;
    virtual void moveSelectedLayers(int destination) = 0;
    virtual void mergeSelectedLayers() = 0;

signals:
//     virtual void layerNodesAdded(IDocumentPresenter::LayerNodesAddedEvent added) = 0;
//     virtual void layerNodesRemoved(IDocumentPresenter::LayerNodesRemovedEvent removed) = 0;
    //virtual void layersMoved(QList<IDocumentPresenter::LayerNode*> moved) = 0;

    virtual void layersChanged() = 0;
    virtual void topSelectedLayerChanged(QSharedPointer<ILayerPresenter>) = 0;
    virtual void layerSelectionChanged(QList<DataTreeNodeAddress> selection) = 0;

};

namespace aux_IDocumentPresenter
{
    ADDLE_FACTORY_PARAMETER_NAME( model )
    ADDLE_FACTORY_PARAMETER_NAME( modelBuilder )
}

ADDLE_DECL_MAKEABLE(IDocumentPresenter);
ADDLE_DECL_FACTORY_PARAMETERS(
    IDocumentPresenter,
    (optional 
        ( model, (QSharedPointer<IDocument>), nullptr )
        ( modelBuilder, (const DocumentBuilder&), DocumentBuilder() )
    )
)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IDocumentPresenter, "org.addle.IDocumentPresenter")
// Q_DECLARE_METATYPE(Addle::IDocumentPresenter::LayerNodesAddedEvent)
// Q_DECLARE_METATYPE(Addle::IDocumentPresenter::LayerNodesRemovedEvent)

#endif // IDOCUMENTPRESENTER_HPP
