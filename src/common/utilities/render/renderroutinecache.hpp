#pragma once

#include "renderroutine.hpp"

namespace Addle {

/**
 * Caches certain data of a render routine and its components, sub-components.
 */
class RenderRoutineCache
{
public:
    RenderRoutineCache() = default;
    RenderRoutineCache(const RenderRoutineCache&) = delete;
    
    void onSubRoutineChanged(RenderRoutineChangedEvent event)
    {
    }
    
    void onDecendantRenderChanged(QRegion affected, const IRenderEntity* entity = nullptr)
    {
    }
    
private:
    std::map<double, QRect> _areas;
};

    
}
