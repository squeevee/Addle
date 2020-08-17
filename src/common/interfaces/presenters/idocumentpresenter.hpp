/**
 * Addle source code
 * 
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

#include "utilities/heirarchylist.hpp"

namespace Addle {

class IDocument;
class ICanvasPresenter;
class ILayerPresenter;

class IDocumentPresenter : public virtual IAmQObject
{
public:
    typedef HeirarchyList<QSharedPointer<ILayerPresenter>> LayerList;
    typedef HeirarchyList<QSharedPointer<ILayerPresenter>>::Node LayerNode;
    typedef HeirarchyList<QSharedPointer<ILayerPresenter>>::NodeRemoved LayerNodeRemoved;
    enum EmptyInitOptions
    {
        initNoModel,
        initEmptyModel,
        initBlankDefaults
    };

    virtual ~IDocumentPresenter() = default;

    virtual void initialize(EmptyInitOptions option = initNoModel) = 0;
    virtual void initialize(QSize size, QColor backgroundColor) = 0;
    virtual void initialize(QSharedPointer<IDocument> model) = 0;

    virtual QSharedPointer<IDocument> model() = 0;

    virtual bool isEmpty() = 0;
    
    virtual QSize size() = 0;
    virtual QRect rect() = 0;
    virtual QColor backgroundColor() = 0;

    virtual const LayerList& layers() const = 0;

    virtual QSet<LayerNode*> layerSelection() const = 0;
    virtual void setLayerSelection(QSet<LayerNode*> layer) = 0;
    virtual void addLayerSelection(QSet<LayerNode*> layer) = 0;
    virtual void removeLayerSelection(QSet<LayerNode*> layer) = 0;

    virtual QSharedPointer<ILayerPresenter> topSelectedLayer() const = 0;

public slots:
    virtual void addLayer() = 0;
    virtual void addLayerGroup() = 0;
    
    virtual void removeSelectedLayers() = 0;
    virtual void moveSelectedLayers(int destination) = 0;
    virtual void mergeSelectedLayers() = 0;

signals:
    virtual void layersAdded(QList<IDocumentPresenter::LayerNode*> added) = 0;
    virtual void layersRemoved(QList<IDocumentPresenter::LayerNodeRemoved> removed) = 0;
    virtual void layersMoved(QList<IDocumentPresenter::LayerNode*> moved) = 0;

    virtual void layersChanged() = 0;
    virtual void topSelectedLayerChanged(QSharedPointer<ILayerPresenter>) = 0;
    virtual void layerSelectionChanged(QSet<IDocumentPresenter::LayerNode*> selection) = 0;

};

DECL_MAKEABLE(IDocumentPresenter);
DECL_EXPECTS_INITIALIZE(IDocumentPresenter);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IDocumentPresenter, "org.addle.IDocumentPresenter")
Q_DECLARE_METATYPE(Addle::IDocumentPresenter::LayerNodeRemoved);

#endif // IDOCUMENTPRESENTER_HPP