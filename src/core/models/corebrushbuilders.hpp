#ifndef COREBRUSHBUILDERS_HPP
#define COREBRUSHBUILDERS_HPP

#include "utilities/model/brushbuilder.hpp"
#include "interfaces/models/ibrushmodel.hpp"

#include "globalconstants.hpp"

namespace CoreBrushBuilders
{

const static BrushBuilder basic = BrushBuilder()
    .setId(GlobalConstants::CoreBrushes::BasicBrush)
    .setEngine(GlobalConstants::CoreBrushEngines::PathEngine);

const static BrushBuilder soft = BrushBuilder()
    .setId(GlobalConstants::CoreBrushes::SoftBrush)
    .setEngine(GlobalConstants::CoreBrushEngines::RasterEngine)
    .setEngineParameters({
        { "mode", "Gradient" },
        { "hardness", 1.0 },
        { "spacing", 0.12 }
    });
}

#endif // COREBRUSHBUILDERS_HPP