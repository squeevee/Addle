#pragma once

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)

#include <QtGlobal>
#include <QString>
#include <QBuffer>

#include <QtDebug>

#include <type_traits>
#include <optional>
#include <utility>
#include <boost/type_traits/is_detected_convertible.hpp>

namespace Addle::aux_debug {

template<typename T>
using _qdebug_t = decltype(std::declval<QDebug>() << std::declval<T>());
    
template<typename T>
using has_qdebug_stream = boost::is_detected_convertible<QDebug, _qdebug_t, T>;

template<typename T>
QString debugString(T&& v)
{
    QString result;
    QDebug(&result) << std::forward<T>(v);
    return result;
}

template<typename T>
QByteArray debugString_b(T&& v)
{
    QByteArray result;
    {
        QBuffer buff(&result);
        buff.open(QIODevice::WriteOnly);
        QDebug(&buff) << std::forward<T>(v);
    }
    return result;
}

}

namespace std {
    template<typename T>
    QDebug operator<< (QDebug d, const std::optional<T>& opt)
    {
        if (opt)
            return d << *opt;
        else
            return d << "(none)";
    }
} // namespace std

#endif // defined(ADDLE_DEBUG) || defined(ADDLE_TEST)

#ifdef ADDLE_TEST

#include <QtTest/QtTest>

#include <boost/utility/identity_type.hpp>

// While QDebug works well at making human-readable (and console safe) string
// representations of objects for essentially all purposes, QtTest requires a 
// bespoke toString function in order to print objects to the console from the 
// QCOMPARE macro.
//
// Fortunately, we can just use QDebug for this as well. A truly generic
// solution does not seem to be possible from our position, so we'll have to put 
// up with a little bit of boilerplating.
//
// NOTE: Qt recommends new code do not specialize the ::QTest::toString function 
// template and instead provide a `toString` function for ADL, but we're not 
// going to do that. A function with such a generic name, so little utility, and 
// a return value that is uncharacteristically awkward for Qt does not get place 
// in our namespaces.

// Standalone toString function body macro in case a custom signature is needed
#define QTEST_TOSTRING_IMPL_BY_QDEBUG_BODY(x)                               \
{                                                                           \
    return qstrdup(::Addle::aux_debug::debugString_b(x).data());            \
}

// NOTE: Type must be wrapped in parentheses when passed into this macro, e.g.,
// QTEST_TOSTRING_IMPL_BY_QDEBUG(( ::Addle::aux_spiff::Spiff ))
#define QTEST_TOSTRING_IMPL_BY_QDEBUG(Type)                                   \
template<> inline char* ::QTest::toString(const BOOST_IDENTITY_TYPE(Type)& x) \
QTEST_TOSTRING_IMPL_BY_QDEBUG_BODY(x)

#endif // ADDLE_TEST
