#pragma once

#include "interfaces/rendering/irenderable.hpp"
#include "utilities/image/tilesurface.hpp"

namespace Addle {
    
class ILayer;
class ILayerAnimationFrame;
class ILayerSurface
{
public:
    virtual ~ILayerSurface() = default;
    
    virtual ILayer* layer() = 0;
    virtual const ILayer* layer() const = 0;
    
    virtual ILayerAnimationFrame* frame() = 0;
    virtual const ILayerAnimationFrame* frame() const = 0;
};
    
}
