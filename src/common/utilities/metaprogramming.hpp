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

} // namespace Addle
