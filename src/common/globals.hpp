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
#include "idtypes/brushengineid.hpp"
#include "idtypes/brushid.hpp"
#include "idtypes/formatid.hpp"
#include "idtypes/moduleid.hpp"
#include "idtypes/paletteid.hpp"
#include "idtypes/toolid.hpp"

#include "utilities/model/colorinfo.hpp"

#include <QtGlobal>
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

#define START_MODULE_IDS 0x01
namespace Modules
{
    constexpr ModuleId Core         = START_MODULE_IDS + 0x00;
    constexpr ModuleId WidgetsGui   = START_MODULE_IDS + 0x01;
}
DECLARE_STATIC_ID_METADATA(Modules::Core,       "org.addle.core");
DECLARE_STATIC_ID_METADATA(Modules::WidgetsGui, "org.addle.widgetsGui");

#undef START_MODULE_IDS

#define START_CORE_BRUSH_ENGINE_IDS 0x100
namespace CoreBrushEngines
{
    constexpr BrushEngineId PathEngine      = START_CORE_BRUSH_ENGINE_IDS + 0x00;
    constexpr BrushEngineId RasterEngine    = START_CORE_BRUSH_ENGINE_IDS + 0x01;
}
DECLARE_STATIC_ID_METADATA(CoreBrushEngines::PathEngine,    "path-brush-engine");
DECLARE_STATIC_ID_METADATA(CoreBrushEngines::RasterEngine,  "raster-brush-engine");

#undef START_CORE_BRUSH_ENGINE_IDS

#define START_CORE_BRUSH_IDS 0x200
#define START_CORE_ERASER_IDS 0x400
namespace CoreBrushes
{
    constexpr BrushId BasicBrush    = START_CORE_BRUSH_IDS + 0x00;
    constexpr BrushId SoftBrush     = START_CORE_BRUSH_IDS + 0x01;

    constexpr BrushId BasicEraser   = START_CORE_ERASER_IDS + 0x00;
}
DECLARE_STATIC_ID_METADATA(CoreBrushes::BasicBrush,     "basic-brush");
DECLARE_STATIC_ID_METADATA(CoreBrushes::SoftBrush,      "soft-brush");
DECLARE_STATIC_ID_METADATA(CoreBrushes::BasicEraser,    "basic-eraser");

#undef START_CORE_BRUSH_IDS
#undef START_CORE_ERASER_IDS

#define START_CORE_DOCUMENT_FORMAT_IDS 0x500
#define START_CORE_BRUSH_FORMAT_IDS 0x600
#define START_CORE_PALETTE_FORMAT_IDS 0x610
namespace CoreFormats
{
    constexpr DocumentFormatId PNG  = START_CORE_DOCUMENT_FORMAT_IDS + 0x00;
    constexpr DocumentFormatId JPEG = START_CORE_DOCUMENT_FORMAT_IDS + 0x01;
    constexpr DocumentFormatId GIF  = START_CORE_DOCUMENT_FORMAT_IDS + 0x02;
    constexpr DocumentFormatId WebP = START_CORE_DOCUMENT_FORMAT_IDS + 0x03;
    constexpr DocumentFormatId ORA  = START_CORE_DOCUMENT_FORMAT_IDS + 0x04;
}
DECLARE_STATIC_FORMAT_ID_METADATA(CoreFormats::PNG,     "format-png");
DECLARE_STATIC_FORMAT_ID_METADATA(CoreFormats::JPEG,    "format-jpeg");
DECLARE_STATIC_FORMAT_ID_METADATA(CoreFormats::GIF,     "format-gif");
DECLARE_STATIC_FORMAT_ID_METADATA(CoreFormats::WebP,    "format-webp");
DECLARE_STATIC_FORMAT_ID_METADATA(CoreFormats::ORA,     "format-ora");

#undef START_CORE_DOCUMENT_FORMAT_IDS
#undef START_CORE_BRUSH_FORMAT_IDS
#undef START_CORE_PALETTE_FORMAT_IDS

#define START_CORE_PALETTE_IDS 0x700
namespace CorePalettes
{
    constexpr PaletteId BasicPalette    = START_CORE_PALETTE_IDS + 0x00;
}
DECLARE_STATIC_ID_METADATA(CorePalettes::BasicPalette,  "basic-palette");

#undef START_CORE_PALETTE_IDS

#define START_TOOL_IDS 0x1000
namespace CoreTools
{
    constexpr ToolId Select     = START_TOOL_IDS + 0x00;
    constexpr ToolId Brush      = START_TOOL_IDS + 0x01;
    constexpr ToolId Eraser     = START_TOOL_IDS + 0x02;
    constexpr ToolId Fill       = START_TOOL_IDS + 0x03;
    constexpr ToolId Text       = START_TOOL_IDS + 0x04;
    constexpr ToolId Shapes     = START_TOOL_IDS + 0x05;
    constexpr ToolId Stickers   = START_TOOL_IDS + 0x06;
    constexpr ToolId Eyedrop    = START_TOOL_IDS + 0x07;
    constexpr ToolId Navigate   = START_TOOL_IDS + 0x08;
    constexpr ToolId Measure    = START_TOOL_IDS + 0x09;
}
DECLARE_STATIC_ID_METADATA(CoreTools::Select,   "select-tool");
DECLARE_STATIC_ID_METADATA(CoreTools::Brush,    "brush-tool");
DECLARE_STATIC_ID_METADATA(CoreTools::Eraser,   "eraser-tool");
DECLARE_STATIC_ID_METADATA(CoreTools::Fill,     "fill-tool");
DECLARE_STATIC_ID_METADATA(CoreTools::Text,     "text-tool");
DECLARE_STATIC_ID_METADATA(CoreTools::Shapes,   "shapes-tool");
DECLARE_STATIC_ID_METADATA(CoreTools::Stickers, "stickers-tool");
DECLARE_STATIC_ID_METADATA(CoreTools::Eyedrop,  "eyedrop-tool");
DECLARE_STATIC_ID_METADATA(CoreTools::Navigate, "navigate-tool");
DECLARE_STATIC_ID_METADATA(CoreTools::Measure,  "measure-tool");
#undef START_TOOL_IDS


namespace GlobalColorInfo
{
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

} // namespace Addle

#endif // GLOBALS_HPP
