#pragma once

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)

#include <utility> // std::forward

#include <QString>
#include <QtDebug>

namespace Addle::aux_debug {
    
template<typename T>
QString debugString(T&& v)
{
    QString result;
    QDebug debug(&result);
    debug.setAutoInsertSpaces(false);
    debug << std::forward<T>(v);
    return result;
}

} // namespace Addle::aux_debug

#endif // defined(ADDLE_DEBUG) || defined(ADDLE_TEST)

#ifdef ADDLE_TEST

#include <QtGlobal> // qstrdup
#include <QtTest/QtTest>

// QtTest's QCOMPARE macro specifies its own bespoke API for getting string 
// representations of objects to print to console in the event of comparison 
// failures. Unlike QDebug, this API is not useful for any other purpose, and is 
// quite awkward in practice.
// 
// The macro QTEST_TOSTRING_IMPL_BY_QDEBUG declares and defines a function that 
// takes the string provided by an existing QDebug operator<< overload and 
// exposes it to QCOMPARE.
// 
// The macro QTEST_TOSTRING_IMPL_BY_QDEBUG_BODY does the same, but provides only 
// the function body, for cases where a custom signature is needed (e.g., 
// templates requiring partial specialization and/or having commas in their type 
// names).
// 
// NOTE: Qt recommends for new code not to specialize `QTest::toString`, but to
// define namespace-level ADL function overloads. In the interest of avoiding
// esoteric clutter, Addle will not follow this recommendation.

#define QTEST_TOSTRING_IMPL_BY_QDEBUG_BODY(x) \
{ return ::qstrdup(::Addle::aux_debug::debugString(x).toLocal8Bit().data()); }

#define QTEST_TOSTRING_IMPL_BY_QDEBUG(Type) \
inline char* ::QTest::toString(const Type& x) QTEST_TOSTRING_IMPL_BY_QDEBUG_BODY(x)

#endif // ADDLE_TEST
