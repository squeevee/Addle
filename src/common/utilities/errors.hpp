/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ERRORS_HPP
#define ERRORS_HPP

#include "exceptions/addleexception.hpp"
#include "unhandledexceptionrouter.hpp"

#include <cstdlib>

namespace Addle {

#ifdef ADDLE_DEBUG

DECL_LOGIC_ERROR(GenericLogicError);
class GenericLogicError : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(GenericLogicError);
public:
    GenericLogicError(const char* message = nullptr, const char* expression = nullptr)
            //% "A logic error occurred"
        : AddleException(qtTrId("debug-messages.logic-error-occurred")), _expression(expression), _message(message)
    {
    }

    virtual ~GenericLogicError() = default;

private:
    const char* const _expression;
    const char* const _message;
};

// Throws a generic logic error exception if `expression` is false.
#define ADDLE_ASSERT_M(expression, message) do { if (!static_cast<bool>(expression)) { ADDLE_THROW(GenericLogicError(message, #expression)); } } while(false);
#define ADDLE_ASSERT(expression) ADDLE_ASSERT_M(expression, nullptr)

// Reports a generic logic error as an unhandled exception and returns if `expression`
// is false.
#define ADDLE_SAFE_ASSERT_M(expression, message) do { if (!static_cast<bool>(expression)) { GenericLogicError ex(message, #expression); UnhandledExceptionRouter::report(ex); return; } } while(false);
#define ADDLE_SAFE_ASSERT(expression) ADDLE_SAFE_ASSERT_M(expression, nullptr)

// Reports a generic logic error as an unhandled exception and returns `retval`
// if `expression` is false.
#define ADDLE_SAFE_ASSRT_NONVOID_M(expression, message, retval) do { if (!static_cast<bool>(expression)) { GenericLogicError ex(message, #expression); UnhandledExceptionRouter::report(ex); return retval; } } while(false);
#define ADDLE_SAFE_ASSERT_NONVOID(expression, retval) ADDLE_SAFE_ASSRT_NONVOID_M(expression, nullptr, retval)

#define ADDLE_LOGIC_ERROR ADDLE_THROW(GenericLogicError());
#define ADDLE_LOGIC_ERROR_M(message) ADDLE_THROW(GenericLogicError(message));

#else

#define ADDLE_ASSERT_M(expression, message) do { if (!static_cast<bool>(expression)) { std::abort(); } } while(false);
#define ADDLE_ASSERT(expression) ADDLE_ASSERT_M(expression, 0)
#define ADDLE_SAFE_ASSERT_M(expression, message) ADDLE_ASSERT(expression, 0);
#define ADDLE_SAFE_ASSERT(expression) ADDLE_ASSERT(expression, 0);
#define ADDLE_SAFE_ASSRT_NONVOID_M(expression, message, retval) ADDLE_ASSERT(expression, 0);
#define ADDLE_SAFE_ASSRT_NONVOID(expression, retval) ADDLE_ASSERT(expression, 0);

#define ADDLE_LOGIC_ERROR std::abort();
#define ADDLE_LOGIC_ERROR_M(message) std::abort();

#endif

} // namespace Addle

#endif // ERRORS_HPP