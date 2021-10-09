#pragma once

class QPainter;

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"
#include "./irenderable.hpp"

#include "utilities/render/renderroutine.hpp"

namespace Addle {
    
class IRenderer : public IAmQObject
{
public:
    enum Mode
    {
        // Optimizes the renderer for live rendering, with speed prioritized
        // over accuracy or memory conservancy.
        // - Routine data is precompiled, buffers and caches are preallocated.
        // - Caches and intermediate buffers use the ARGB32-premultiplied format
        // - timeout is initialized with a value of 0
        // - Only fast entity render functions block the calling thread. Slow
        // entity render functions are called from a worker thread pool, and
        // `renderChanged` is emitted for each when they return.
        Mode_Live,
        
        // Optimizes the renderer for IO rendering, with accuracy and memory
        // conservancy prioritized over speed.
        // - cacheCapacity is initialized with a value of 0, routine data is
        // not precompiled, caches are not preallocated
        // - Caches and intermediate buffers use the ARGB32 format.
        // - timeout is initialized with a value of -1
        // - Slow entity render functions are called from a worker thread pool,
        // but the calling thread is blocked until all rendering is complete.
        // Fast render functions are not called if a slow entity render function
        // is available.
        Mode_IO
    };
    
    virtual ~IRenderer() = default;
    
    virtual Mode mode() const = 0;
    
    // The renderable associated with this renderer
    virtual const IRenderable* renderable() const = 0;
    
    virtual RenderRoutine routine() const = 0;
    
    // If the resulting render is expected to be scaled, then the client can set
    // the scaleHint to the expected scaling factor. The renderer may use this
    // value to apply optimizations.
    virtual double scaleHint() const = 0;
    virtual void setScaleHint(double scale) = 0;

    virtual int cacheCapacity() const = 0;
    virtual void setCacheCapacity(int capacity) = 0;
    
    virtual void clearCache() = 0;
    
    virtual int timeout() const = 0;
    virtual void setTimeout(int timeout) = 0;
    
    virtual void render(QPainter& painter, QRegion region) const = 0;
    
signals:
    virtual void renderChanged(QRegion region) = 0;
};
    
namespace aux_IRenderer {
    ADDLE_FACTORY_PARAMETER_NAME( mode )
    ADDLE_FACTORY_PARAMETER_NAME( renderable )
    ADDLE_FACTORY_PARAMETER_NAME( routine )
    
    //using renderable_t = std::variant<const IRenderable*, RenderRoutine>;
}

ADDLE_DECL_MAKEABLE(IRenderer)
ADDLE_DECL_FACTORY_PARAMETERS(
    IRenderer,
    (required 
        (mode,          (IRenderer::Mode))
    )
    (optional
        (renderable,    (const IRenderable*),   nullptr)
        (routine,       (RenderRoutine),        RenderRoutine())
    )
)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IRenderer, "org.addle.IRenderer")

