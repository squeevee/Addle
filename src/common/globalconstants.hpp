#ifndef GLOBALCONSTANTS_HPP
#define GLOBALCONSTANTS_HPP

#include "compat.hpp"
#include "idtypes/brushid.hpp"
#include "idtypes/formatid.hpp"

#include <QColor>

namespace GlobalConstants
{
    const char* const ADDLE_VERSION = "0.0.1-dev";

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

    namespace CoreBrushes
    {
        extern ADDLE_COMMON_EXPORT const BrushId BasicBrush;
    };

    namespace CoreFormats
    {
        extern ADDLE_COMMON_EXPORT const FormatId JPEG;
        extern ADDLE_COMMON_EXPORT const FormatId PNG;
    }

}

#endif // GLOBALCONSTANTS_HPP