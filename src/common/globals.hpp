#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "compat.hpp"
#include "idtypes/brushengineid.hpp"
#include "idtypes/brushid.hpp"
#include "idtypes/formatid.hpp"
#include "idtypes/paletteid.hpp"

#include "utilities/model/colorinfo.hpp"

#include <QtGlobal>
#include <QColor>

namespace Addle
{
Q_NAMESPACE_EXPORT(ADDLE_COMMON_EXPORT)

const char* const ADDLE_VERSION = "0.0.1-dev";

//% "Pretty little drawing program and image utility"
const char* const ADDLE_TAGLINE_TRID = QT_TRID_NOOP("addle-tagline");

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

#ifdef ADDLE_DEBUG
const char* const DEBUG_BEHAVIOR_ENV_VARIABLE_NAME = "ADDLE_DEBUG_BEHAVIOR";

const char* const DEBUG__NONE_TYPE_NAME = "<No type given>";
#endif

extern ADDLE_COMMON_EXPORT const QColor DefaultBackgroundColor;

namespace CoreBrushEngines
{
    extern ADDLE_COMMON_EXPORT const BrushEngineId PathEngine;
    extern ADDLE_COMMON_EXPORT const BrushEngineId RasterEngine;
};

namespace CoreBrushes
{
    extern ADDLE_COMMON_EXPORT const BrushId BasicBrush;
    extern ADDLE_COMMON_EXPORT const BrushId SoftBrush;

    extern ADDLE_COMMON_EXPORT const BrushId BasicEraser;
};

namespace CoreFormats
{
    extern ADDLE_COMMON_EXPORT const FormatId JPEG;
    extern ADDLE_COMMON_EXPORT const FormatId PNG;
}

namespace CorePalettes
{
    extern ADDLE_COMMON_EXPORT const PaletteId BasicPalette;
}

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