#include "translation.hpp"

QString fallback_translate(const char* context, const char* key, const QString fallback)
{
    QString translation = QCoreApplication::translate(context, key);
    if (translation == QString(key))
        return fallback;
    else
        return translation;
}