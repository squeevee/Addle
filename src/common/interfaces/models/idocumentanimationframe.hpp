#pragma once

#include "interfaces/iamqobject.hpp"
#include "interfaces/rendering/irenderable.hpp"

namespace Addle {
 
class IDocument;
class ILayerAnimationFrame;
class IDocumentAnimationFrame : public IRenderable, public virtual IAmQObject
{
public:
    virtual ~IDocumentAnimationFrame() = default;
    
    virtual IDocument& document() = 0;
    virtual const IDocument& document() const = 0;
    
    virtual int index() const = 0;
    virtual void setIndex(int index) = 0;
    
    virtual double duration() const = 0;
    virtual void setDuration(double duration) = 0;
    
    virtual QList<QSharedPointer<ILayerAnimationFrame>> layerFrames() const = 0;
    virtual QList<QSharedPointer<ILayerAnimationFrame>> layerKeyFrames() const = 0;
    
signals:
    virtual void indexChanged(int index) = 0;
    virtual void durationChanged(double duration) = 0;
};
    
}
