#pragma once

#include <QString>
#include <QPainter>

#include "utilities/datatree/addledatatree.hpp"
// #include "utilities/datatree/observer.hpp"
#include "utilities/datatree/views.hpp"

#include "interfaces/rendering/irenderable.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {
    
class ILayerPresenter;
class ILayerGroupPresenter;

class IDocumentPresenter;
class ILayerNodePresenter : public virtual IAmQObject, public IRenderable
{
public:    
    using LayersTree        = AddleDataTree<QSharedPointer<ILayerNodePresenter>, aux_datatree::AddleDataTreeOptions_Observer>;
    using ConstLayersTree   = DataTreeCastView<LayersTree, QSharedPointer<const ILayerNodePresenter>>;
    
    using LayerNode         = typename LayersTree::Node;
    using ConstLayerNode    = typename ConstLayersTree::Node;
    
    using LayerNodeRef      = aux_datatree::NodeRef<LayersTree, false>;
    using ConstLayerNodeRef = aux_datatree::NodeRef<ConstLayersTree, true>;
    
    virtual ~ILayerNodePresenter() = default;
    
    virtual IDocumentPresenter& document() = 0;
    virtual const IDocumentPresenter& document() const = 0;

    virtual ConstLayerNodeRef layerNode() const = 0;
    virtual LayerNodeRef layerNode() = 0;
    
    virtual QString name() const = 0;
    virtual void setName(QString name) = 0;
    
    virtual QPainter::CompositionMode compositionMode() const = 0;
    virtual void setCompositionMode(QPainter::CompositionMode mode) = 0;
    
    virtual double opacity() const = 0;
    virtual void setOpacity(double opacity) = 0;
    
    virtual bool isVisible() const = 0;
    virtual void setVisibility(bool isVisible) = 0;
    
signals:
    virtual void nameChanged(QString name) = 0;
    virtual void compositionModeChanged(QPainter::CompositionMode mode) = 0;
    virtual void opacityChanged(double opacity) = 0;
    virtual void visibilityChanged(bool isVisible) = 0;
};
    
} // namespace Addle 

Q_DECLARE_INTERFACE(Addle::ILayerNodePresenter, "org.addle.ILayerNodePresenter")
Q_DECLARE_METATYPE(Addle::ILayerNodePresenter::LayerNodeRef)
