#pragma once

#include <functional>
#include <QByteArray>
#include <QUuid>

#include <cstring>
#include <cstdint>
#include <array>
#include <boost/mp11.hpp>

#include "interfaces/iamqobject.hpp"
#include "utilities/metaprogramming.hpp"

#include "utilities/qobject.hpp"

#include "../config/factoryparams.hpp"

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
#include <QDebug> // operator<<
#endif

namespace Addle::aux_config3 {
    
template<typename _>
struct _get_anonymous_params_impl;

template<template<class...> class P, typename... Specs>
struct _get_anonymous_params_impl<P<Specs...>>
{
    std::array<const void*, sizeof...(Specs)> operator()(const P<Specs...>& p) const
    {
        return { p.template addressof<boost::mp11::mp_first<Specs>>()... };
    }
};

template<typename Params>
inline auto _get_anonymous_params(const Params& p) { return _get_anonymous_params_impl<Params> {} (p); }
    
template<typename T, typename U, 
    bool = std::is_enum_v<U>,
    bool = std::is_pointer_v<U>>
struct _integral_alias_impl
{ 
    using type = boost::mp11::mp_bool<(
               std::is_same_v<T, U> 
            || (
                   std::is_integral_v<U>
                && std::is_signed_v<T> == std::is_signed_v<U>
                && sizeof(T) == sizeof(U)
            )
        )>;
};

template<typename T, typename U>
struct _integral_alias_impl<T, U, true, false>
{ 
    using type = boost::mp11::mp_or<
            std::is_same<T, U>, 
            typename _integral_alias_impl<T, std::underlying_type_t<U>>::type
        >; 
};

template<typename T, typename U>
struct _integral_alias_impl<T, U, false, true>
{ 
    using type = boost::mp11::mp_or<
            std::is_same<T, U>, 
            typename _integral_alias_impl<T, std::uintptr_t>::type
        >; 
};

template<typename T>
struct _integral_alias_q { template<typename U> using fn = typename _integral_alias_impl<T, U>::type; };

// template<typename T, typename U>
// using copy_signedness_t = boost::mp11::mp_cond<
//         std::is_signed<U>, boost::make_signed_t<T>,
//         std::is_unsigned<U>, boost::make_unsigned_t<T>
//     >;

template<typename T>
struct _is_same_as_q { template<typename U> using fn = std::is_same<T, U>; };

template<typename T>
struct _bind_condition_bytes_traits
{
    static constexpr bool enabled = false;
};

template<> struct _bind_condition_bytes_traits<QByteArray>
{
    static constexpr bool enabled = true;
    static QByteArray toBytes(QByteArray bytes) { return bytes; }
    static bool equals(const QByteArray& lhs, const QByteArray& rhs) { return lhs == rhs; }
};

template<> struct _bind_condition_bytes_traits<const char*>
{
    static constexpr bool enabled = true;
    static QByteArray toBytes(const char* bytes) { return QByteArray(bytes); }
    static bool equals(const char* lhs, const QByteArray& rhs) { return lhs == rhs; }
};

template<> struct _bind_condition_bytes_traits<QString>
{
    static constexpr bool enabled = true;
    static QByteArray toBytes(const QString& string) { return string.toUtf8(); }
    static bool equals(const QString& lhs, const QByteArray& rhs) { return lhs == rhs; }
};

template<> struct _bind_condition_bytes_traits<QUuid>
{
    static constexpr bool enabled = true;
    static QByteArray toBytes(const QUuid& id) { return id.toByteArray(); }
    static bool equals(const QUuid& lhs, const QByteArray& rhs) 
    { 
        return lhs == QUuid(rhs);
    }
};

template<typename _> struct _bind_condition_bytes_equal_impl;

template<template<typename...> class L, typename... Ts>
struct _bind_condition_bytes_equal_impl<L<Ts...>>
{
    QByteArray bytes;
    const void** values;
    const std::size_t index;
    
    bool operator()() const { return (_test<Ts>() || ...); }
    
    template<typename T, std::enable_if_t<
            _bind_condition_bytes_traits<T>::enabled
        , void*> = nullptr>
    bool _test() const
    {
        return test_static_predicate<_is_same_as_q<T>::template fn, L<Ts...>>(index)
            && _bind_condition_bytes_traits<T>::equals(
                    *reinterpret_cast<const T*>(values[index]),
                    bytes
                );
    }
    
    template<typename T, std::enable_if_t<
            !_bind_condition_bytes_traits<T>::enabled
        , void*> = nullptr>
    bool _test() const { return false; }
};


template<typename _> struct _bind_condition_bytes_extract_impl;

template<template<typename...> class L, typename... Ts>
struct _bind_condition_bytes_extract_impl<L<Ts...>>
{
    const void** values;
    const std::size_t index;
    
    QByteArray operator()() const
    {
        QByteArray result;
        (void)(
            ( test_static_predicate<_is_same_as_q<Ts>::template fn, L<Ts...>>(index) && (_extract<Ts>(result), true)) || ...
        );
        
        return result;
    }
    
    template<typename T, std::enable_if_t<
            _bind_condition_bytes_traits<T>::enabled
        , void*> = nullptr>
    void _extract(QByteArray& result) const
    {
        result = _bind_condition_bytes_traits<T>::toBytes(
                *reinterpret_cast<const T*>(values[index])
            );
    }
    
    template<typename T, std::enable_if_t<
            !_bind_condition_bytes_traits<T>::enabled
        , void*> = nullptr>
    void _extract(QByteArray&) const { Q_UNREACHABLE(); }
};

extern bool _typename_check_impl(const QObject*, const QByteArray& name, bool exact);   // defined in interfacebindingconfig.cpp
extern bool _typename_check_impl(const QVariant&, const QByteArray& name, bool exact, QVariant* overwrite);  // defined in interfacebindingconfig.cpp

using _typename_check_function_ptr_t = bool (*)(const void*, const QByteArray&, bool, void*);

template<typename T
        , bool = (std::is_base_of_v< QObject, std::remove_cv_t<std::remove_pointer_t<T>> >)
        , bool = (Traits::implemented_as_QObject<std::remove_pointer_t<T>>::value)
    >
struct _typename_check_dispatcher
{
    static constexpr _typename_check_function_ptr_t value = nullptr;
};

template<typename T, bool _>
struct _typename_check_dispatcher<T, true, _>
{
    static bool impl(const void* v, const QByteArray& name, bool exact, void*)
    { 
        return _typename_check_impl(static_cast<const QObject*>(reinterpret_cast<const T*>(v)), name, exact);
    }
    static constexpr _typename_check_function_ptr_t value = &impl;
};

template<typename T, bool _>
struct _typename_check_dispatcher<T*, true, _>
{
    static bool impl(const void* v, const QByteArray& name, bool exact, void*)
    { 
        return _typename_check_impl(static_cast<const QObject*>(*reinterpret_cast<const T* const *>(v)), name, exact); 
    }
    static constexpr _typename_check_function_ptr_t value = &impl;
};

template<typename T>
struct _typename_check_dispatcher<T, false, true>
{
    static bool impl(const void* v, const QByteArray& name, bool exact, void*) 
    { 
        return _typename_check_impl(qobject_interface_cast<const QObject*>(reinterpret_cast<const T*>(v)), name, exact); 
    }
    static constexpr _typename_check_function_ptr_t value = &impl;
};

template<typename T>
struct _typename_check_dispatcher<T*, false, true>
{
    static bool impl(const void* v, const QByteArray& name, bool exact, void*) 
    { 
        return _typename_check_impl(qobject_interface_cast<const QObject*>(*reinterpret_cast<const T* const *>(v)), name, exact); 
    }
    static constexpr _typename_check_function_ptr_t value = &impl;
};

template<>
struct _typename_check_dispatcher<QVariant, false, false>
{
    static bool impl(const void* v, const QByteArray& name, bool exact, void* overwrite) 
    { 
        return _typename_check_impl(*reinterpret_cast<const QVariant*>(v), name, exact, reinterpret_cast<QVariant*>(overwrite)); 
    }
    static constexpr _typename_check_function_ptr_t value = &impl;
};

template<typename _>
struct _typename_check_dispatch_table;

template<template<typename...> class L, typename... Ts>
struct _typename_check_dispatch_table<L<Ts...>>
{
    static constexpr _typename_check_function_ptr_t value[sizeof...(Ts)] = {
            _typename_check_dispatcher<Ts>::value...
        };
};

// Encapsulates a set of conditions that InjectorConfig can apply to a set of 
// factory parameters to choose an implementation at runtime. Arbitrary code may
// be used to implement this (via a std::function object), but for the benefit 
// of efficiency (e.g., hash lookup, branch target optimization) and debugging 
// (inspecting values at failure, preventing some redundancies) a narrow set of
// condition features are exposed transparently.
struct BindingCondition
{
    union {
        int             asInt;
        unsigned        asUnsigned;
        long            asLong;
        unsigned long   asUnsignedLong;
    } number;
    
    QByteArray bytes;
    
    int paramIndex = -1;
    
    using function_t = std::function<bool(const void*)>;
    function_t function;
    
#ifdef ADDLE_DEBUG
    QByteArray paramName;
    QString (*valueToString)(const BindingCondition&) = nullptr;
    const std::type_info* paramSetType = nullptr;
#endif
    
    enum { None, Int, Unsigned, Long, UnsignedLong, Bytes, TypeName } 
    valueType : 7;
    bool exactTypeName : 1;
    
    struct bytes_tag {};
    struct typename_tag {};
    
    BindingCondition() : valueType(None) {}
    BindingCondition(const BindingCondition&) = default;
    
    bool isNull() const { return valueType == None && !function; }
    
    template<typename Params>
    bool test(const Params& p) const
    {
        static_assert(config_detail::_is_factory_param_set<Params>::value);
        using value_types = boost::mp11::mp_transform<boost::remove_cv_ref_t, typename Params::value_types>;
        
#ifdef ADDLE_DEBUG
        assert(paramSetType && *paramSetType == typeid(Params));
#endif
        assert(valueType == None || (paramIndex >= 0 && paramIndex < boost::mp11::mp_size<Params>::value));
        
        switch(valueType)
        {
            case None:
                break;
            
            case Int:
            {
                auto anonParams = _get_anonymous_params(p);
                if (test_static_predicate<_integral_alias_q<int>::template fn, value_types>(paramIndex)
                    && *reinterpret_cast<const int*>(anonParams[paramIndex]) == number.asInt) break;
                else if (test_static_predicate<_integral_alias_q<short>::template fn, value_types>(paramIndex)
                    && *reinterpret_cast<const short*>(anonParams[paramIndex]) == number.asInt) break;
                else if (test_static_predicate<_integral_alias_q<char>::template fn, value_types>(paramIndex)
                    && *reinterpret_cast<const char*>(anonParams[paramIndex]) == number.asInt) break;
                return false;
            }
                
            case Unsigned:
            {
                auto anonParams = _get_anonymous_params(p);
                if (test_static_predicate<_integral_alias_q<unsigned int>::template fn, value_types>(paramIndex)
                    && *reinterpret_cast<const unsigned int*>(anonParams[paramIndex]) == number.asInt) break;
                else if (test_static_predicate<_integral_alias_q<unsigned short>::template fn, value_types>(paramIndex)
                    && *reinterpret_cast<const unsigned short*>(anonParams[paramIndex]) == number.asInt) break;
                else if (test_static_predicate<_integral_alias_q<unsigned char>::template fn, value_types>(paramIndex)
                    && *reinterpret_cast<const unsigned char*>(anonParams[paramIndex]) == number.asInt) break;
                return false;
            }
                
            case Long:
                if (test_static_predicate<_integral_alias_q<long>::template fn, value_types>(paramIndex)
                    && *reinterpret_cast<const long*>(_get_anonymous_params(p)[paramIndex]) == number.asLong) break;
                return false;
                
            case UnsignedLong:
                if (test_static_predicate<_integral_alias_q<unsigned long>::template fn, value_types>(paramIndex)
                    && *reinterpret_cast<const unsigned long*>(_get_anonymous_params(p)[paramIndex]) == number.asUnsignedLong) break;
                return false;
                
            case Bytes:
                if (test_bytes<Params>(bytes, _get_anonymous_params(p).data())) break;
                return false;
                
            case TypeName:
                if (test_type_name<Params>(bytes, _get_anonymous_params(p).data())) break;
                return false;
        }
        
        return (!function) || function(std::addressof(p));
    }

private:
    template<class Params>
    bool test_bytes(const QByteArray& bytes, const void** anonParams) const
    {
        using value_types = boost::mp11::mp_transform<boost::remove_cv_ref_t, typename Params::value_types>;
        return _bind_condition_bytes_equal_impl<value_types> { bytes, anonParams }();
    }
    
    template<class Params>
    bool test_type_name(const QByteArray& typeName, const void** anonParams) const
    {
        using value_types     = boost::mp11::mp_transform<boost::remove_cv_ref_t,  typename Params::value_types>;
        using cv_values_types = boost::mp11::mp_transform<std::remove_reference_t, typename Params::value_types>;
        
        auto impl = _typename_check_dispatch_table<value_types>::value[paramIndex];
        assert(impl);
        
        void* overwrite = nullptr;
        
        if(!function && test_static_predicate<_is_same_as_q<QVariant>::template fn, cv_values_types>(paramIndex))
        {
            // The typename check for QVariant may need to perform non-trivial 
            // conversion logic to determine at runtime if the given value is 
            // actually convertible to the desired type (e.g., in the case of 
            // QString -> int, canConvert will always return true but only some
            // values like QString("49") can be successfully converted.)
            // 
            // In order to avoid unnecessarily performing such conversion 
            // multiple times, the typename check will be permitted to overwrite 
            // the original value with the converted value under certain 
            // conditions:
            // 
            // 1) This BindingCondition must not have a function, since the 
            // conversion is potentially destructive, we must be sure that we do
            // not modify its value and mess up subsequent conditional checks.
            // 
            // 2) The QVariant in question must be owned as a first-hand value 
            // or a non-const reference.
            
            overwrite = const_cast<void*>(anonParams[paramIndex]);
        }
        
        return impl(anonParams[paramIndex], typeName, exactTypeName, overwrite);
    }
};

template<typename T, typename U>
struct _is_same_functor
{
    bool operator()(const void* otherValue_) const
    {
        assert(otherValue_);
        return value == *reinterpret_cast<const std::remove_reference_t<U>*>(otherValue_);
    }
    
    std::decay_t<T> value;
};

template<typename Params, typename Tag>
struct BindingConditionBuilder
{
    static_assert(config_detail::_is_factory_param_set<Params>::value 
        && boost::mp11::mp_map_contains<Params, Tag>::value);
    
    BindingConditionBuilder()
    {
        result.paramIndex = Params::template get_index_of<Tag>::value;
#ifdef ADDLE_DEBUG
        result.paramName = Tag::keyword_name();
        result.paramSetType = &typeid(Params);
#endif
    }
    
    using _param_value_t = boost::remove_cv_ref_t<typename Params::template get_value_type_of<Tag>>;
    
    template<typename T>
    using _not_transparent_type = boost::mp11::mp_not<boost::mp11::mp_any<
            _integral_alias_q<int          >::template fn<boost::remove_cv_ref_t<T>>,
            _integral_alias_q<unsigned     >::template fn<boost::remove_cv_ref_t<T>>,
            _integral_alias_q<long         >::template fn<boost::remove_cv_ref_t<T>>,
            _integral_alias_q<unsigned long>::template fn<boost::remove_cv_ref_t<T>>,
            boost::mp11::mp_bool<_bind_condition_bytes_traits<boost::remove_cv_ref_t<T>>::enabled>
        >>;
    
    template<typename T, std::enable_if_t<
               std::is_constructible_v<_param_value_t, T&&>
            && std::is_signed_v<T>
            && _integral_alias_q<int>::template fn<_param_value_t>::value
        , void*> = nullptr>
    BindingConditionBuilder& setValue(T v)
    {
        result.valueType = BindingCondition::Int;
        result.number.asInt = reinterpret_cast<int>(_param_value_t(std::move(v)));
        return *this;
    }
    
    template<typename T, std::enable_if_t<
               std::is_constructible_v<_param_value_t, T&&>
            && std::is_unsigned_v<T>
            && _integral_alias_q<unsigned>::template fn<_param_value_t>::value
        , void*> = nullptr>
    BindingConditionBuilder& setValue(T v)
    {
        result.valueType = BindingCondition::Unsigned;
        result.number.asUnsigned = reinterpret_cast<unsigned>(_param_value_t(std::move(v)));
        return *this;
    }
        
    template<typename T, std::enable_if_t<
               _integral_alias_q<long>::template fn<T>::value
            && _integral_alias_q<long>::template fn<_param_value_t>::value
        , void*> = nullptr>
    BindingConditionBuilder& setValue(T v)
    {
        result.valueType = BindingCondition::Long;
        result.number.asLong = reinterpret_cast<long>(_param_value_t(std::move(v)));
        return *this;
    }

    template<typename T, std::enable_if_t<
               _integral_alias_q<unsigned long>::template fn<T>::value
            && _integral_alias_q<unsigned long>::template fn<_param_value_t>::value
        , void*> = nullptr>
    BindingConditionBuilder& setValue(T v)
    {
        result.valueType = BindingCondition::UnsignedLong;
        result.number.asUnsignedLong = reinterpret_cast<unsigned long>(_param_value_t(std::move(v)));
        return *this;
    }
    
    template<typename T, std::enable_if_t<
               std::is_constructible_v<_param_value_t, T&&>
            && std::is_constructible_v<_param_value_t, boost::remove_cv_ref_t<T>>
        , void*> = nullptr>
    BindingConditionBuilder& setValue(T&& v)
    {
        result.valueType = BindingCondition::Bytes;
        result.bytes = _bind_condition_bytes_traits<_param_value_t>::toBytes(std::forward<T>(v));
        return *this;
    }
    
    template<typename T, std::enable_if_t<
               std::is_constructible_v<_param_value_t, T&&>
            && _not_transparent_type<_param_value_t>::value
        , void*> = nullptr>
    BindingConditionBuilder& setValue(T&& v)
    {
        result.valueType = BindingCondition::None;
        result.function = _is_same_functor<T, _param_value_t> { std::forward<T>(v) };
        return *this;
    }
    
    BindingCondition result;
};

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
QDebug operator<<(QDebug, const BindingCondition&); // defined in interfacebindingconfig.cpp
#endif

}
