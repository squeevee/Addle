#pragma once

#include "interfaces/iamqobject.hpp"
#include "interfaces/rendering/irenderable.hpp"

namespace Addle {
 
class ILayer;
class IDocumentAnimationFrame;
class ILayerAnimationFrame : public IRenderable, public virtual IAmQObject
{
public:
    virtual ~ILayerAnimationFrame() = default;
    
    virtual ILayer& layer() = 0;
    virtual const ILayer& layer() const = 0;
    
    virtual int layerIndex() const = 0;
    virtual void setLayerIndex(int index) = 0;
    
    virtual int documentIndex() const = 0;
    virtual void setDocumentIndex(int index) = 0;
    
    virtual int documentIndexSpan() const = 0;
    virtual void setDocumentIndexSpan(int span) = 0;
    
    virtual double duration() const = 0;
    
    virtual QList<QSharedPointer<IDocumentAnimationFrame>> documentFrames() const = 0;
    
signals:
    virtual void layerIndexChanged(int index) = 0;
    virtual void documentIndexChanged(int index) = 0;
    virtual void documentIndexSpanChanged(int index) = 0;
    virtual void durationChanged(double duration) = 0;
};
    
}

