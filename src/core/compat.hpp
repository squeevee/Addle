#ifndef CORE__COMPAT_HPP
#define CORE__COMPAT_HPP

#include <QtGlobal>

#ifdef Q_CC_MSVC

#ifdef ADDLE_BUILDING_CORE
#define ADDLE_CORE_EXPORT __declspec(dllexport)
#else 
#define ADDLE_CORE_EXPORT __declspec(dllimport)
#endif

#else

#define ADDLE_CORE_EXPORT

#endif

#endif // CORE__COMPAT_HPP