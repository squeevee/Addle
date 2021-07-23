/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#pragma once

#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <tuple>

#include <list>

#include <QList>
#include <QHash>
#include <QSharedPointer>

#include <boost/iterator.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/indirect_iterator.hpp>

#include <boost/range.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <boost/type_traits.hpp>
#include <boost/type_traits/is_detected.hpp>
#include <boost/type_traits/is_detected_convertible.hpp>

#include "compat.hpp"
#include "hashfunctions.hpp"

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

// template<typename T>
// inline QList<T> qToList(const QSet<T>& set)
// {
//     return QList<T>(set.constBegin(), set.constEnd());
// }
// 
// template<typename T>
// inline QList<T> qToList(const T arr[], std::size_t length)
// {
//     QList<T> outList;
//     outList.reserve(length);
//     for (int i = 0; i < length; i++)
//     {
//         outList.append(arr[i]);
//     }
// 
//     return outList;
// }
// 
// template<typename T, std::size_t N>
// inline QList<T> qToList(const T (&arr)[N])
// {
//     return qToList(arr, N);
// }

namespace aux_range_utils {

template<typename Range>
using _range_iterator_t = typename boost::range_iterator<boost::remove_cv_ref_t<Range>>::type;

}

template<typename Range, std::enable_if_t<boost::is_detected<aux_range_utils::_range_iterator_t, Range>::value, void*> = nullptr>
QList<std::decay_t<typename boost::range_value<boost::remove_cv_ref_t<Range>>::type>>
qToList(Range&& range)
{
    using list_t = QList<std::decay_t<typename boost::range_value<boost::remove_cv_ref_t<Range>>::type>>;
    return list_t(std::begin(range), std::end(range));
}

template<typename T>
inline QSet<T> qToSet(const QList<T>& list)
{
    return QSet<T>(list.constBegin(), list.constEnd());
}

namespace aux_range_utils
{
    template<typename T, class BaseIterator>
    class forwarding_indirect_iterator 
        : public boost::iterator_adaptor<
                forwarding_indirect_iterator<T, BaseIterator>,
                BaseIterator,
                std::remove_reference_t<T>,
                boost::use_default,
                T&&
            >
    {
        friend class boost::iterator_core_access;
        using adaptor_t = boost::iterator_adaptor<
                forwarding_indirect_iterator<T, BaseIterator>,
                BaseIterator,
                std::remove_reference_t<T>,
                boost::use_default,
                T&&
            >;
    public:
        forwarding_indirect_iterator() = default;
        forwarding_indirect_iterator(const forwarding_indirect_iterator&) = default;
        
        forwarding_indirect_iterator& operator=(const forwarding_indirect_iterator&) = default;
        
        explicit inline forwarding_indirect_iterator(const BaseIterator& base)
            : adaptor_t(base)
        {
        }
        
    protected:
        inline T&& dereference() const { return static_cast<T&&>(**this->base()); }
    };
    
    template<typename T, std::size_t N>
    class forwarding_ref_range
    {
        using ptr_t = boost::copy_cv_t<std::decay_t<T>, std::remove_reference_t<T>>*;
        using arr_t = std::array<ptr_t, N>; 
        
    public:
        using iterator = forwarding_indirect_iterator<T, typename arr_t::iterator>;
        using const_iterator = forwarding_indirect_iterator<T, typename arr_t::const_iterator>;
        
        forwarding_ref_range(const forwarding_ref_range&) = default;
        forwarding_ref_range(forwarding_ref_range&&) = default;
        
        template<typename... U>
        inline forwarding_ref_range(U&&... v)
            : _arr({ static_cast<ptr_t>(std::addressof(v))... })
        {
        }
        
        inline iterator begin() { return iterator(_arr.begin()); }
        inline const_iterator begin() const { return const_iterator(_arr.begin()); }
        
        inline iterator end() { return iterator(_arr.end()); }
        inline const_iterator end() const { return const_iterator(_arr.end()); }
        
    private:
         arr_t _arr;
    };
    
    
    class mixed_category_error_tag {};
    
    
    template<typename T, typename... U>
    using _copy_max_underlying_const = boost::mp11::mp_if<
            boost::mp11::mp_any< std::is_const<std::remove_reference_t<U>>... >,
            const std::remove_const_t<T>,
            std::remove_const_t<T>
        >;
    
    template<typename T, typename... U>
    using _copy_max_underlying_volatile = boost::mp11::mp_if<
            boost::mp11::mp_any< std::is_volatile<std::remove_reference_t<U>>... >,
            volatile std::remove_volatile_t<T>,
            std::remove_volatile_t<T>
        >;
    
    template<typename T, typename... U>
    using _copy_max_underlying_cv = _copy_max_underlying_const< 
            _copy_max_underlying_volatile< T, U... >,
            U...
        >;
    
    template<typename... T>
    using _gentler_common_t = boost::mp11::mp_if<
            boost::mp11::mp_same<T...>,
            boost::mp11::mp_first<boost::mp11::mp_list<T...>>,
            std::common_type_t<T...>
        >;
    
    template<typename... T>
    using common_forward_t = boost::mp11::mp_cond<
            boost::mp11::mp_and<
                boost::mp11::mp_not<boost::mp11::mp_all<std::is_class<boost::remove_cv_ref_t<T>>...>>,
                boost::mp11::mp_not<boost::mp11::mp_same<boost::remove_cv_ref_t<T>...>>
            >,
                std::common_type_t<T...>,

            boost::mp11::mp_all<std::is_lvalue_reference<T>...>,
                _copy_max_underlying_cv<_gentler_common_t<boost::remove_cv_ref_t<T>...>, T...>&,
                
            boost::mp11::mp_all<std::is_rvalue_reference<T>...>,
                _copy_max_underlying_cv<_gentler_common_t<boost::remove_cv_ref_t<T>...>, T...>&&,
                
            boost::mp11::mp_true,
                mixed_category_error_tag
        >;
}

// An inline range is a construct meant to enable iteration and other range-
// based tools on one or more existing values. An inline range can be
// constructed and accessed with very little overhead (they are essentially
// wrappers for fixed-size arrays allocated on the stack). 

// Makes an inline range of values of the given arguments. Arguments passed as
// l-value will be copied and arguments passed as r-value will be moved.
template<typename... T>
auto make_inline_range(T&&... args)
{
    // how to handle reference wrappers?
    return std::array<std::common_type_t<T...>, sizeof...(args)>({ std::forward<T>(args)... });
}

// Makes an inline range of forwarding references to the given arguments.
// CAUTION as with many situations involving references, be mindful not to
// access the range after the referent objects have been destroyed.
template<typename... T>
auto make_inline_ref_range(T&&... args)
{
    using common_forward_t = aux_range_utils::common_forward_t<T&&...>;
    static_assert(
        !std::is_same<common_forward_t, aux_range_utils::mixed_category_error_tag>::value,
        "An inline reference range cannot be made from a mix of l-value and "
        "r-value references."
    );
    
    return aux_range_utils::forwarding_ref_range<common_forward_t, sizeof...(T)>( std::forward<T>(args)... );
}

template<typename Range>
auto&& only_value(Range&& r)
{
    assert (boost::size(std::forward<Range>(r)) == 1); // exception?
    return *std::begin(std::forward<Range>(r));
}

template<class TargetContainer, typename SourceRange>
inline void reserve_for_size_if_forward_range(TargetContainer& t, const SourceRange& s)
{
    if constexpr (
            std::is_convertible_v<
                typename boost::range_category<SourceRange>::type, 
                std::forward_iterator_tag
            >
        )
    {
        t.reserve(boost::size(s) + t.size());
    }
}

template<class Range>
inline QList<
        typename boost::range_value<boost::remove_cv_ref_t<Range>>::type
    > toQList(Range&& range)
{
    return QList<
        typename boost::range_value<boost::remove_cv_ref_t<Range>>::type
    >( 
        std::begin(std::forward<Range>(range)), 
        std::end(std::forward<Range>(range)) 
    );
}

namespace aux_range_utils {

template<typename Range>
struct has_contiguous_storage : std::false_type {};

template<typename T, std::size_t N>
struct has_contiguous_storage<T[N]> : std::true_type {};

template<typename T>
struct has_contiguous_storage<T*> : std::true_type {};

template<typename T, std::size_t N>
struct has_contiguous_storage<std::array<T, N>> : std::true_type {};

template<typename T, typename A>
struct has_contiguous_storage<std::vector<T, A>> : std::true_type {};

template<typename T>
struct has_contiguous_storage<QVector<T>> : std::true_type {};

template<typename T, std::size_t N>
struct has_contiguous_storage<QVarLengthArray<T, N>> : std::true_type {};

template<typename Range, std::enable_if_t<std::is_class<boost::remove_cv_ref_t<Range>>::value, void*> = nullptr>
inline typename boost::range_pointer<boost::remove_cv_ref_t<Range>>::type 
    get_contiguous_storage(Range&& range)
{
    return range.data();
}

template<typename T>
inline T* get_contiguous_storage(T* ptr) 
{
    return ptr;
}

template<typename, typename U>
struct copy_extents_impl
{
    using type = U;
};

template<typename T, typename U>
struct copy_extents_impl<T[], U>
{
    using type = U[];
};

template<typename T, std::size_t N, typename U>
struct copy_extents_impl<T[N], U>
{
    using type = boost::mp11::mp_if<
            std::is_array<std::remove_extent_t<T>>,
            U[N],
            typename copy_extents_impl<T, U>::type[N]
        >;
};

template<typename T, typename U>
struct copy_extents
{
    using type = 
        typename copy_extents_impl<
            std::remove_reference_t<T>,
            U
        >::type;
};

template<typename T, typename U>
using copy_extents_t = typename copy_extents<T, U>::type;

// A "range" presenting a value as if it were repeated sequentially some number 
// of times.
template<typename T>
class repetition_range
{
public:
    template<bool IsConst>
    class iterator_impl : public boost::iterator_facade<
            iterator_impl<IsConst>,
            T,
            std::random_access_iterator_tag,
            boost::mp11::mp_if_c<
                IsConst,
                boost::copy_reference_t<
                    const std::remove_reference_t<T>,
                    T&
                >,
                T&
            >
        >
    {
        using ptr_t = boost::mp11::mp_if_c<
                IsConst,
                const boost::remove_cv_ref_t<T>*,
                boost::remove_cv_ref_t<T>*
            >;
        
        using ref_t = boost::mp11::mp_if_c<
                IsConst,
                const boost::remove_cv_ref_t<T>&,
                boost::remove_cv_ref_t<T>&
            >;
            
    public:
        iterator_impl() = default;
        iterator_impl(const iterator_impl&) = default;
        
        template<class MutableIterator,
            std::enable_if_t<
                IsConst 
                && std::is_same_v<MutableIterator, iterator_impl<false>>,
                void*
            > = nullptr
        >
        iterator_impl(const MutableIterator& i)
            : _index(i._index), _valuePtr(i._valuePtr)
        {
        }
        
        std::size_t index() const { return _index; }
        
    private:
        iterator_impl(std::size_t index, ref_t value)
            : _index(index), _valuePtr(std::addressof(value))
        {
        }
        
        ref_t dereference() const
        {
            return *_valuePtr;
        }
        
        template<bool IsConst_>
        bool equal(const iterator_impl<IsConst_>& other) const
        {
            return _index == other._index
                && _valuePtr == other._valuePtr;
        }
        
        void increment() { ++_index; }
        void decrement() { --_index; }
        void advance(std::ptrdiff_t d) { _index += d; }
        
        template<bool IsConst_>
        std::ptrdiff_t distance_to(const iterator_impl<IsConst_>& other) const
        {
            return other._index - _index;
        }
        
        std::size_t _index = 0;
        ptr_t _valuePtr = nullptr;
        
        friend class repetition_range<T>;
        friend class boost::iterator_core_access;
    };
    
    using value_type        = std::remove_reference_t<T>;
    using reference         = T&;
    using iterator          = iterator_impl<
            std::is_const_v<std::remove_reference_t<T>>
        >;
    using const_iterator    = iterator_impl<true>;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    
    template<typename U = T>
    repetition_range(std::size_t size, U&& value = {})
        : _size(size), _value(std::forward<U>(value))
    {
    }
    
    iterator begin() { return iterator(0, _value); }
    const_iterator begin() const { return const_iterator(0, _value); }
    const_iterator cbegin() const { return const_iterator(0, _value); }
    
    iterator end() { return iterator(_size, _value); }
    const_iterator end() const { return const_iterator(_size, _value); }
    const_iterator cend() const { return const_iterator(_size, _value); }
    
    std::size_t size() const { return _size; }
    void setSize(std::size_t size) { _size = size; }
    
    void swap(repetition_range& other)
    {
        std::swap(_size, other._size);
        std::swap(_value, other._value);
    }
    
    friend void swap(repetition_range& lhs, repetition_range& rhs)
    {
        lhs.swap(rhs);
    }
    
private:
    std::size_t _size;
    T _value;
};
} // namespace aux_range_utils

template<typename T>
inline aux_range_utils::repetition_range<std::decay_t<T>> 
    make_repetition_range(std::size_t size, T&& value = {})
{
    return aux_range_utils::repetition_range<std::decay_t<T>> (
            size,
            std::forward<T>(value)
        );
}

template<typename T>
inline aux_range_utils::repetition_range<T&&> 
    make_repetition_ref_range(std::size_t size, T&& value = {})
{
    return aux_range_utils::repetition_range<T&&> (
            size,
            std::forward<T>(value)
        );
}

namespace aux_range_utils {

template<typename... BaseIterators>
class iterator_variant_impl 
    : public boost::iterator_facade<
        iterator_variant_impl<BaseIterators...>,
        std::remove_reference_t<
            common_forward_t<
                typename std::iterator_traits<BaseIterators>::reference...
            >
        >,
        std::common_type_t<
            typename std::iterator_traits<BaseIterators>::iterator_category...
        >,
        common_forward_t<
            typename std::iterator_traits<BaseIterators>::reference...
        >,
        std::common_type_t<
            typename std::iterator_traits<BaseIterators>::difference_type...
        >
    >
{
    using _ref_t = common_forward_t<
            typename std::iterator_traits<BaseIterators>::reference...
        >;
    
    using _category = std::common_type_t<
            typename std::iterator_traits<BaseIterators>::iterator_category...
        >;
        
    using _diff_t = std::common_type_t<
            typename std::iterator_traits<BaseIterators>::difference_type...
        >;
        
    static_assert(!std::is_same_v<_ref_t, mixed_category_error_tag>);
        
public:
    iterator_variant_impl() = default;
    iterator_variant_impl(const iterator_variant_impl&) = default;
    iterator_variant_impl(iterator_variant_impl&&) = default;
    
    iterator_variant_impl& operator=(const iterator_variant_impl&) = default;
    iterator_variant_impl& operator=(iterator_variant_impl&&) = default;
    
    template<typename BaseIterator>
    iterator_variant_impl(BaseIterator&& base)
        : _base(std::forward<BaseIterator>(base))
    {
    }
    
    template<typename OtherIter,
        std::enable_if_t<boost::mp11::mp_any<
            boost::has_equal_to<BaseIterators, OtherIter>...
        >::value, void*> = nullptr>
    bool operator==(const OtherIter& other) const
    {
        return this->equal_hetero(other);
    }
    
    template<typename OtherIter,
        std::enable_if_t<boost::mp11::mp_any<
            boost::has_equal_to<BaseIterators, OtherIter>...
        >::value, void*> = nullptr>
    bool operator!=(const OtherIter& other) const
    {
        return !(this->equal_hetero(other));
    }
        
private:
    _ref_t dereference() const
    {
        return std::visit([] (const auto& base) -> _ref_t { return *base; }, _base );
    }
    
    struct visitor_equal
    {
        template<
            typename T, 
            typename U,
            std::enable_if_t<
                boost::has_equal_to<const T&, const U&>::value,
            void*> = nullptr>
        bool operator()(const T& lhs, const U& rhs) const
        {
            return lhs == rhs;
        }
        
        template<
            typename T, 
            typename U,
            std::enable_if_t<
                !boost::has_equal_to<const T&, const U&>::value,
            void*> = nullptr>
        constexpr bool operator()(const T&, const U&) const
        {
            return false;
        }
    };
    
    bool equal(const iterator_variant_impl& other) const
    {
        return std::visit(visitor_equal {}, _base, other._base);
    }
    
    template<typename OtherIter>
    bool equal_hetero(const OtherIter& other) const
    {
        return std::visit(
                std::bind( visitor_equal {}, std::placeholders::_1, other ),
                _base
            );
    }
    
    void increment()
    {
        std::visit([] (auto& base) { ++base; }, _base);
    }
    
    void decrement()
    {
        if constexpr (std::is_convertible_v<_category, std::bidirectional_iterator_tag>)
            std::visit([] (auto& base) { --base; }, _base);
        else
            Q_UNREACHABLE();
    }
    
    void advance(_diff_t n)
    {
        if constexpr (std::is_convertible_v<_category, std::random_access_iterator_tag>)
            std::visit([n] (auto& base) { std::advance(base, n); }, _base);
        else
            Q_UNREACHABLE();
    }
        
    struct visitor_distance
    {
        template<typename T, typename U>
        using _distance_t = decltype( std::distance( std::declval<T>(), std::declval<U>() ) );
        
        template<
            typename T, 
            typename U,
            std::enable_if_t<
                boost::mp11::mp_valid<_distance_t, const T&, const U&>::value,
            void*> = nullptr>
        _diff_t operator()(const T& lhs, const U& rhs) const
        {
            return std::distance( lhs, rhs);
        }
        
        template<
            typename T, 
            typename U,
            std::enable_if_t<
                !boost::mp11::mp_valid<_distance_t, const T&, const U&>::value,
            void*> = nullptr>
        constexpr _diff_t operator()(const T&, const U&) const
        {
            return 0;
        }
    };
    
    _diff_t distance_to( const iterator_variant_impl& other) const
    {
        return std::visit( visitor_distance {}, _base, other._base );
    }
    
    std::variant<BaseIterators...> _base;
    
    friend class boost::iterator_core_access;
};

template<typename... BaseIterators>
using iterator_variant = boost::mp11::mp_if<
        boost::mp11::mp_same<BaseIterators...>,
        boost::mp11::mp_first<boost::mp11::mp_list<BaseIterators...>>,
        boost::mp11::mp_apply<
            iterator_variant_impl,
            boost::mp11::mp_unique<boost::mp11::mp_list<BaseIterators...>>
        >
    >;

} // namespace aux_range_utils
} // namespace Addle
