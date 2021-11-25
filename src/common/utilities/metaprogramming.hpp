#pragma once

#include <tuple>
#include <utility>
#include <boost/mp11.hpp>

namespace Addle {
namespace metaprogramming_detail {
    
template<bool does_contain, bool contains_at_last_position, typename L, typename V>
struct mp_contains_once_impl { using type = boost::mp11::mp_false; };

template<typename L, typename V>
struct mp_contains_once_impl<true, true, L, V> { using type = boost::mp11::mp_true; };

template<typename L, typename V>
struct mp_contains_once_impl<true, false, L, V>
{
    using type = boost::mp11::mp_not<
            boost::mp11::mp_contains<
                boost::mp11::mp_drop<
                    L, 
                    boost::mp11::mp_plus< 
                        boost::mp11::mp_find<L, V>, 
                        boost::mp11::mp_int<1>
                    >
                >,
                V
            >
        >;
};

} // namespace metaprogramming_detail

template<typename L, typename V>
using mp_contains_once = typename metaprogramming_detail::mp_contains_once_impl<
        boost::mp11::mp_contains<L, V>::value,
        boost::mp11::mp_find<L, V>::value == boost::mp11::mp_size<L>::value - 1,
        L, 
        V
    >::type;
    
namespace metaprogramming_detail {
    
struct std_tuple_q
{
    template<typename... Element>
    using fn = std::tuple<Element...>;
};

struct auto_array_type_tag {};

template<typename TypeHint = auto_array_type_tag>
struct std_array_q
{
    template<typename... Element>
    struct _impl
    {
        static constexpr std::size_t size = sizeof...(Element);
        using value_type = boost::mp11::mp_if<
                std::is_same<TypeHint, auto_array_type_tag>,
                boost::mp11::mp_first<boost::mp11::mp_list<Element...>>,
                TypeHint
            >;
        
        static_assert(
            boost::mp11::mp_all_of_q<
                boost::mp11::mp_list<Element...>,
                boost::mp11::mp_bind<
                    std::is_convertible, boost::mp11::_1, value_type
                >
            >::value,
            "Not all generated values are convertible to the array's value type."
        );
        
        using type = std::array<value_type, size>;
    };
    
    template<typename... Element>
    using fn = typename _impl<Element...>::type;
};

struct const_array_functor
{
    template<typename Handle>
    constexpr const decltype(Handle::type::value)& operator()(const Handle&) const 
    { 
        return Handle::type::value; 
    }
};
    
} // namespace metaprogramming_detail
    
template <typename L, class Tuple_q = metaprogramming_detail::std_tuple_q>
class tuple_generator
{
    template<typename I>
    using _type_handle_at = boost::mp11::mp_identity<boost::mp11::mp_at<L, I>>;
    
    template<typename I, typename F>
    using _result_1_t = decltype( std::declval<F>()( 
            std::declval<_type_handle_at<I>>()
        ));
    
    template<typename I, typename F>
    using _result_2_t = decltype( std::declval<F>()( 
            std::declval<_type_handle_at<I>>(),
            std::declval<I>()
        ));
    
    template<typename I, typename F>
    using element_t = typename boost::mp11::mp_if<
            boost::mp11::mp_valid<_result_2_t, I, F>,
            boost::mp11::mp_defer<_result_2_t, I, F>,
            boost::mp11::mp_defer<_result_1_t, I, F>
        >::type;
    
    template<typename I, typename F>
    static constexpr typename std::enable_if<
        boost::mp11::mp_valid<_result_2_t, I, F>::value,
        element_t<I, F>
    >::type get_element(const F& f)
    {
        static_assert(!boost::mp11::mp_valid<_result_1_t, I, F>::value, "");
        return f( _type_handle_at<I>(), I() );
    }
    
    template<typename I, typename F>
    static constexpr typename std::enable_if<
        boost::mp11::mp_valid<_result_1_t, I, F>::value
        && !boost::mp11::mp_valid<_result_2_t, I, F>::value,
        element_t<I, F>
    >::type get_element(const F& f)
    {
        return f( _type_handle_at<I>() );
    }
    
    template<typename... I>
    struct impl 
    {
        template<typename F>
        using result_t = typename Tuple_q::template fn<element_t<I, F>...>;
        
        template<typename F>
        static constexpr result_t<F> gen(const F& f)
        {
            return result_t<F> {get_element<I>(f)...};
        }
    };
    
    using iota = boost::mp11::mp_iota<boost::mp11::mp_size<L>>;
    
public:
    template<typename F>
    using result_t = typename boost::mp11::mp_apply<impl, iota>
        ::template result_t<F>;

    template<typename F>
    static constexpr result_t<F> gen(const F& f)
    {
        return boost::mp11::mp_apply<impl, iota>::template gen<F>(f);
    }
};

template<typename L, typename F>
constexpr typename tuple_generator<L>::template result_t<F> generate_tuple_over_list(const F& f)
{
    return tuple_generator<L>::template gen<F>(f);
}

template<
        typename L, 
        typename F,
        typename TypeHint = metaprogramming_detail::auto_array_type_tag
    >
constexpr typename tuple_generator<L, metaprogramming_detail::std_array_q<TypeHint>>
    ::template result_t<F> generate_array_over_list(const F& f)
{
    return tuple_generator<
            L, 
            metaprogramming_detail::std_array_q<TypeHint>
        >::template gen<F>(f);
}

template<
        typename L, 
        typename TypeHint = metaprogramming_detail::auto_array_type_tag
    >
constexpr typename tuple_generator<L, metaprogramming_detail::std_array_q<TypeHint>>
    ::template result_t<metaprogramming_detail::const_array_functor> const_array_from_list()
{
    return generate_array_over_list<
            L, 
            metaprogramming_detail::const_array_functor,
            TypeHint
        >(metaprogramming_detail::const_array_functor());
}

// like mp_inherit, but with a public virtual destructor
template<typename... T>
class mp_polymorphic_inherit : public T...
{
public:
    virtual ~mp_polymorphic_inherit() = default;
};

// like mp_inherit, but with a public virtual destructor and all the base
// classes are virtual
template<typename... T>
class mp_virtual_inherit : public virtual T...
{
public:
    virtual ~mp_virtual_inherit() = default;
};

namespace metaprogramming_detail {
    
template<class... R>
struct mp_build_list_r;

template<class C, class T, class... R>
using mp_build_list = boost::mp11::mp_if<
        C,
        boost::mp11::mp_push_front<typename mp_build_list_r<R...>::type, T>,
        typename mp_build_list_r<R...>::type
    >;

template<class C, class T, class... R>
struct mp_build_list_r<C, T, R...> { using type = mp_build_list<C, T, R...>; };

template<>
struct mp_build_list_r<> { using type = boost::mp11::mp_list<>; };

} // namespace metaprogramming_detail

// Alias for `mp_list<...>` with members built from the given arguments.
// 
// This metafunction follows a similar form to mp_cond where arguments are
// alternating C, T pairs. For every C where static_cast<bool>(C::value) == true
// the corresponding T will be appended to the resulting list.
//
// If no C is true, then the result is mp_list<>
using metaprogramming_detail::mp_build_list;

template<class T>
using mp_undefer = typename T::type;
    
// Unwraps a list L of deferred types then applies the undeferred types to a 
// function F.
template<template<typename...> class F, class L>
using mp_apply_undeferred = boost::mp11::mp_apply<
        F,
        boost::mp11::mp_transform<
            mp_undefer,
            L
        >
    >;
    
// Similar to mp_defer but takes a quoted metafunction
template<class Q, typename... T>
using mp_defer_q = boost::mp11::mp_defer<Q::template fn, T...>;


namespace metaprogramming_detail {
    
template<typename L>
struct mp_only_impl {};

template<template<typename...> class L, typename T>
struct mp_only_impl<L<T>> { using type = T; };

template<template<typename...> class L, typename T, typename... U>
struct mp_only_impl<L<T, U...>> {};
    
}

// If L is a list containing only one element, mp_only<L> is an alias for that
// element, otherwise mp_only<L> is a substitution failure.
template<typename L>
using mp_only = typename metaprogramming_detail::mp_only_impl<L>::type;

namespace metaprogramming_detail {

template<template<class> class P, typename _>
struct _static_predicate_bitmap_impl {};
    
template<template<class> class P, template<class...> class L>
struct _static_predicate_bitmap_impl<P, L<>>
{
    static constexpr unsigned long long value = 0;
};
 
template<template<class> class P, template<class...> class L, typename T1, typename... Ts>
struct _static_predicate_bitmap_impl<P, L<T1, Ts...>>
{
    static constexpr unsigned long long value = (_static_predicate_bitmap_impl<P, L<Ts...>>::value << 1) | (bool)(P<T1>::value);
};

template<template<class> class P, typename _>
struct _common_predicate_type { using type = void; };

template<template<class> class P, template<class...> class L>
struct _common_predicate_type<P, L<>> { using type = void; };

template<template<class> class P, template<class...> class L, typename... Ts>
struct _common_predicate_type<P, L<Ts...>> { using type = std::common_type_t<decltype(P<Ts>::value)...>; };

template<template<class> class P, typename _>
struct _static_predicate_array_impl {};

template<template<class> class P, template<class...> class L>
struct _static_predicate_array_impl<P, L<>> {};

template<template<class> class P, template<class...> class L, typename... Ts>
struct _static_predicate_array_impl<P, L<Ts...>>
{
    using value_t = typename _common_predicate_type<P, L<Ts...>>::type;
    static constexpr value_t value[] = { P<Ts>::value... };
};

template <template<class> class P, class L,
    bool = (
           boost::mp11::mp_size<L>::value <= (sizeof(unsigned long long) * 8)
        && std::is_same_v<typename _common_predicate_type<P, L>::type, bool>
    )>
struct test_static_predicate_impl
{
    inline bool operator()(std::size_t index) const
    {
        return (0x01 << index) & _static_predicate_bitmap_impl<P, L>::value;
    }
};

template <template<class> class P, class L>
struct test_static_predicate_impl<P, L, false>
{
    inline auto operator()(std::size_t index) const
    {
        return _static_predicate_array_impl<P, L>::value[index];
    }
};

}

// Tests whether metafunction predicate `P` is true for the member of type list 
// `L` at `index` where index is only known at runtime.
// WARNING undefined behavior if index exceeds `boost::mp11::mp_size<L>::value`
template<template<class> class P, class L>
inline auto test_static_predicate(std::size_t index)
{
    return metaprogramming_detail::test_static_predicate_impl<P, L> {} (index);
}

} // namespace Addle
