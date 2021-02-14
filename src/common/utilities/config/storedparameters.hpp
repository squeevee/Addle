#pragma once

#include "utilities/metaprogramming.hpp"

#include <type_traits>
#include <tuple>

#include <boost/parameter.hpp>
#include <boost/mp11.hpp>
#include <boost/type_traits.hpp>

namespace Addle::config_detail {

/** 
 * A container that stores arguments for deferred or multiple application to a 
 * Boost Parameter-enabled function.
 * 
 * L-value references are stored and handled as-is, whereas r-values are moved
 * or copied into the container (depending on e.g., constness and available
 * constructors). Keyword-tagged references (and other Boost Parameter argument 
 * packs) are automatically handled with respect to their value type(s).
 * 
 * This gives the correct object lifetime behavior for the majority of cases, 
 * and has syntax consistent with other Boost Parameter-enabled functions, but
 * is different from std::bind, as references do not need to be explicitly 
 * wrapped. Be careful not to store l-value references to temporary objects.
```
make_stored_parameters(r_ = QRect(0, 0, 1, 1)); 
    // temporary object passed as r-value, safe
    
this->_localParams = make_stored_parameters(spiff_ = *this);
    // long-lived object passed as l-value reference, safe
    
QRect r(1, 1, 2, 2);
make_stored_parameters(r_ = r);
    // temporary object passed as l-value, not safe
    
make_stored_parameters(r_ = std::move(r));
    // temporary object passed as explicit r-value, safe
```
 * The parameters in the container can be accessed through the toArgTuple()
 * member function, which returns a tuple for use with e.g., std::apply.
 * Depending on whether the && or const & overload is used will determine
 * whether held values are exposed as mutable r-value or const l-value
 * references respectively. L-value references are always exposed as l-value
 * references with their original constness.
 */
template<typename... Args>
class stored_parameters
{
    struct _no_tag {};
    
    template<class S, class V>
    struct _pair_type_op_argpack
    {
        template<typename Tag>
        using _pair_type = boost::mp11::mp_list<
                Tag,
                decltype( 
                    std::declval<V>() [
                        std::declval<boost::parameter::keyword<Tag> const &>()
                    ] 
                )
            >;
        
        using type = boost::mp11::mp_append<
                S, 
                boost::mp11::mp_transform<
                    _pair_type, 
                    boost::mp11::mp_map_keys<V>
                >
            >;
    };
    
    template<class S, class V>
    struct _pair_type_op_other
    {
        using type = boost::mp11::mp_push_back<
            S, 
            boost::mp11::mp_list<_no_tag, V>
        >;
    };
    
    template<class S, class V>
    using _pair_type_op = typename boost::mp11::mp_if<
            boost::parameter::is_argument_pack<V>,
            _pair_type_op_argpack<S, V>,
            _pair_type_op_other<S, V>
        >::type;
    
    using pair_types = boost::mp11::mp_fold<
            boost::mp11::mp_list<Args...>,
            boost::mp11::mp_list<>,
            _pair_type_op
        >;
        
    using tag_types = boost::mp11::mp_transform<
            boost::mp11::mp_first,
            pair_types
        >;
        
    using value_types = boost::mp11::mp_transform<
            boost::mp11::mp_second,
            pair_types
        >;
        
    template<typename T>
    using _get_storage_type = boost::mp11::mp_if<
            std::is_lvalue_reference<T>,
            T,
            std::decay_t<T>
        >;
        
    using storage_t = boost::mp11::mp_apply<
            std::tuple, 
            boost::mp11::mp_transform<
                _get_storage_type,
                value_types
            >
        >;
        
    template<typename T>
    using _is_tag = boost::mp11::mp_not<std::is_same<T, _no_tag>>;
        
    static_assert(
        boost::mp11::mp_is_set<
            boost::mp11::mp_filter<_is_tag, tag_types>
        >::value,
        "All arguments must either have a distinct keyword or no keyword."
    );
    
    template<
        typename T, 
        typename std::enable_if_t<
            boost::parameter::is_argument_pack<boost::remove_cv_ref_t<T>>::value, 
            void*> = nullptr
    >
    constexpr auto _data_from_arg(T&& arg)
    {
        return generate_tuple_over_list<
                boost::mp11::mp_map_keys<boost::remove_cv_ref_t<T>>
            >(
                [&] (auto t) -> auto&& {
                    using Tag = typename decltype(t)::type;
                    return arg[boost::parameter::keyword<Tag>::instance];
                }
            );
    }
    
    template<
        typename T,
        typename std::enable_if_t<
            !boost::parameter::is_argument_pack<boost::remove_cv_ref_t<T>>::value, 
            void*> = nullptr
    >
    constexpr auto _data_from_arg(T&& arg)
    {
        return std::tuple<T&&>(std::forward<T>(arg));
    }
    
    template<typename Storage, bool Move = false>
    struct arg_tuple_functor
    {
        arg_tuple_functor(Storage& data_)
            : data(data_)
        {
        }
        
        template<
            class Pair, 
            class I, 
            typename std::enable_if_t<
                _is_tag<boost::mp11::mp_first<Pair>>::value 
                && (
                    std::is_lvalue_reference<boost::mp11::mp_second<Pair>>::value
                    || !Move
                ),
            void*> = nullptr
        >
        auto operator()(boost::mp11::mp_identity<Pair>, I) const
        {
            using Tag = boost::mp11::mp_first<Pair>;
            return boost::parameter::keyword<Tag>::instance = std::get<I::value>(data);
        }
        
        template<
            class Pair, 
            class I, 
            typename std::enable_if_t<
                _is_tag<boost::mp11::mp_first<Pair>>::value 
                && !std::is_lvalue_reference<boost::mp11::mp_second<Pair>>::value
                && Move,
            void*> = nullptr
        >
        auto operator()(boost::mp11::mp_identity<Pair>, I) const
        {
            using Tag = boost::mp11::mp_first<Pair>;
            return boost::parameter::keyword<Tag>::instance = std::move(std::get<I::value>(data));
        }
        
        template<
            class Pair, 
            class I, 
            typename std::enable_if_t<
                !_is_tag<boost::mp11::mp_first<Pair>>::value 
                && (
                    std::is_lvalue_reference<boost::mp11::mp_second<Pair>>::value
                    || !Move
                ),
            void*> = nullptr
        >
        auto&& operator()(boost::mp11::mp_identity<Pair>, I) const
        {
            return std::get<I::value>(data);
        }

        template<
            class Pair, 
            class I, 
            typename std::enable_if_t<
                !_is_tag<boost::mp11::mp_first<Pair>>::value 
                && !std::is_lvalue_reference<boost::mp11::mp_second<Pair>>::value
                && Move,
            void*> = nullptr
        >
        auto&& operator()(boost::mp11::mp_identity<Pair>, I) const
        {
            return std::move(std::get<I::value>(data));
        }
        
        Storage& data;
    };
    
public:
    constexpr stored_parameters(Args&&... args)
        : _data(
            std::tuple_cat(
                _data_from_arg(std::forward<Args>(args))...
            )
        )
    {
    }
    
    auto toArgTuple() const &
    {
        return generate_tuple_over_list<pair_types>(
                arg_tuple_functor<const storage_t, false>(_data)
            );
    }

    auto toArgTuple() &&
    {
        return generate_tuple_over_list<pair_types>(
                arg_tuple_functor<storage_t, true>(_data)
            );
    }
    
private:
    storage_t _data;
};

template<typename... Args>
stored_parameters<Args...> make_stored_parameters(Args&&... args)
{
    return stored_parameters<Args...>(std::forward<Args>(args)...);
}
} // namespace Addle::config_detail
