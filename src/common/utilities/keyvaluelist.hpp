#ifndef KEYVALUELIST_HPP
#define KEYVALUELIST_HPP

// Note: this is not actually used anywhere.

#include <QList>
#include <QPair>
#include <QHash>
namespace Addle {

template<typename KeyType, typename ValueType>
class QKeyValueList : public QList<QPair<KeyType, ValueType>>
{
public:
    
    QList<KeyType> keys() const
    {
        QList<KeyType> result;
        result.reserve(QList<QPair<KeyType, ValueType>>::size());
        for (const QPair<KeyType, ValueType>& entry : *this)
        {
            result.append(entry.first);
        }
        return result;
    }

    QList<ValueType> values() const
    {
        QList<ValueType> result;
        result.reserve(QList<QPair<KeyType, ValueType>>::size());
        for (const QPair<KeyType, ValueType>& entry : *this)
        {
            result.append(entry.second);
        }
        return result;
    }

    // TODO: key/value iterators? It'd basically be a QList::iterator with some
    // syntactic sugar.

    QHash<KeyType, ValueType> toHash() const
    {
        QHash<KeyType, ValueType> result;
        for (const QPair<KeyType, ValueType>& entry : *this)
        {
            result.insert(entry.first, entry.second);
        }
        return result;
    }

    void append(const KeyType& key, const ValueType& value) 
    {
        QList<QPair<KeyType, ValueType>>::append(QPair<KeyType, ValueType>(key, value));
    }
};

} // namespace Addle
#endif // KEYVALUELIST_HPP