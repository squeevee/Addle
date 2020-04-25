#ifndef qtextensions__TRANSLATION_H
#define qtextensions__TRANSLATION_H

#include "compat.hpp"
#include <QString>
#include <QCoreApplication>

// Translates a string, using a different string as a fallback rather than the
// key. This is useful for debug messages which tend to be long and complex, but
// should also be fail-safe with at least one human-readable version specified
// in source code.

ADDLE_COMMON_EXPORT QString fallback_translate(const char* context, const char* key, const QString fallback);

#endif // qtextensions__TRANSLATION_H