#pragma once

#include "interfaces/editing/irastersurface2.hpp"

namespace Addle {
    
class RasterSurface2 : public QObject, public IRasterSurface2
{
    Q_OBJECT
    Q_INTERFACES(Addle::IRasterSurface2)
    IAMQOBJECT_IMPL
public:
    RasterSurface2() = default;
    virtual ~RasterSurface2() = default;
    
    RenderRoutine renderRoutine() const override { return {}; }

signals:
    void renderRoutineChanged(RenderRoutineChangedEvent) override;
    void renderChanged(QRegion affected, DataTreeNodeAddress entity = {}) override;
};
    
}
