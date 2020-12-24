/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#pragma once

#include <tuple>
#include <type_traits>
#include <memory>
#include <utility>

#include <cstring>

#include <boost/type_traits.hpp>
#include <boost/mp11.hpp>

namespace Addle {
    
// TODO: make new class template `piecemeal_tuple` which:
// 1) stores a tuple as raw memory
// 2) initializes members by emplacing
// 3) tracks which members are initialized, destroys them where appropriate
// 4) automatically wraps reference types
// 5) is convertible to an equivalent std::tuple
//
// By having a destructor, this will be exception safe without the need for
// explicit exception handling. This would be very similar to a tuple of
// optionals, but could potentially be upstreamed to Mp11 without creating a
// dependency on Boost/C++17 

template<typename T, std::size_t I>
struct generate_tuple_index
{
    using type = T;
    static constexpr std::size_t index = I;
};

template<typename L, typename F, typename I>
using _tuple_gen_t = decltype(
        std::declval<F>()
        (
            std::declval<generate_tuple_index<boost::mp11::mp_at<L, I>, I::value>>()
        )
    );

template<typename L, typename F>
using _tuple_gen_types = boost::mp11::mp_transform_q<
        boost::mp11::mp_bind<
            _tuple_gen_t,
            L,
            F,
            boost::mp11::_1
        >,
        boost::mp11::mp_iota<boost::mp11::mp_size<L>>
    >;

template<typename Tuple, typename F, typename L>
struct _tuple_emplacer
{
    inline _tuple_emplacer(Tuple& tuple, F&& f, int& i)
        : _tuple(tuple), _f(std::move(f)), _i(i = -1)
    {
    }
    
    template<std::size_t I>
    inline void operator()(boost::mp11::mp_size_t<I>) const
    {
        using InputType = boost::mp11::mp_at_c<L, I>;
        using OutputType = typename std::tuple_element<I, Tuple>::type;
        OutputType* address = std::addressof(std::get<I>(_tuple));

        auto&& result = _f(generate_tuple_index<InputType, I>());
        
        new(address) OutputType(std::move(result));
        ++_i;
    }
    
    Tuple& _tuple;
    F&& _f;
    int& _i;
};

template<typename Tuple>
struct _tuple_partial_cleanup
{
    inline _tuple_partial_cleanup(Tuple& tuple, int i)
        : _tuple(tuple), _i(i)
    {
    }
    
    template<std::size_t I>
    inline void operator()(boost::mp11::mp_size_t<I>) const noexcept
    {
        if (_i < I)
            return;
        
        using T = typename std::tuple_element<I, Tuple>::type;
        std::get<I>(_tuple).~T();
    }
    
    Tuple& _tuple;
    const int _i;
};


template<typename L, typename F, template<typename...> class Tuple = std::tuple>
boost::mp11::mp_apply<Tuple, _tuple_gen_types<L, F>> generate_tuple_over(F&& f)
{
    using namespace boost::mp11;
    
    static_assert(
        mp_is_list<L>::value, 
        "L must be a Boost Mp11-compatible type list"
    );
    
    static_assert(
        mp_none_of<_tuple_gen_types<L, F>, std::is_reference>::value,
        "This function cannot generate tuples containing references. Consider "
        "using std::reference_wrapper in place of reference types."
    );
    
    using result_t = mp_apply<Tuple, _tuple_gen_types<L, F>>;
    constexpr std::size_t result_size = std::tuple_size<result_t>::value;
    
    typename std::aligned_storage<sizeof(result_t), alignof(result_t)>::type raw_data;
    auto& result = *reinterpret_cast<result_t*>(&raw_data);
    
    int i;
    try {
        mp_for_each<mp_iota_c<result_size>>(
            _tuple_emplacer<result_t, F, L>(result, std::move(f), i)
        );
    }
    catch(...)
    {
        mp_for_each<mp_iota_c<result_size>>(
            _tuple_partial_cleanup<result_t>(result, i)
        );
        throw;
    }
    
    return result;
}

} // namespace Addle
