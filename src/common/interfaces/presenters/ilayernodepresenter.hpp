#pragma once

#include <QString>
#include <QPainter>

#include "utilities/datatree/basicdatatree.hpp"
// #include "utilities/datatree/observers.hpp"
#include "utilities/datatree/views.hpp"

#include "interfaces/iamqobject.hpp"

namespace Addle {
    
class IDocumentPresenter;
class ILayerGroupPresenter;
class ILayerNodePresenter : public virtual IAmQObject
{
public:    
    using LayersTree        = BasicDataTree<
                                    QSharedPointer<ILayerNodePresenter>
                                >;
    using ConstLayersTree   = DataTreeCastView<
                                    LayersTree,
                                    QSharedPointer<const ILayerNodePresenter>
                                >;
    using LayerNode         = LayersTree::Node;
    using ConstLayerNode    = ConstLayersTree::Node;
    
    virtual ~ILayerNodePresenter() = default;
    
    virtual IDocumentPresenter& document() = 0;
    virtual const IDocumentPresenter& document() const = 0;

    virtual ConstLayerNode layerNode() const = 0;
    virtual const LayerNode& layerNode() = 0;
    
//     inline QSharedPointer<const ILayerGroupPresenter> parentLayerGroup() const
//     {
//         if (layerNode().parent())
//             return (*layerNode().parent()).branchValue();
//         else
//             return nullptr;
//     }
//     
//     inline QSharedPointer<ILayerGroupPresenter> parentLayerGroup() 
//     {
//         if (layerNode().parent())
//             return layerNode().parent()->branchValue();
//         else
//             return nullptr;
//     }
    
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
