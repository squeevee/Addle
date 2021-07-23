#pragma once

class QPaintDevice;

#include "interfaces/iamqobject.hpp"
#include "./irenderable2.hpp"

namespace Addle {
    
class IRenderer2 : public IAmQObject
{
public:
    virtual ~IRenderer2() = default;
    
    virtual void setLiveHint(bool hint) = 0;
    virtual bool liveHint() const = 0;
    
    virtual void setRenderable(IRenderable2& renderable) = 0;
    virtual IRenderable2& renderable() const = 0;
    
    virtual void clearCache() const = 0;
    
    virtual void render(QPaintDevice* device) const = 0;
    
signals:
    virtual void renderChanged(QRect area) = 0;
};
    
} // namespace Addle
