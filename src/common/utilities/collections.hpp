/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

//TODO: rename to ranges.hpp or range.hpp or something
//TODO: use Boost Ranges (in every place where cpplinq was)

// I lowkey intend to move these range utilities into their own library. I liked
// some of the goodies that came with cpplinq (not present in Boost or STL) but
// the implementation was pretty poor. It seems to me there's a place for an
// assortment of compatibility and convenience utilities extending the Boost/STL
// style API.

#ifndef COLLECTIONS_HPP
#define COLLECTIONS_HPP

#include <initializer_list>
#include <type_traits>
#include <tuple>

#include <list>

#include <QList>
#include <QHash>
#include <QSharedPointer>

#include <boost/range.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/indirect_iterator.hpp>

#include <boost/type_traits.hpp>
#include <boost/type_traits/is_detected_convertible.hpp>

// #define CPPLINQ_NOEXCEPT noexcept
// #include <cpplinq/cpplinq.hpp>

#include "compat.hpp"
#include "hashfunctions.hpp"

// namespace cpplinq {
// 
// namespace detail {
// 
// template<typename range_t>
// struct _noexcept_front : std::integral_constant<bool, noexcept(std::declval<range_t>().front())> {};
//     
// template<typename range_t>
// struct _noexcept_next : std::integral_constant<bool, noexcept(std::declval<range_t>().next())> {};
// 
// template<typename caster_t, typename source_t>
// struct _noexcept_caster : std::integral_constant<bool, noexcept(
//     std::declval<caster_t>()(std::declval<source_t>())
// )> {};
// /*
// template<typename dest_t>
// struct static_cast_functor
// {
//     static_assert(
//         !std::is_reference<dest_t>::value
//         || std::is_const<typename std::remove_reference<dest_t>::type>::value,
//         "cannot cast to non-const reference type"
//     );
//         
//     template<typename source_t>
//     inline dest_t operator()(const source_t& s) const noexcept { return static_cast<dest_t>(s); }
// };
// 
// template<typename dest_t>
// struct dynamic_cast_functor
// {
//     static_assert(
//         !std::is_reference<dest_t>::value
//         || std::is_const<typename std::remove_reference<dest_t>::type>::value,
//         "cannot cast to non-const reference type"
//     );
//         
//     template<typename source_t>
//     inline dest_t operator()(const source_t& s) const { return dynamic_cast<dest_t>(s); }
// };
// 
// struct const_ref_functor
// {
//     template<typename source_t>
//     inline const source_t& operator()(const source_t& s) const { return s; }
// };
// 
// struct add_ref_functor
// {
//     template<typename source_t>
//     inline source_t& operator()(source_t& s) const { return s; }
//     
//     template<typename source_t>
//     inline const source_t& operator()(const source_t& s) const { return s; }
// };
// 
// struct remove_ref_functor
// {
//     template<typename source_t>
//     inline source_t operator()(const source_t& s) const { return s; }
// };
// 
// struct deref_functor
// {
//     template<typename pointer_t>
//     inline auto operator()(const pointer_t& p) const
//         -> std::reference_wrapper<std::remove_reference<decltype(*p)>>
//     {
//         return ref(*p);
//     }
// };
// 
// struct address_functor
// {
//     template<typename source_t>
//     inline source_t* operator()(const source_t& p) const
//     {
//         return &p;
//     }
// };
// 
// template<typename TValue>
// class opt_ref
// {
//     typedef TValue value_type;
// 
//     inline opt_ref() noexcept
//         : _pointer(nullptr)
//     {
//     }
// 
//     inline explicit opt_ref(value_type& value) noexcept
//         :   _pointer(std::addressof(value))
//     {
//     }
// 
//     inline opt_ref(const opt_ref&) = default;
//     inline opt_ref(opt_ref&& other) noexcept
//         :   _pointer(other._pointer)
//     {
//         other._pointer = nullptr;
//     }
// 
//     inline void swap(opt_ref& other) noexcept
//     { 
//         std::swap(_pointer, other._pointer);
//     }
// 
//     inline opt_ref& operator= (const opt_ref& other) noexcept
//     {
//         _pointer = other._pointer;
//         return *this;
//     }
// 
//     inline opt_ref& operator= (opt_ref&& other) noexcept
//     {
//         _pointer = other._pointer;
//         other._pointer = nullptr;
//         return *this;
//     }
// 
//     inline opt_ref & operator= (value_type& value) noexcept
//     {
//         _pointer = std::addressof(value);
//         return *this;
//     }
// 
//     inline void clear() noexcept
//     {
//         _pointer = nullptr;
//     }
// 
//     inline value_type* get_ptr() const noexcept
//     {
//         return _pointer;
//     }
// 
//     inline value_type & get() const noexcept
//     {
//         assert(_pointer);
//         return *_pointer;
//     }
// 
//     inline bool has_value () const noexcept
//     {
//         return static_cast<bool>(_pointer);
//     }
// 
//     explicit operator bool() const noexcept
//     {
//         return has_value();
//     }
// 
//     inline value_type& operator* () const noexcept
//     {
//         return get();
//     }
// 
//     inline value_type* operator-> () const noexcept
//     {
//         return get_ptr();
//     }
// 
// private:
//     value_type* _pointer;
//     bool _is_initialized;
// };
// */
// 
// 
// template<typename T>
// using _has_isNull_t = decltype(std::declval<T>().isNull());
// 
// struct not_null_predicate
// {
//     template<typename T>
//     inline bool operator()(const T& value) const 
//     {
//         return !is_null_impl(value);
//     }
//     
// private:
//     template<typename T_>
//     static inline typename std::enable_if<
//         boost::is_detected_convertible<bool, _has_isNull_t, T_>::value,
//         bool
//     >::type is_null_impl(const T_& value)
//     {
//         return value.isNull();
//     }
//     
//     template<typename T_>
//     static inline typename std::enable_if<
//         !boost::is_detected_convertible<bool, _has_isNull_t, T_>::value
//         && std::is_convertible<T_, bool>::value,
//         bool
//     >::type is_null_impl(const T_& value)
//     {
//         return !static_cast<bool>(value);
//     }
// };
// 
// struct _placeholder_1;
// struct _placeholder_2;
// 
// template<typename qt_sequence_t, typename T>
// struct _qt_sequence_helper;
// 
// template<typename T>
// struct _qt_sequence_helper<QList<_placeholder_1>, T>
// {
//     typedef QList<T> type;
//     static inline void add(type& seq, const T& t) noexcept { seq.append(t); }
// };
// 
// template<typename T>
// struct _qt_sequence_helper<QVector<_placeholder_1>, T>
// {
//     typedef QVector<T> type;
//     static inline void add(type& seq, const T& t) noexcept { seq.append(t); }
// };
// 
// template<typename T>
// struct _qt_sequence_helper<QSet<_placeholder_1>, T>
// {
//     typedef QSet<T> type;
//     static inline void add(type& seq, const T& t) noexcept { seq.insert(t); }
// };
// 
// template<typename qt_sequence_t>
// struct to_qt_sequence_builder : base_builder
// {
//     typedef to_qt_sequence_builder this_type;
//     
//     int reserve;
//     
//     to_qt_sequence_builder(int reserve_)
//         : reserve(reserve_)
//     {
//     }
//     
//     to_qt_sequence_builder(const to_qt_sequence_builder&) = default;
//     to_qt_sequence_builder(to_qt_sequence_builder&&) = default;
//     
//     template <typename range_t>
//     inline typename _qt_sequence_helper<
//         qt_sequence_t, typename range_t::value_type
//     >::type build(range_t range) const 
//         noexcept(_noexcept_front<range_t>::value && _noexcept_next<range_t>::value)
//     {
//         auto result = typename _qt_sequence_helper<
//             qt_sequence_t, typename range_t::value_type
//         >::type();
//         if (reserve > 0)
//         {
//             result.reserve(reserve);
//         }
//         
//         while(range.next())
//         {
//             _qt_sequence_helper<
//                 qt_sequence_t, typename range_t::value_type
//             >::add(result, range.front());
//         }
//         
//         return result;
//     }
// };
// /*
// template<typename TRange>
// inline experimental::container_iterator<typename cleanup_type<TRange>::type> begin(TRange&& range)
// {
//     return experimental::container_iterator<typename cleanup_type<TRange>::type>(std::move(range));
// }
// 
// template<typename TRange>
// inline experimental::container_iterator<typename cleanup_type<TRange>::type> end(const TRange&)
// {
//     return experimental::container_iterator<typename cleanup_type<TRange>::type>();
// }*/
// 
// } // namespace detail
// /*
// template<typename dest_t>
// inline detail::select_builder<detail::static_cast_functor<dest_t>> cast() noexcept
// {
//     return select(detail::static_cast_functor<dest_t>());
// }
// 
// template<typename dest_t>
// inline detail::select_builder<detail::dynamic_cast_functor<dest_t>> dynamic_cast_() noexcept
// {
//     return select(detail::dynamic_cast_functor<dest_t>());
// }
// 
// inline detail::select_builder<detail::const_ref_functor> const_ref() noexcept
// {
//     return select(detail::const_ref_functor());
// }
// 
// inline detail::select_builder<detail::add_ref_functor> add_ref() noexcept
// {
//     return select(detail::add_ref_functor());
// }
// 
// inline detail::select_builder<detail::remove_ref_functor> remove_ref() noexcept
// {
//     return select(detail::remove_ref_functor());
// }
// 
// inline detail::select_builder<detail::deref_functor> deref() noexcept
// {
//     return select(detail::deref_functor());
// }
// 
// inline detail::select_builder<detail::address_functor> address() noexcept
// {
//     return select(detail::address_functor());
// }
//     */
// inline detail::where_builder<detail::not_null_predicate> where_not_null() noexcept
// {
//     return where(detail::not_null_predicate());
// }
//     
// inline detail::to_qt_sequence_builder<QList<detail::_placeholder_1>>
//     to_QList(int reserve = 0) noexcept
// {
//     return detail::to_qt_sequence_builder<QList<detail::_placeholder_1>>(reserve);
// }
// 
// inline detail::to_qt_sequence_builder<QVector<detail::_placeholder_1>>
//     to_QVector(int reserve = 0) noexcept
// {
//     return detail::to_qt_sequence_builder<QVector<detail::_placeholder_1>>(reserve);
// }
// 
// inline detail::to_qt_sequence_builder<QSet<detail::_placeholder_1>>
//     to_QSet(int reserve = 0) noexcept
// {
//     return detail::to_qt_sequence_builder<QSet<detail::_placeholder_1>>(reserve);
// }
// 
// } // namespace cpplinq

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

namespace aux_range_utils
{
//     template<typename T, std::size_t N>
//     class lvalue_refs_range
//     {
//         using arr_t = std::array<T*, N>;
//     public:
//         using iterator = boost::indirect_iterator<typename arr_t::iterator>;
//         using const_iterator = boost::indirect_iterator<typename arr_t::const_iterator>;
//         
//         inline lvalue_refs_range(std::initializer_list<T&> init)
//             : _arr(
//                   init 
//                 | boost::adaptors::transformed( [](T& v) -> T* {
//                     return std::addressof(v);
//                 }) 
//             )
//         {
//         }
//         
//         inline iterator begin() { return iterator(_arr.begin()); }
//         inline const_iterator begin() const { return iterator(_arr.begin()); }
//         
//         inline iterator end() { return iterator(_arr.end()); }
//         inline const_iterator end() const { return iterator(_arr.end()); }
//         
//     private:
//          arr_t _arr;
//     };

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
        using arr_t = std::array<std::decay_t<T>*, N>; 
        
    public:
        using iterator = forwarding_indirect_iterator<T, typename arr_t::iterator>;
        using const_iterator = forwarding_indirect_iterator<T, typename arr_t::const_iterator>;
        
        forwarding_ref_range(const forwarding_ref_range&) = default;
        forwarding_ref_range(forwarding_ref_range&&) = default;
        
        template<typename... U>
        inline forwarding_ref_range(U&&... v)
            : _arr({ static_cast<std::decay_t<T>*>(std::addressof(v))... })
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
            boost::mp11::mp_all<std::is_lvalue_reference<T>...>,
                _copy_max_underlying_cv<_gentler_common_t<T...>, T...>&,
                
            boost::mp11::mp_all<std::is_rvalue_reference<T>...>,
                _copy_max_underlying_cv<_gentler_common_t<T...>, T...>&&,
                
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
    return *boost::begin(std::forward<Range>(r));
}

namespace aux_range_utils
{
    template<typename Range>
    struct getting_size_is_cheap : std::is_convertible<
            typename boost::range_category<Range>::type,
            std::random_access_iterator_tag
        > {};
        
    // Specializations for container types that don't have random access
    // iterators
    
    template<typename T, typename A>
    struct getting_size_is_cheap<std::list<T, A>> : std::true_type {};
    
    // std::forward_list, std::set, std::map, std::multiset, std::multimap 
        
    template<typename T>
    struct getting_size_is_cheap<QSet<T>> : std::true_type {};
    
    template<typename K, typename V>
    struct getting_size_is_cheap<QHash<K, V>> : std::true_type {};
    
    template<typename K, typename V>
    struct getting_size_is_cheap<QMultiHash<K, V>> : std::true_type {};
    
    template<typename K, typename V>
    struct getting_size_is_cheap<QMap<K, V>> : std::true_type {};
    
    template<typename K, typename V>
    struct getting_size_is_cheap<QMultiMap<K, V>> : std::true_type {};
    
}

// Given a container object `t` and a range `s`, this function will reserve
// enough additional space in `t` for its current contents and as many elements
// as are in `s` *iff* acquiring the size of `s` is "cheap" [i.e., determined at
// compile time to support amortized faster than O(log N) where N is the size of 
// `s`].
// 
// The motivating case for this function is if the contents of `s` will be
// iterated linearly and (worst case) for each element in `s` one corresponding 
// element will be pushed/inserted into `t`. Without reserving, most array-based 
// containers will require O(log(N)) reallocations where N is the size of `s`. 
// If the size of `s` can be determined cheaper than that (e.g., is constant or 
// amortized constant), it is likely worth the trouble.
//
// Most containers support amortized constant size lookup. Additionally any
// array, and any range built from random access iterators will allow cheap size
// querying.
//
// Assumes that TargetContainer *has* a `reserve` member function that is used
// the same way as std::vector::reserve, and takes for granted that `t.size()`
// is cheap. 
// 
// In particular the following container types are compatible with this function 
// and stand to benefit from its use:
// - std::vector
// - std::unordered_set (and unordered_multiset)
// - std::unordered_map (and unordered_multimap)
// - std::string (and relatives)
// - QList (and QVector)
// - QSet
// - QHash (and QMultiHash)
// - QByteArray and QString
// - QVarLengthArray
//
// And notably, the following container types do not provide a `reserve` member
// function:
// - std::deque
// - QBitArray

template<class TargetContainer, typename SourceRange>
inline void reserve_for_size_if_cheap(TargetContainer& t, const SourceRange& s)
{
    if constexpr (
            aux_range_utils::getting_size_is_cheap<SourceRange>::value
       )
    {
        t.reserve(
                boost::size(s) + t.size()
            );
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
        boost::begin(std::forward<Range>(range)), 
        boost::end(std::forward<Range>(range)) 
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

} // namespace aux_range_utils

} // namespace Addle

#endif // COLLECTIONS_HPP
