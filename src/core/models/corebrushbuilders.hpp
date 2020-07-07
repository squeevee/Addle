#ifndef COREBRUSHBUILDERS_HPP
#define COREBRUSHBUILDERS_HPP

#include "utilities/model/brushbuilder.hpp"
#include "interfaces/models/ibrushmodel.hpp"

#include "globalconstants.hpp"

namespace CoreBrushBuilders
{

const static BrushBuilder basic = BrushBuilder()
    .setId(GlobalConstants::CoreBrushes::BasicBrush)
    .setEngine(GlobalConstants::CoreBrushEngines::PathEngine)
    .setPreferredStartingSize(7.0);

const static BrushBuilder soft = BrushBuilder()
    .setId(GlobalConstants::CoreBrushes::SoftBrush)
    .setEngine(GlobalConstants::CoreBrushEngines::RasterEngine)
    .setCustomEngineParameters({
        { "mode", "Gradient" },
        { "hardness", 1.0 },
        { "spacing", 0.12 }
    })
    .setMinSize(21.0)
    .setPreferredStartingSize(60.0);

const static BrushBuilder basicEraser = BrushBuilder()
    .setId(GlobalConstants::CoreBrushes::BasicEraser)
    .setEngine(GlobalConstants::CoreBrushEngines::PathEngine)
    .setPreviewHints(IBrushModel::Subtractive)
    .setEraserMode(true)
    .setPreferredStartingSize(21.0);

}

#endif // COREBRUSHBUILDERS_HPP