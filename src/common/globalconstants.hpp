#ifndef GLOBALCONSTANTS_HPP
#define GLOBALCONSTANTS_HPP

#include "interfaces/traits/compat.hpp"
#include "idtypes/brushid.hpp"
#include "idtypes/toolid.hpp"
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

	extern const QColor DEFAULT_BACKGROUND_COLOR;

	namespace CoreBrushes
	{
		extern const ADDLE_COMMON_EXPORT BrushId BasicBrush;
	}

	namespace CoreTools
	{
		extern const ADDLE_COMMON_EXPORT ToolId SELECT;
		extern const ADDLE_COMMON_EXPORT ToolId BRUSH;
		extern const ADDLE_COMMON_EXPORT ToolId ERASER;
		extern const ADDLE_COMMON_EXPORT ToolId FILL;
		extern const ADDLE_COMMON_EXPORT ToolId TEXT;
		extern const ADDLE_COMMON_EXPORT ToolId SHAPES;
		extern const ADDLE_COMMON_EXPORT ToolId STICKERS;
		extern const ADDLE_COMMON_EXPORT ToolId EYEDROP;
		extern const ADDLE_COMMON_EXPORT ToolId NAVIGATE;
		extern const ADDLE_COMMON_EXPORT ToolId MEASURE;
	}

	namespace CoreFormats
	{
		extern const ADDLE_COMMON_EXPORT FormatId PNG;
		extern const ADDLE_COMMON_EXPORT FormatId JPEG;
	}
}

#endif // GLOBALCONSTANTS_HPP