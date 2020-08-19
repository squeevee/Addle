#ifndef LOCALIZEDSTRING_HPP
#define LOCALIZEDSTRING_HPP

#include <QMetaType>
#include <QLocale>
#include <QString>
#include <QByteArray>

namespace Addle {

// Represents an internationalized string, containing either a manually defined
// set of translations to a set of locales (e.g., loaded from externally-
// internationalized data), or a TrId for use with QTranslator.
class TranslatedString
{
public:
    TranslatedString() = default;
    TranslatedString(std::initializer_list<std::pair<QLocale, QString>> versions);
    TranslatedString(const char* trId)
        : _trId(trId)
    {
    }

    TranslatedString(const TranslatedString&) = default;

    QString get() const;

    inline operator QString() const { return get(); }

    inline bool isEmpty() const { return _trId.isEmpty() || _versions_byLocaleName.isEmpty(); }

    inline bool operator==(const TranslatedString& x) const
    { 
        return _trId == x._trId
            && _versions_byLocaleName == x._versions_byLocaleName; 
    }

    inline bool operator!=(const TranslatedString& x) const { return !(*this == x); }
    
private:
    QHash<QString, QString> _versions_byLocaleName;
    QByteArray _trId;

};

} // namespace Addle

Q_DECLARE_METATYPE(Addle::TranslatedString);

#endif // LOCALIZEDSTRING_HPP