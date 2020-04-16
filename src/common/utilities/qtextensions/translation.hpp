#ifndef qtextensions__TRANSLATION_H
#define qtextensions__TRANSLATION_H

#include "interfaces/traits/compat.hpp"
#include <QString>
#include <QCoreApplication>

// Translates a string, using a different string as a fallback rather than the
// key. This is useful for debug messages which tend to be long and complex, but
// should also be fail-safe with at least one human-readable version specified
// in source code.

inline QString fallback_translate(const char* context, const char* key, const QString fallback)
{
	QString translation = QCoreApplication::translate(context, key);
	if (translation == QString(key))
		return fallback;
	else
		return translation;
}

#endif // qtextensions__TRANSLATION_H