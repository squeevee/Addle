#pragma once

#include <boost/di.hpp>
#include <boost/mp11.hpp>

#include "utilities/metaprogramming.hpp"

namespace Addle::config_detail {

// A group of injector dependencies injected into a constructor as a single
// object. Allows for metaprogrammatic dependency requirements. Also available
// as a convenience for classes with long constructor parameter lists.
template<typename... Deps>
struct InjectBundle
{
private:
    using _deps_list = boost::mp11::mp_list<Deps...>;
    
    template<typename T>
    static constexpr void _assert_unique_value()
    {
        static_assert(
            mp_contains_once<_deps_list, T>::value,
            "This InjectBundle contains multiple instances of the requested "
            "type T, meaning that `value<T>()` has ambiguous meaning and is "
            "not allowed. Instead, use `std::get` on the `data` member to "
            "acquire the desired value by its position."
        );
    }
    
public:
    using boost_di_inject__ = boost::di::inject<Deps&&...>;
    inline InjectBundle(Deps&&... deps)
        : data(std::forward<decltype(deps)>(deps)...)
    {
    }
    
    inline operator std::tuple<Deps...> () const { return data; }

    template<
        typename T, 
        typename std::enable_if_t<std::is_reference<T>::value, void*> = nullptr
        >
    inline T value() const
    {
        _assert_unique_value<T>();
        return std::forward<T>(
            std::get<boost::mp11::mp_find<_deps_list, T>::value>(data)
        );
    }
    
    template<
        typename T, 
        typename std::enable_if_t<!std::is_reference<T>::value, void*> = nullptr
        >
    inline T& value() &
    {
        _assert_unique_value<T>();
        return std::get<boost::mp11::mp_find<_deps_list, T>::value>(data);
    }
    
    template<
        typename T, 
        typename std::enable_if_t<!std::is_reference<T>::value, void*> = nullptr
        >
    inline const T& value() const
    {
        _assert_unique_value<T>();
        return std::get<boost::mp11::mp_find<_deps_list, T>::value>(data);
    }
    
    template<
        typename T,
        std::enable_if_t<!boost::mp11::mp_contains<_deps_list, T>::value, void*> = nullptr
        >
    inline operator T& () const
    {
        return this->value<T&>();
    }
    
    template<
        typename T,
        std::enable_if_t<!boost::mp11::mp_contains<_deps_list, T>::value, void*> = nullptr
        >
    inline operator T&& () const
    {
        return this->value<T&&>();
    }
    
    template<
        typename T,
        std::enable_if_t<!boost::mp11::mp_contains<_deps_list, T&>::value, void*> = nullptr
        >
    inline operator T& () &
    {
        return this->value<T>();
    }
    
    template<
        typename T,
        std::enable_if_t<!boost::mp11::mp_contains<_deps_list, T&&>::value, void*> = nullptr
        >
    inline operator T&& () &&
    {
        return this->value<T>();
    }

    // These overloads are deleted because the deps list contains the type as 
    // itself and as reference-qualified, making the operator's meaning 
    // ambiguous.
    
    template<
        typename T,
        std::enable_if_t<
            boost::mp11::mp_contains<_deps_list, T&>::value
            && boost::mp11::mp_contains<_deps_list, T>::value, 
        void*> = nullptr
        >
    inline operator T& () const = delete;
        
    template<
        typename T, std::enable_if_t<
            boost::mp11::mp_contains<_deps_list, T&&>::value
            && boost::mp11::mp_contains<_deps_list, T>::value, 
        void*> = nullptr
        >
    inline operator T&& () const = delete;

    std::tuple<Deps...> data;
};

} // namespace Addle::config_detail
