#pragma once

#include "utilities/datatree/observer.hpp"
#include "utilities/render/renderroutine.hpp"

#include "interfaces/iamqobject.hpp"

namespace Addle {

class IRenderEntity;
class IRenderable : public virtual IAmQObject
{
public:
    virtual ~IRenderable() = default;
    
    virtual RenderRoutine renderRoutine() const = 0;

signals:
    virtual void renderRoutineChanged(RenderRoutineChangedEvent) = 0;
    virtual void renderChanged(QRegion affected, DataTreeNodeAddress entity = {}) = 0;
};

}

Q_DECLARE_INTERFACE(Addle::IRenderable, "org.addle.IRenderable")
