#ifndef qtextensions__TRANSLATION_H
#define qtextensions__TRANSLATION_H

#include "compat.hpp"
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

inline QString template_translate(const char* context, const char* keyTemplate, QStringList args)
{
    QString key = keyTemplate;
    for (QString arg : args)
    {
        key = key.arg(arg);
    }
    return QCoreApplication::translate(context, qUtf8Printable(key));
}

inline QString template_translate(const char* context, const char* keyTemplate, QString arg)
{
    return template_translate(context, keyTemplate, QStringList({arg}));
}

#endif // qtextensions__TRANSLATION_H