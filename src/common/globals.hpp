/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "compat.hpp"

#include <boost/mp11.hpp>

#include "utilities/model/colorinfo.hpp"

#include <QtGlobal>
#include <QEvent>
#include <QColor>

namespace Addle
{
Q_NAMESPACE_EXPORT(ADDLE_COMMON_EXPORT)

const char* const ADDLE_NAME = "Addle";
const char* const ADDLE_VERSION = "0.0.1-dev";

//% "Addle"
const char* const ADDLE_NAME_TRID = QT_TRID_NOOP("global.addle-name");

//% "Pretty little drawing program and image utility"
const char* const ADDLE_TAGLINE_TRID = QT_TRID_NOOP("global.addle-tagline");

const bool DEBUG_BUILD =
#ifdef ADDLE_DEBUG
    true;
#else
    false;
#endif

enum ErrorCodes {
    COMMAND_LINE_ERROR_CODE = 1,
    
    UNKNOWN_ERROR_CODE = -1
};

extern ADDLE_COMMON_EXPORT const QColor DefaultBackgroundColor;

namespace BuiltinModules {
    constexpr auto Core         = "org.addle.core";
    constexpr auto WidgetsGui   = "org.addle.widgets_gui";
    
    constexpr const char* all_builtin_modules[] = { Core, WidgetsGui };
}

namespace CoreBrushEngines {
    constexpr auto PathEngine   = "org.addle.path-brush-engine";
    constexpr auto RasterEngine = "org.addle.raster-brush-engine";
    
    constexpr const char* all_brush_engines[] = { PathEngine, RasterEngine };
}

namespace CoreBrushes {
    constexpr auto BasicBrush   = "org.addle.basic-brush";
    constexpr auto SoftBrush    = "org.addle.soft-brush";
    constexpr auto BasicEraser  = "org.addle.basic-eraser";
    
    constexpr const char* all_brushes[] = { BasicBrush, SoftBrush, BasicEraser };
}

namespace CorePalettes {
    constexpr auto BasicPalette = "org.addle.basic-palette";
    
    constexpr const char* all_palettes[] = { BasicPalette };
}

namespace CoreTools {
    constexpr auto Select   = "org.addle.select-tool";
    constexpr auto Brush    = "org.addle.brush-tool";
    constexpr auto Eraser   = "org.addle.eraser-tool";
    constexpr auto Fill     = "org.addle.fill-tool";
    constexpr auto Text     = "org.addle.text-tool";
    constexpr auto Shapes   = "org.addle.shapes-tool";
    constexpr auto Stickers = "org.addle.stickers-tool";
    constexpr auto Eyedrop  = "org.addle.eyedrop-tool";
    constexpr auto Navigate = "org.addle.navigate-tool";
    constexpr auto Measure  = "org.addle.measure-tool";
    
    constexpr const char* all_tools[] = {
            Select, Brush, Eraser, Fill, Text, Shapes, Stickers, Eyedrop, 
            Navigate, Measure
        };
}

namespace GlobalColorInfo
{
    // ???
    extern ADDLE_COMMON_EXPORT const ColorInfo Black;
    extern ADDLE_COMMON_EXPORT const ColorInfo White;
    extern ADDLE_COMMON_EXPORT const ColorInfo Transparent;
    
    extern ADDLE_COMMON_EXPORT const QHash<QRgb, ColorInfo> ByRGB;
}

enum LayoutUnits
{
    pixels
};
Q_ENUM_NS(LayoutUnits)

enum EventTypes
{
    EventType_LayerRasterEditCommitted = QEvent::User + 100
};

} // namespace Addle

#endif // GLOBALS_HPP
