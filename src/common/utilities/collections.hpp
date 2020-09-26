/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 *
 * @brief Various general-purpose utilities related to collections.
 * 
 * Notable contents include:
 * 
 * - cpplinq is included by this file, and some extensions are provided:
 *   - to_QList, etc allow cpplinq ranges to be converted into Qt containers.
 *   - cast operator statically casts members of a range to a new type.
 *   - cpplinq ranges can be used as the range expression of a C++11 for-loop.
 * 
 * - `noDetach`, a wrapper that prevent implicitly shared types from detaching
 * when used on the range expression of a for loop.
 * 
 * - Various conversion functions between generic collection types. Qt has
 * deprecated their implementations of these, but I like sugar so here they are.
 * 
 * @note cpplinq acquires ranges from const l-value references to containers, so
 * no special action is needed to avoid detaching Qt containers when calling
 * cpplinq::from() on one.
 * 
 * @todo Another possible extension to cpplinq would be a container-like class
 * for use as a return type to mimic yield-return semantics.
 * - The only type parameter of this yield class would be the value type of the
 *   range it represents.
 * - This will probably require virtualization so it will not be as efficient as
 *   an inline cpplinq range -- but should be more efficient than saving off to
 *   an unneeded strong container type and immediately disposing it.
 * - The yield could be iterated STL-style or used as a range in another linq 
 *   expression. In either case, the values are only resolved when dereferenced.
 */

#ifndef COLLECTIONS_HPP
#define COLLECTIONS_HPP

#include <initializer_list>
#include <type_traits>

#include <QList>
#include <QHash>
#include <QSharedPointer>

#define CPPLINQ_NOEXCEPT noexcept
#include <cpplinq/cpplinq.hpp>

#include "compat.hpp"
#include "hashfunctions.hpp"

namespace cpplinq {

namespace detail {

template<typename range_t>
struct _noexcept_front : std::integral_constant<bool, noexcept(std::declval<range_t>().front())> {};
    
template<typename range_t>
struct _noexcept_next : std::integral_constant<bool, noexcept(std::declval<range_t>().next())> {};

template<typename caster_t, typename source_t>
struct _noexcept_caster : std::integral_constant<bool, noexcept(
    std::declval<caster_t>()(std::declval<source_t>())
)> {};

template<typename dest_t>
struct static_caster
{
    template<typename source_t>
    inline dest_t operator()(const source_t& s) const noexcept { return static_cast<dest_t>(s); }
};

template<typename dest_t>
struct dynamic_caster
{
    template<typename source_t>
    inline dest_t operator()(const source_t& s) const { return dynamic_cast<dest_t>(s); }
};
  
template<typename range_t, typename dest_t, typename caster_t = static_caster<dest_t>>
struct cast_range : base_range
{
    static_assert(
        !std::is_reference<dest_t>::value || std::is_const<dest_t>::value,
        "cannot cast to non-const reference type"
    );
    
    typedef cast_range this_type;
    typedef dest_t value_type;
    typedef dest_t return_type;

    static constexpr bool returns_reference = std::is_reference<dest_t>::value;
    
    typedef range_t range_type;
    typedef typename range_type::return_type source_t;
    
    range_type          range;
    
    inline cast_range(range_type range_)
        : range(range_)
    {
    }
    
    inline cast_range(const cast_range&) noexcept = default;
    inline cast_range(cast_range&&) noexcept = default;
    
    inline return_type front() const noexcept(
            _noexcept_front<range_t>::value &&
            _noexcept_caster<caster_t, source_t>::value
        )
    {
        return caster_t()(range.front());
    }
    
    inline bool next() noexcept(
            _noexcept_next<range_t>::value
        )
    {
        return range.next();
    }
    
    template<typename range_tBuilder>
    inline typename get_builtup_type<range_tBuilder, this_type>::type
        operator>>(range_tBuilder range_builder) noexcept
    {
        return range_builder.build(*this);
    }
};

template<typename dest_t, typename caster_t = static_caster<dest_t>>
struct cast_builder
{
    typedef cast_builder    this_type;
    
    template<typename range_t>
    inline cast_range<range_t, dest_t, caster_t> build(range_t range) const noexcept
    {
        return cast_range<range_t, dest_t, caster_t>(range);
    }
};

struct _placeholder_1;
struct _placeholder_2;

template<typename qt_sequence_t, typename T>
struct _qt_sequence_helper;

template<typename T>
struct _qt_sequence_helper<QList<_placeholder_1>, T>
{
    typedef QList<T> type;
    static inline void add(type& seq, const T& t) noexcept { seq.append(t); }
};

template<typename T>
struct _qt_sequence_helper<QVector<_placeholder_1>, T>
{
    typedef QVector<T> type;
    static inline void add(type& seq, const T& t) noexcept { seq.append(t); }
};

template<typename T>
struct _qt_sequence_helper<QSet<_placeholder_1>, T>
{
    typedef QSet<T> type;
    static inline void add(type& seq, const T& t) noexcept { seq.insert(t); }
};

template<typename qt_sequence_t>
struct to_qt_sequence_builder : base_builder
{
    typedef to_qt_sequence_builder this_type;
    
    int reserve;
    
    to_qt_sequence_builder(int reserve_)
        : reserve(reserve_)
    {
    }
    
    to_qt_sequence_builder(const to_qt_sequence_builder&) = default;
    to_qt_sequence_builder(to_qt_sequence_builder&&) = default;
    
    template <typename range_t>
    inline typename _qt_sequence_helper<
        qt_sequence_t, typename range_t::value_type
    >::type build(range_t range) const 
        noexcept(_noexcept_front<range_t>::value && _noexcept_next<range_t>::value)
    {
        auto result = typename _qt_sequence_helper<
            qt_sequence_t, typename range_t::value_type
        >::type();
        if (reserve > 0)
        {
            result.reserve(reserve);
        }
        
        while(range.next())
        {
            _qt_sequence_helper<
                qt_sequence_t, typename range_t::value_type
            >::add(result, range.front());
        }
        
        return result;
    }
};

template<typename TRange>
inline experimental::container_iterator<typename cleanup_type<TRange>::type> begin(TRange&& range)
{
    return experimental::container_iterator<typename cleanup_type<TRange>::type>(std::move(range));
}

template<typename TRange>
inline experimental::container_iterator<typename cleanup_type<TRange>::type> end(TRange&&)
{
    return experimental::container_iterator<typename cleanup_type<TRange>::type>();
}

} // namespace detail

template<typename dest_t>
inline detail::cast_builder<dest_t> cast() noexcept
{
    return detail::cast_builder<dest_t>();
}

template<typename dest_t>
inline detail::cast_builder<dest_t> dynamicCast() noexcept
{
    return detail::cast_builder<dest_t, detail::dynamic_caster<dest_t>>();
}
    
inline detail::to_qt_sequence_builder<QList<detail::_placeholder_1>>
    to_QList(int reserve = 0) noexcept
{
    return detail::to_qt_sequence_builder<QList<detail::_placeholder_1>>(reserve);
}

inline detail::to_qt_sequence_builder<QVector<detail::_placeholder_1>>
    to_QVector(int reserve = 0) noexcept
{
    return detail::to_qt_sequence_builder<QVector<detail::_placeholder_1>>(reserve);
}

inline detail::to_qt_sequence_builder<QSet<detail::_placeholder_1>>
    to_QSet(int reserve = 0) noexcept
{
    return detail::to_qt_sequence_builder<QSet<detail::_placeholder_1>>(reserve);
}

} // namespace cpplinq

namespace Addle {

/**
 * Prevent implicitly shared data type container from detaching.
 */
template<typename T>
inline const T& noDetach(T& container)
{
    return qAsConst(container);
}

/**
 * Prevent implicitly shared data type container from detaching.
 */
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

template<typename T>
inline QSet<T> qToSet(const QList<T>& list)
{
    return QSet<T>(list.constBegin(), list.constEnd());
}

} // namespace Addle

#endif // COLLECTIONS_HPP
