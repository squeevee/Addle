#ifndef qtextensions__QLIST_HPP
#define qtextensions__QLIST_HPP

#include <initializer_list>
#include <type_traits>

#include <QList>
#include <QHash>
#include <QPair>
#include <QSharedPointer>

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

template<typename OutType, typename InType>
inline QList<OutType> qListCast(const QList<InType>& inList)
{
    QList<OutType> outList;
    outList.reserve(inList.size());
    for (InType entry : inList)
    {
        outList.append(static_cast<OutType>(entry));
    }
    return outList;
}

template<typename OutType, typename InType>
inline QList<QSharedPointer<OutType>> qSharedPointerListCast(const QList<QSharedPointer<InType>>& inList)
{
    QList<QSharedPointer<OutType>> outList;
    outList.reserve(inList.size());
    for (QSharedPointer<InType> entry : inList)
    {
        outList.append(entry.template staticCast<OutType>());
    }
    return outList;
}

// TODO: static cast iterator

template<typename T>
inline QList<T> qListFromArray(const T arr[], std::size_t length)
{
    QList<T> outList;
    outList.reserve(length);
    for (int i = 0; i < length; i++)
    {
        outList.append(arr[i]);
    }

    return outList;
}


template<typename T, std::size_t N>
inline QList<T> qListFromArray(const T (&arr)[N]) //qToList
{
    return qListFromArray(arr, N);
}

/*

template<typename OutType, typename InType>
QList<OutType> qListCast(
    const QList<InType>& inList,
    std::function<OutType(InType)> converter =
        [] (InType i) { return static_cast<OutType>(i); }
    )
{
    QList<OutType> outList;
    outList.reserve(inList.size());
    for (const InType& entry : inList)
    {
        outList.append(converter(entry));
    }
    return outList;
}

template<typename OutType, typename InType>
QList<QSharedPointer<OutType>> qSharedPointerListCast(const QList<QSharedPointer<InType>>& inList)
{
    return qListCast<QSharedPointer<OutType>>(
        inList, 
        [] (QSharedPointer<InType> i) {
            return i.template staticCast<OutType>();
        }
    );
}
*/

// I am deeply annoyed that Qt is deprecating QList::toSet() and the like, just
// because range-based constructors exist now. It's not like Qt has heretofore
// been shy about API sugar. So I'm willfully undermining that decision.

template<typename T>
inline QSet<T> qToSet(const QList<T>& list)
{
    return QSet<T>(list.constBegin(), list.constEnd());
}

template<typename T>
inline QList<T> qToList(const QSet<T>& set)
{
    return QList<T>(set.constBegin(), set.constEnd());
}

// A general const-wrapper for use on a range expression in a range-based `for`
// loop, to prevent implicitly shared containers from detaching. With lvalue
// references, this is equivalent to `qAsConst`. With rvalue references, this
// constructs a "copy" and returns it as a const value -- under the assumption
// that such a copy operation is inexpensive and results in an attached
// shared-reference object.
//
// Naturally, this should only be used with implicitly shared types.

template<typename T>
inline const T& noDetach(T& container)
{
    return qAsConst(container);
}

template<typename T>
inline const T noDetach(T&& container)
{
    return T(std::move(container));
}

#endif // qtextensions__QLIST_HPP