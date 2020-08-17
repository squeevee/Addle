#ifndef COREBRUSHBUILDERS_HPP
#define COREBRUSHBUILDERS_HPP

#include "utilities/model/brushbuilder.hpp"
#include "interfaces/models/ibrush.hpp"

#include "globals.hpp"
namespace Addle {

namespace CoreBrushBuilders
{

const static BrushBuilder basic = BrushBuilder()
    .setId(CoreBrushes::BasicBrush)
    .setEngine(CoreBrushEngines::PathEngine)
    .setPreferredStartingSize(7.0);

const static BrushBuilder soft = BrushBuilder()
    .setId(CoreBrushes::SoftBrush)
    .setEngine(CoreBrushEngines::RasterEngine)
    .setCustomEngineParameters({
        { "mode", "Gradient" },
        { "hardness", 1.0 },
        { "spacing", 0.12 }
    })
    .setMinSize(21.0)
    .setPreferredStartingSize(60.0);

const static BrushBuilder basicEraser = BrushBuilder()
    .setId(CoreBrushes::BasicEraser)
    .setEngine(CoreBrushEngines::PathEngine)
    .setPreviewHints(IBrush::Subtractive)
    .setEraserMode(true)
    .setPreferredStartingSize(21.0);

}

} // namespace Addle
#endif // COREBRUSHBUILDERS_HPP