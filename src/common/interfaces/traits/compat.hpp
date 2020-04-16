#ifndef COMMON__COMPAT_HPP
#define COMMON__COMPAT_HPP

#include <QtGlobal>

#ifdef Q_CC_MSVC

#ifdef ADDLE_BUILDING_COMMON
#define ADDLE_COMMON_EXPORT __declspec(dllexport)
#else 
#define ADDLE_COMMON_EXPORT __declspec(dllimport)
#endif

#else

#define ADDLE_COMMON_EXPORT

#endif

#endif // COMMON_COMPAT_HPP