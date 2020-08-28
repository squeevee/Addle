/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ADDLEEXCEPTION_HPP
#define ADDLEEXCEPTION_HPP

#include "compat.hpp"
#include <exception>
#include <type_traits>
#include <typeinfo>

#include <QStack>
#include <QString>
#include <QByteArray>

#include <QMetaType>
#include <QSharedPointer>

namespace Addle {

class ADDLE_COMMON_EXPORT AddleException : public std::exception
{
#ifdef ADDLE_DEBUG
    struct Location
    {
        const char* function;
        const char* file;
        int line;
    };
public:
    AddleException(const QString& what);
#else
public:
    AddleException() = default;
#endif
public:
    virtual ~AddleException() = default;

    virtual bool isLogicError() const = 0;
    virtual bool isRuntimeError() const = 0;
    [[noreturn]] virtual void raise() const = 0;
    virtual AddleException* clone() const = 0;

#ifdef ADDLE_DEBUG
    const char* what() const noexcept { return _whatBytes.constData(); }
    [[noreturn]] void debugRaise(const char* function, const char* file, const int line);
private:
    QStack<Location> _locations;
    const QString _what;
    QByteArray _whatBytes;

    void updateWhat();
#endif
};

#define ADDLE_EXCEPTION_BOILERPLATE(T) \
public: \
    [[noreturn]] void raise() const { throw *this; } \
    AddleException* clone() const { return new T(*this); } \
    bool isLogicError() const { return is_logic_error<T>::value; } \
    bool isRuntimeError() const { return is_runtime_error<T>::value; }

template<class T>
struct is_logic_error : std::false_type {};
#define DECL_LOGIC_ERROR(T) class T; template<> struct is_logic_error<T> : std::true_type {}; 

template<class T>
struct is_runtime_error : std::false_type {};
#define DECL_RUNTIME_ERROR(T) class T; template<> struct is_runtime_error<T> : std::true_type {}; 

#ifdef ADDLE_DEBUG
#define ADDLE_THROW(ex) \
{ \
    static_assert( \
        std::is_base_of< \
            AddleException, \
            std::remove_reference<decltype(ex)>::type \
        >::value, \
        "ADDLE_THROW may only be used for AddleException" \
    ); \
    (ex).debugRaise( Q_FUNC_INFO, __FILE__, __LINE__ ); \
}
#else
#define ADDLE_THROW(ex) \
{ \
    static_assert( \
        std::is_base_of< \
            AddleException, \
            std::remove_reference<decltype(ex)>::type \
        >::value, \
        "ADDLE_THROW may only be used for AddleException" \
    ); \
    (ex).raise(); \
}
#endif

} // namespace Addle

Q_DECLARE_METATYPE(QSharedPointer<Addle::AddleException>);

#endif // ADDLEEXCEPTION_HPP

