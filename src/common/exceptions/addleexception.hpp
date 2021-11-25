/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ADDLEEXCEPTION_HPP
#define ADDLEEXCEPTION_HPP

#include <type_traits>
#include <boost/type_traits/is_detected_convertible.hpp>
#include <boost/type_traits.hpp>

#ifdef ADDLE_DEBUG
#include <functional> // std::function
# if defined(ADDLE_USE_LIBBACKTRACE)
#  define BOOST_STACKTRACE_USE_BACKTRACE
# endif
#include <boost/stacktrace/stacktrace.hpp>
#endif // ADDLE_DEBUG

#include "compat.hpp"
#include <exception>
#include <type_traits>
#include <typeinfo>

#include <variant>

#include <QList>
#include <QAtomicInteger>
#include <QMutex>
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QException>

#include <QMetaType>
#include <QSharedPointer>

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
#include <QtDebug> // operator<<
#endif

namespace Addle {
 
/**
 * @brief Abstract base class for rich exceptions in Addle.
 */
class ADDLE_COMMON_EXPORT AddleException : public QException
{
public:
    using inner_exception_t = std::variant<QSharedPointer<AddleException>, std::exception_ptr>;
    struct AddleExceptionBuilder
    {
        AddleExceptionBuilder() = default;
        AddleExceptionBuilder(const AddleExceptionBuilder&) = default;
        AddleExceptionBuilder(AddleExceptionBuilder&&) = default;
        
        AddleExceptionBuilder(QString what_) : what(std::move(what_)) {}
        AddleExceptionBuilder(QList<inner_exception_t> innerExceptions_) 
            : innerExceptions(std::move(innerExceptions_)) 
        {
        }
        
        QString what;
        AddleExceptionBuilder& setWhat(QString what_) { what = std::move(what_); return *this; }
        
        QList<inner_exception_t> innerExceptions;
        AddleExceptionBuilder& setInnerExceptions(QList<inner_exception_t> innerExceptions_)
        {
            innerExceptions = std::move(innerExceptions_);
            return *this;
        }
        
#ifdef ADDLE_DEBUG
        using property_t = std::function<QString(const AddleException&)>;
        QMap<QByteArray, property_t> properties;
        AddleExceptionBuilder& addProperty(QByteArray name, property_t property)
        {
            properties.insert(name, property);
            return *this;
        }
#endif
    };
    
    AddleException& operator=(const AddleException&) = delete;
    
    virtual bool isLogicError() const { return false; }
    virtual bool isRuntimeError() const { return false; }

    // Throws this exception as its most derived polymorphic type.
    [[noreturn]] virtual void raise() const override = 0;

    // Makes a copy of this exception as its most derived polymorphic type.
    virtual AddleException* clone() const override = 0;
    
    const QList<inner_exception_t> innerExceptions() const { return _innerExceptions; }
    
protected:
    QList<inner_exception_t> _innerExceptions;
    
#ifdef ADDLE_DEBUG
public:
    struct HistoryEntry
    {
        const char* function;
        const char* file;
        int line;
        QDateTime dateTime;
        QString threadName;
        const void* threadId;
        
        boost::stacktrace::stacktrace backTrace;
    };
    
    explicit AddleException(AddleExceptionBuilder builder);
    AddleException(const AddleException&);
    virtual ~AddleException();
    
    [[noreturn]] void debugRaise(const char* function, const char* file, const int line);
        
    const QList<HistoryEntry> history() const { return _history; }
    
    const char* what() const noexcept override;
    
protected:
    using debug_data_t = QList< std::pair<QByteArray, QString> >;
    virtual debug_data_t debugData() const;
    void dataChanged();
    
    virtual QString what_p(int verbosity = 2) const;
    
private:   
    debug_data_t debugData_p() const;

    QList<HistoryEntry> _history;
    const QString _what;
    QMap<QByteArray, AddleExceptionBuilder::property_t> _properties;
    
    mutable QByteArray _whatBytes;
    mutable QMutex _whatBytesMutex;
    
    mutable debug_data_t _debugDataCache;
    mutable bool _debugDataCacheIsSet = false;
    mutable QMutex _debugDataCacheMutex;
#else
public:
    explicit AddleException(AddleExceptionBuilder builder)
        : _innerExceptions(std::move(builder.innerExceptions)), 
        _whatBytes(builder.what.toUtf8())
    {
    }
    virtual ~AddleException() = default;
    
    const char* what() const noexcept override { return _whatBytes.constData(); }
private:
    const QByteArray _whatBytes;
#endif
    
#if defined(ADDLE_DEBUG)
    friend QDebug operator<<(QDebug debug, const AddleException& ex)
    {
        QDebugStateSaver s(debug);
        return debug.noquote() << ex.what_p(debug.verbosity());
    }
#elif defined(ADDLE_TEST)
    friend QDebug operator<<(QDebug debug, const AddleException& ex)
    {
        QDebugStateSaver s(debug);
        return debug.noquote() << "AddleException(" << ex._whatBytes << ")";
    }
#endif
};

#define ADDLE_EXCEPTION_BOILERPLATE(T)                                      \
public:                                                                     \
    [[noreturn]] void raise() const override { throw *this; }               \
    T* clone() const override { return new T(*this); }                      \
    bool isLogicError() const override                                      \
    { return Traits::is_logic_error<T>::value; }                            \
    bool isRuntimeError() const override                                    \
    { return Traits::is_runtime_error<T>::value; }

namespace Traits {

namespace detail {

template<typename T>
using _is_logic_error_t = decltype(T::IsLogicError);

template<typename T, bool = boost::is_detected_convertible<bool, _is_logic_error_t, T>::value>
struct is_logic_error_impl : std::integral_constant<bool, T::IsLogicError> {};

template<typename T>
struct is_logic_error_impl<T, false> : std::false_type {};

template<typename T>
using _is_runtime_error_t = decltype(T::IsRuntimeError);

template<typename T, bool = boost::is_detected_convertible<bool, _is_runtime_error_t, T>::value>
struct is_runtime_error_impl : std::integral_constant<bool, T::IsRuntimeError> {};

template<typename T>
struct is_runtime_error_impl<T, false> : std::false_type {};

}
    
template<class T>
struct is_logic_error : detail::is_logic_error_impl<T> {};

template<class T>
struct is_runtime_error : detail::is_runtime_error_impl<T> {};

} // namespace Traits

#ifdef ADDLE_DEBUG

[[noreturn]] inline void _addle_throw_helper(AddleException& ex, const char* func, const char* file, int line)
{
    ex.debugRaise(func, file, line);
}

[[noreturn]] inline void _addle_throw_helper(const AddleException& ex, const char* func, const char* file, int line)
{
    std::unique_ptr<AddleException> ex_(ex.clone());
    ex_->debugRaise(func, file, line);
}

[[noreturn]] inline void _addle_throw_helper(AddleException&& ex, const char* func, const char* file, int line)
{
    ex.debugRaise(func, file, line);
}

[[noreturn]] inline void _addle_throw_helper(const AddleException&& ex, const char* func, const char* file, int line)
{
    std::unique_ptr<AddleException> ex_(ex.clone());
    ex_->debugRaise(func, file, line);
}

/**
 * @def 
 * Throws the AddleException ex as its most derived polymorphic type.
 * (Attaches debug information in debug builds.)
 */
# define ADDLE_THROW(ex) \
::Addle::_addle_throw_helper(ex, Q_FUNC_INFO, __FILE__, __LINE__ )

#else
# if defined(Q_CC_GNU)

template<typename T>
[[noreturn]] inline void _addle_throw_helper(T&& ex)
{
    {
        (static_cast<boost::copy_cv_ref_t<AddleException, T&&>>(ex), std::forward<T>(ex)).raise();
    }
    Q_UNREACHABLE();
    // G++ warns about non-returning code paths despite [[noreturn]] on raise()
}

# else // release mode for compilers besides G++

template<typename T>
[[noreturn]] inline void _addle_throw_helper(T&& ex)
{
    (static_cast<boost::copy_cv_ref_t<AddleException, T&&>>(ex), std::forward<T>(ex)).raise();
}

# endif

#define ADDLE_THROW(ex) \
::Addle::_addle_throw_helper(ex)

#endif

} // namespace Addle

Q_DECLARE_METATYPE(QSharedPointer<Addle::AddleException>);

#endif // ADDLEEXCEPTION_HPP
