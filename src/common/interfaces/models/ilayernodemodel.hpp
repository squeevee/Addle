#pragma once

#include <QPainter>
#include "utilities/datatree/addledatatree.hpp"
#include "utilities/datatree/views.hpp"
// #include "utilities/datatree/observer.hpp"

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"


namespace Addle {
    
class IDocument;
class ILayerNodeModel : public virtual IAmQObject
{
public:
    using LayersTree        = AddleDataTree<
                                    QSharedPointer<ILayerNodeModel>
                                >;
    using ConstLayersTree   = DataTreeCastView<
                                    LayersTree, 
                                    QSharedPointer<const ILayerNodeModel>
                                >;
    
    using LayerNode =       typename LayersTree::Node;
    using ConstLayerNode =  typename ConstLayersTree::Node;
    
    virtual ~ILayerNodeModel() = default;
    
    virtual IDocument& document() = 0;
    virtual const IDocument& document() const = 0;
    
    virtual const LayerNode& layerNode() = 0;
    virtual ConstLayerNode layerNode() const = 0;
    
    virtual QRect boundary() const = 0;

    virtual QColor skirtColor() const = 0;
    virtual void setSkirtColor(QColor skirtColor) = 0;
    
    virtual QString name() const = 0;
    virtual void setName(QString name) = 0;
    
    virtual QPainter::CompositionMode compositionMode() const = 0;
    virtual void setCompositionMode(QPainter::CompositionMode mode) = 0;
    
    virtual double opacity() const = 0;
    virtual void setOpacity(double opacity) = 0;
    
signals:
    virtual void boundaryChanged(QRect boundary) = 0;
    virtual void skirtColorChanged(QColor skirtColor) = 0;
    virtual void nameChanged(QString name) = 0;
    virtual void compositionModeChanged(QPainter::CompositionMode compositionMode) = 0;
    virtual void opacityChanged(double opacity) = 0;
};
    
}
