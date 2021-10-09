/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ILAYER_HPP
#define ILAYER_HPP

#include <QPainter>

#include <QRect>
#include <QPoint>
#include <QImage>
#include <QPainter>

#include "utilities/model/layernodebuilder.hpp"

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

#include "./ilayernodemodel.hpp"

namespace Addle {

//class IRasterSurface;
class TileBuffer;
class IRenderable;

// WARNING Behavior is undefined if ILayer outlives the IDocument it was
// initialized with -- be aware of this if keeping a persistent shared reference
// to ILayer
class ILayer : public ILayerNodeModel, public virtual IAmQObject
{
public:
    virtual ~ILayer() = default;

//     virtual const TileBuffer& rasterSurface() const = 0;
    
    
//     virtual QSharedPointer<IRasterSurface> rasterSurface() = 0;
//     virtual QSharedPointer<const IRasterSurface> rasterSurface() const = 0;
    
    
    
//     virtual QSharedPointer<IRenderable> renderable() = 0;
//     virtual QSharedPointer<const IRenderable> renderable() const = 0;
};

namespace aux_ILayer {
    ADDLE_FACTORY_PARAMETER_NAME( document )
    ADDLE_FACTORY_PARAMETER_NAME( layerNode )
    ADDLE_FACTORY_PARAMETER_NAME( builder )
}

ADDLE_DECL_MAKEABLE(ILayer)
ADDLE_DECL_FACTORY_PARAMETERS(
    ILayer,
    (required 
        (document,  (IDocument&))
        (layerNode, (ILayer::LayerNodeRef))
    )
    (optional (builder, (const LayerNodeBuilder), LayerNodeBuilder {}))
)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::ILayer, "org.addle.ILayer")

#endif // ILAYER_HPP
