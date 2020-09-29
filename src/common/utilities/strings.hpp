/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef STRINGS_HPP
#define STRINGS_HPP

#include "compat.hpp"

#include <QSet>
#include <QString>
#include <QCollator>
#include <QByteArray>
#include <QByteArrayList>
#include <QMetaEnum>

#include "globals.hpp"
#include "dynamic_qttrid.hpp"

namespace Addle
{

inline QString affixUnits(LayoutUnits unit, double a,
    int fieldwidth = 0, char format = 'g', int precision = -1,
    QChar fillChar = QLatin1Char(' '))
{
                    //% "%L1 %2"
    return qtTrId("templates.affix-units")
        .arg(a, fieldwidth, format, precision, fillChar)
        .arg(dynamic_qtTrId({ "units", QMetaEnum::fromType<LayoutUnits>().valueToKey(unit) }));
}

inline QString affixUnits(LayoutUnits unit, int a, 
    int fieldwidth = 0, int base = 10, QChar fillChar = QLatin1Char(' '))
{
    return qtTrId("templates.affix-units")
        .arg(a, fieldwidth, base, fillChar)
        .arg(dynamic_qtTrId({ "units", QMetaEnum::fromType<LayoutUnits>().valueToKey(unit) }, a));
}

/**
 * @class
 * Adapter for QString whose regular compare operators use locale-aware
 * ordering, for use with e.g., QMap or cpplinq::orderby where an explicit
 * comparator can't be specified.
 */
class ADDLE_COMMON_EXPORT CollatingString
{
public:
    inline CollatingString(QString string)
        : _string(string), _collator(QLocale())
    {
    }
    
    inline CollatingString(QString string, const QLocale& locale)
        : _string(string), _collator(locale)
    {   
    }
    
    inline CollatingString(QString string, const QCollator& collator)
        : _string(string), _collator(collator)
    {
    }
    
    inline CollatingString(const CollatingString&) = default;
    inline CollatingString(CollatingString&&) = default;
    inline CollatingString& operator=(const CollatingString&) = default;
    inline CollatingString& operator=(CollatingString&&) = default;
    
    inline bool operator<  (const CollatingString& other) const { return compare(*this, other) <  0; }
    inline bool operator<= (const CollatingString& other) const { return compare(*this, other) <= 0; }
    inline bool operator>  (const CollatingString& other) const { return compare(*this, other) >  0; }
    inline bool operator>= (const CollatingString& other) const { return compare(*this, other) >= 0; }
    inline bool operator== (const CollatingString& other) const { return compare(*this, other) == 0; }
    inline bool operator!= (const CollatingString& other) const { return compare(*this, other) != 0; }
    
    inline operator QString() const { return _string; }
    inline QString string() const { return _string; }
    
    inline const QCollator& collator() const { return _collator; }
    
    inline void setCaseSensitivity(Qt::CaseSensitivity sensitivity) { _collator.setCaseSensitivity(sensitivity); }
    inline void setIgnorePunctuation(bool on) { _collator.setIgnorePunctuation(on); }
    inline void setLocale(const QLocale& locale) { _collator.setLocale(locale); }
    inline void setNumericMode(bool on) { _collator.setNumericMode(on); }
    
    static int compare(const CollatingString& a, const CollatingString& b);
private:
    QCollator _collator;
    QString _string;
};

} // namespace Addle

#endif // STRINGS_HPP
