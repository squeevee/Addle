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

template<typename T>
inline QList<T> arrayToQList(const T arr[], std::size_t length)
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
inline QList<T> arrayToQList(const T (&arr)[N])
{
    return arrayToQList(arr, N);
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

#endif // qtextensions__QLIST_HPP