#ifndef COLLATINGSTRING_HPP
#define COLLATINGSTRING_HPP

#include "compat.hpp"

#include <QString>
#include <QCollator>

namespace Addle {
    
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
        : _collator(QLocale()), _string(string)
    {
    }
    
    inline CollatingString(QString string, const QLocale& locale)
        : _collator(locale), _string(string)
    {   
    }
    
    inline CollatingString(QString string, const QCollator& collator)
        : _collator(collator), _string(string)
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

#endif // COLLATINGSTRING_HPP
