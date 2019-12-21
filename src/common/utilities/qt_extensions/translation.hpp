#ifndef QT_EXTENSIONS__TRANSLATION_H
#define QT_EXTENSIONS__TRANSLATION_H

#include <QString>
#include <QCoreApplication>

// Translates a string, using a different string as a fallback rather than the
// key. This is useful for debug messages which tend to be long and complex, but
// should also be fail-safe with at least one human-readable version specified
// in source code.

QString fallback_translate(const char* context, const char* key, const QString fallback);

#endif // QT_EXTENSIONS__TRANSLATION_H