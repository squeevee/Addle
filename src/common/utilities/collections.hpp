/**
 * @file
 * @brief Various general-purpose utilities related to collections.
 * Notable contents include:
 * 
 * - `noDetach`, a wrapper that prevent implicitly shared types from detaching
 * when used on the range expression of a for loop. Unlike `qAsConst` this accepts
 * rvalues as well as lvalues.
 * 
 * - Various conversion functions between generic collection types. Qt has
 * deprecated their implementations of these, but I like sugar so here they are.
 * 
 * - `qListCast` and `qSharedPointerListCast` which typecast the contents of a
 * `QList`.
 * 
 * As mentioned in TODO, Addle will likely use a library like cpplinq or
 * Boost.Ranges to perform high-level list manipulations. This file will likely
 * contain convenience extensions to said library. `qListCast` will either be
 * reimplemented to take advantage of / work  in harmony with the library, or
 * deprecated in favor of directly using the library instead.
 */

#ifndef collections_hpp
#define collections_hpp

#include <initializer_list>
#include <type_traits>

#include <QList>
#include <QHash>
#include <QSharedPointer>

#include "compat.hpp"
#include "hashfunctions.hpp"
namespace Addle {

template<typename T>
inline const T& noDetach(T& container)
{
    return qAsConst(container);
}

template<typename T>
inline const T noDetach(T&& container)
{
    // Assuming T is an implicitly shared type, the "copy" made here is an
    // inexpensive shared reference. In the calling context it will be a const 
    // prvalue (thus non-detaching) whose validitity is assured for its own
    // lifetime.

    // Naturally, it should only be used for implicitly shared types

    return T(std::move(container));
}

template<typename T>
inline QList<T> qToList(const QSet<T>& set)
{
    return QList<T>(set.constBegin(), set.constEnd());
}

template<typename T>
inline QList<T> qToList(const T arr[], std::size_t length)
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
inline QList<T> qToList(const T (&arr)[N])
{
    return qToList(arr, N);
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

template<typename T>
inline QSet<T> qToSet(const QList<T>& list)
{
    return QSet<T>(list.constBegin(), list.constEnd());
}

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

} // namespace Addle
#endif // collections_hpp