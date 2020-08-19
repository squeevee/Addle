#include "translatedstring.hpp"

#include <QtGlobal>

using namespace Addle;

TranslatedString::TranslatedString(std::initializer_list<std::pair<QLocale, QString>> versions)
{
    for (auto& pair : versions)
    {
        for (QString name : pair.first.uiLanguages())
        {
            _versions_byLocaleName.insert(name, pair.second);
        }
    }
}

QString TranslatedString::get() const
{
    if (!_trId.isEmpty())
    {
        return qtTrId(_trId.constData());
    }
    else if (!_versions_byLocaleName.isEmpty())
    {
        QLocale defaultLocale;
        for (QString name : defaultLocale.uiLanguages())
        {
            if (_versions_byLocaleName.contains(name))
            {
                return _versions_byLocaleName.value(name);
            }
        }
    }

    return QString();
}