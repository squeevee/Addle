#ifndef COMMONCOMPAT_HPP

#include <QtGlobal>

#ifdef Q_OS_WINDOWS

#ifdef ADDLE_EXPORTING_COMMON
#define ADDLE_COMMON_EXPORT __declspec( dllexport )
#else
#define ADDLE_COMMON_EXPORT __declspec( dllimport )
#endif

#ifdef ADDLE_EXPORTING_CORE
#define ADDLE_CORE_EXPORT __declspec( dllexport )
#else
#define ADDLE_CORE_EXPORT __declspec( dllimport )
#endif

#ifdef ADDLE_EXPORTING_WIDGETSGUI
#define ADDLE_WIDGETSGUI_EXPORT __declspec( dllexport )
#else
#define ADDLE_WIDGETSGUI_EXPORT __declspec( dllimport )
#endif

#ifdef ADDLE_EXPORTING_EXTRA
#define ADDLE_EXTRA_EXPORT __declspec( dllexport )
#else
#define ADDLE_EXTRA_EXPORT __declspec( dllimport )
#endif

#else

#define ADDLE_COMMON_EXPORT
#define ADDLE_CORE_EXPORT
#define ADDLE_WIDGETSGUI_EXPORT
#define ADDLE_EXTRA_EXPORT

#endif

#endif // COMMONCOMPAT_HPP