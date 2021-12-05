#pragma once

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)

#include <QtDebug>

#include <type_traits>

#include <optional>
#include <variant>
#include <functional>

#include <utility>
#include <boost/type_traits/is_detected_convertible.hpp>

namespace Addle::aux_debug {

template<typename T>
using _qdebug_t = decltype(std::declval<QDebug>() << std::declval<T>());
    
template<typename T>
using has_qdebug_stream = boost::is_detected_convertible<QDebug, _qdebug_t, T>;

QByteArray _typeNameHelper(const std::type_info& type);

// use to stream info about the current thread into QDebug
struct CurrentThreadInfo_t final {};
QDebug operator<< (QDebug d, CurrentThreadInfo_t);

constexpr CurrentThreadInfo_t CurrentThreadInfo = {};

void _functionHelper(QDebug& d, const void* function);

struct _stdVariantDebugVisitor
{
    QDebug& debug;
    
    template<typename T, std::enable_if_t<has_qdebug_stream<T>::value, void*> = nullptr>
    void operator()(const T& value) const 
    {
        debug << "value:" << value;
    }
    
    template<typename T, std::enable_if_t<!has_qdebug_stream<T>::value, void*> = nullptr>
    void operator()(const T&) const
    {
        debug << "<value type does not support QDebug>";
    }
};

}

template<class R, class... A>
inline QDebug operator<< (QDebug d, R(&function)(A...))
{ ::Addle::aux_debug::_functionHelper(d, reinterpret_cast<const void*>(&function)); return d; }

template<class R, class... A>
QDebug operator<< (QDebug d, R(*function)(A...))
{ ::Addle::aux_debug::_functionHelper(d, reinterpret_cast<const void*>(function)); return d; }

template<class R, class... A>
QDebug operator<< (QDebug d, R(&function)(A...) noexcept)
{ ::Addle::aux_debug::_functionHelper(d, reinterpret_cast<const void*>(&function)); return d; }

template<class R, class... A>
QDebug operator<< (QDebug d, R(*function)(A...) noexcept)
{ ::Addle::aux_debug::_functionHelper(d, reinterpret_cast<const void*>(function)); return d; }

namespace std {
    
    inline QDebug operator<< (QDebug d, std::type_info& info)
    {
        QDebugStateSaver s(d);
        d.noquote();
        d.nospace();
        d << "std::type_info(" << ::Addle::aux_debug::_typeNameHelper(info) << ")";
        return d;
    }
    
    template<typename T>
    QDebug operator<< (QDebug d, const std::optional<T>& optional)
    {
        QDebugStateSaver s(d);
        d.nospace();
        d << "std::optional(";
        
        if (optional)
            d << *optional;
        else
            d << "<empty>";
        
        d << ")";
        return d;
    }
    
    template<class... Ts, std::enable_if_t<
            ( ::Addle::aux_debug::has_qdebug_stream<Ts>::value || ... )
        , void*> = nullptr>
    QDebug operator<< (QDebug d, const std::variant<Ts...>& variant)
    {
        QDebugStateSaver s(d);
        d.nospace();
        d << "std::variant(";
        
        if (Q_LIKELY(!variant.valueless_by_exception()))
        {
            d << "index:" << variant.index();
            std::visit(::Addle::aux_debug::_stdVariantDebugVisitor { d }, variant);
        }
        else
        {
            d << "<valueless by exception>";
        }
        
        d << ")";
        return d;
    }
        
    template<class R, class... A>
    inline QDebug operator<< (QDebug d, const std::function<R(A...)>& function)
    {
        if (!function)
        {
            d << "std::function(<empty>)";
        }
        else if ( function.target_type() == typeid(R(A...)) )
        {
            QDebugStateSaver s(d);
            d.nospace();
            d << "std::function(";
            ::Addle::aux_debug::_functionHelper(d, reinterpret_cast<const void*>( function.template target<R(A...)>() ));
            d << ")";
        }
        else if ( function.target_type() == typeid(R(A...) noexcept) )
        {
            QDebugStateSaver s(d);
            d.nospace();
            d << "std::function(";
            ::Addle::aux_debug::_functionHelper(d, reinterpret_cast<const void*>( function.template target<R(A...) noexcept>() ));
            d << ")";
        }
        else
        {
            QDebugStateSaver s(d);
            d.nospace();
            d.noquote();
            d << "std::function(<callable of type "
                << ::Addle::aux_debug::_typeNameHelper(function.target_type())
                << ">)";
        }
        return d;
    }

} // namespace std

#endif // defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
