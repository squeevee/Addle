#ifndef EXCEPTIONUTILS_HPP
#define EXCEPTIONUTILS_HPP

#include <type_traits>
#include <typeinfo>

#include <QStack>
#include <QString>
#include <QByteArray>

#include "common/interfaces/iaddleexception.hpp"

class BaseAddleException : public IAddleException
{
#ifdef ADDLE_DEBUG
    struct Location
    {
        const char* function;
        const char* file;
        int line;
    };
public:
    BaseAddleException(const QString& what);
#else
public:
    BaseAddleException() = default;
#endif
public:
    virtual ~BaseAddleException() = default;
#ifdef ADDLE_DEBUG
    const char* what() const noexcept { return _whatBytes.constData(); }
    [[noreturn]] void debugRaise(const char* function, const char* file, const int line);
private:
    QStack<Location> _locations;
    const QString _what;
    QByteArray _whatBytes;
    const char* _what_cstr; //for convenience when peeking in a debugger

    void updateWhat();
#endif
};

#define ADDLE_EXCEPTION_BOILERPLATE(T) \
public: \
    [[noreturn]] void raise() const { throw *this; } \
    IAddleException* clone() const { return new T(*this); } \
    const std::type_info& type_info() const { return typeid(T); } \
    bool isLogicError() const { return is_logic_error<T>::value; } \
    bool isRuntimeError() const { return is_runtime_error<T>::value; }

#endif // EXCEPTIONUTILS_HPP

