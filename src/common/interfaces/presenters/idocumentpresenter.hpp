#ifndef IDOCUMENTPRESENTER_HPP
#define IDOCUMENTPRESENTER_HPP

#include <QUrl>
#include <QFileInfo>
#include <QWeakPointer>
#include <QSet>
#include <QMap>

#include "iraiseerrorpresenter.hpp"
#include "ihavetoolspresenter.hpp"

#include "interfaces/models/idocument.hpp"

#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/metaobjectinfo.hpp"

#include "utilities/heirarchylist.hpp"

#include "utilities/qtextensions/qhash.hpp"

class ICanvasPresenter;
class ILayerPresenter;
class IDocumentPresenter
{
public:
    typedef HeirarchyList<QSharedPointer<ILayerPresenter>> LayerList;
    typedef HeirarchyList<QSharedPointer<ILayerPresenter>>::Node LayerNode;
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

    virtual QSharedPointer<IDocument> getModel() = 0;

    virtual bool isEmpty() = 0;
    
    virtual QSize getSize() = 0;
    virtual QRect getRect() = 0;
    virtual QColor getBackgroundColor() = 0;

    virtual LayerList layers() const = 0;

    virtual QSet<LayerNode*> layerSelection() const = 0;
    virtual void setLayerSelection(QSet<LayerNode*> layer) = 0;
    virtual void addLayerSelection(QSet<LayerNode*> layer) = 0;
    virtual void removeLayerSelection(QSet<LayerNode*> layer) = 0;

    virtual QSharedPointer<ILayerPresenter> topSelectedLayer() const = 0;

public slots:
    virtual LayerNode& addLayer() = 0;
    virtual LayerNode& addLayerGroup() = 0;
    
    virtual void removeSelectedLayers() = 0;
    virtual void moveSelectedLayers(int destination) = 0;
    virtual void mergeSelectedLayers() = 0;

signals:
    virtual void layersAdded(QList<IDocumentPresenter::LayerNode*> added) = 0;
    virtual void layersRemoved(QList<IDocumentPresenter::LayerNode*> removed) = 0;
    virtual void layersMoved(QList<IDocumentPresenter::LayerNode*> moved) = 0;

    virtual void layersChanged(IDocumentPresenter::LayerList layers) = 0;
    virtual void topSelectedLayerChanged(QSharedPointer<ILayerPresenter>) = 0;
    virtual void layerSelectionChanged(QSet<IDocumentPresenter::LayerNode*> selection) = 0;

};

Q_DECLARE_METATYPE(IDocumentPresenter::LayerList);

DECL_MAKEABLE(IDocumentPresenter);
DECL_EXPECTS_INITIALIZE(IDocumentPresenter);
DECL_IMPLEMENTED_AS_QOBJECT(IDocumentPresenter);

#endif // IDOCUMENTPRESENTER_HPP