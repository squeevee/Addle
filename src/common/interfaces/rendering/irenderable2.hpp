#pragma once

#include "utilities/datatree/observer.hpp"
#include "utilities/render/renderroutine.hpp"

#include "interfaces/iamqobject.hpp"

namespace Addle {

class IRenderEntity;
class IRenderable2 : public virtual IAmQObject
{
public:
    virtual ~IRenderable2() = default;
    
    virtual RenderRoutine renderRoutine() const = 0;

signals:
    virtual void renderRoutineChanged(RenderRoutineChangedEvent) = 0;
    virtual void renderChanged(QRegion affected, const IRenderEntity* entity = nullptr) = 0;
};

}
