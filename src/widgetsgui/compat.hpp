#ifndef WIDGETSGUI__COMPAT_HPP
#define WIDGETSGUI__COMPAT_HPP

#include <QtGlobal>

#ifdef Q_CC_MSVC

#ifdef ADDLE_BUILDING_WIDGETSGUI
#define ADDLE_WIDGETSGUI_EXPORT __declspec(dllexport)
#else 
#define ADDLE_WIDGETSGUI_EXPORT __declspec(dllimport)
#endif

#else

#define ADDLE_WIDGETSGUI_EXPORT

#endif

#endif // WIDGETSGUI__COMPAT_HPP