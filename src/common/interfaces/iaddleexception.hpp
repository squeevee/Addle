#ifndef IADDLEEXCEPTION_HPP
#define IADDLEEXCEPTION_HPP

#include <exception>
#include <QSharedPointer>

namespace Addle {

class IAddleException : public std::exception
{
public:
    virtual ~IAddleException() = default;
    virtual const std::type_info& type_info() const = 0;
    virtual bool isLogicError() const = 0;
    virtual bool isRuntimeError() const = 0;
    [[noreturn]] virtual void raise() const = 0;
    virtual IAddleException* clone() const = 0;

#ifdef ADDLE_DEBUG
    [[noreturn]] virtual void debugRaise(const char* function, const char* file, const int line) = 0;
#endif
};

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
            IAddleException, \
            std::remove_reference<decltype(ex)>::type \
        >::value, \
        "ADDLE_THROW may only be used for IAddleException" \
    ); \
    (ex).debugRaise( Q_FUNC_INFO, __FILE__, __LINE__ ); \
}
#else
#define ADDLE_THROW(ex) \
{ \
    static_assert( \
        std::is_base_of< \
            IAddleException, \
            std::remove_reference<decltype(ex)>::type \
        >::value, \
        "ADDLE_THROW may only be used for IAddleException" \
    ); \
    (ex).raise(); \
}
#endif

// Q_DECLARE_METATYPE(QSharedPointer<IAddleException>);

} // namespace Addle

#endif // IADDLEEXCEPTION_HPP