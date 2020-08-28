/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <cstdlib>
#include <iostream>

#include <QtGlobal>

#include "compat.hpp"

#include "interfaces/services/ierrorservice.hpp"

#include "exceptions/addleexception.hpp"
#include "exceptions/unhandledexception.hpp"

#include "servicelocator.hpp"

namespace Addle {

DECL_LOGIC_ERROR(GenericLogicError);

#ifdef ADDLE_DEBUG

/**
 * A general-purpose AddleException for logic errors.
 */
class ADDLE_COMMON_EXPORT GenericLogicError : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(GenericLogicError);
public:
    GenericLogicError(const char* expression = nullptr, QString message = QString());
    virtual ~GenericLogicError() = default;

private:
    const char* const _expression;
    QString _message;
};

/**
 * @def ADDLE_ASSERT_M(expression, message)
 * Asserts that the given expression resolves to true.
 * If the assertion fails, a logic error AddleException with the given message
 * is thrown.
 */
#define ADDLE_ASSERT_M(expression, message) \
if (!static_cast<bool>(expression)) { ADDLE_THROW(GenericLogicError(#expression, message)); }

/**
 * @def ADDLE_ASSERT(expression)
 * Asserts that the given expression resolves to true.
 * If the assertion fails, a logic error AddleException is thrown.
 */
#define ADDLE_ASSERT(expression) \
ADDLE_ASSERT_M(expression, QString())

/**
 * @def ADDLE_LOGIC_ERROR_M(message)
 * Indiscriminately throws a logic error AddleException with the given message.
 */
#define ADDLE_LOGIC_ERROR_M(message) \
ADDLE_THROW(GenericLogicError(nullptr, message));

//% "An exception of an unknown type was caught."
#define _LAST_DITCH_CATCH(x) ServiceLocator::get<IErrorService>().reportUnhandledError(GenericLogicError(nullptr, qtTrId("debug-messages.last-ditch-catch")));

[[noreturn]] void ADDLE_COMMON_EXPORT _cannotReportError_impl(const std::exception* primaryEx);

#define _CANNOT_REPORT_ERROR_1 _cannotReportError_impl(&ex);
#define _CANNOT_REPORT_ERROR_2 _cannotReportError_impl(nullptr);

#else // ADDLE_DEBUG

class ADDLE_COMMON_EXPORT GenericLogicError : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(GenericLogicError);
public:
    virtual ~GenericLogicError() = default;
};

#define ADDLE_ASSERT_M(expression, message) if (!static_cast<bool>(expression)) { ADDLE_THROW(GenericLogicError()); }
#define ADDLE_ASSERT(expression) ADDLE_ASSERT_M(expression, NULL)

#define ADDLE_LOGIC_ERROR_M(message) ADDLE_THROW(GenericLogicError());

#define _LAST_DITCH_CATCH(x) ServiceLocator::get<IErrorService>().reportUnhandledError(GenericLogicError());

#define _CANNOT_REPORT_ERROR_1 std::abort();
#define _CANNOT_REPORT_ERROR_2 std::abort();

#endif //ADDLE_DEBUG


#define ADDLE_SLOT_CATCH_SEVERITY(x) \
catch (const AddleException& ex) \
{ \
    if (static_cast<bool>(sender())) \
    { \
        try { ServiceLocator::get<IErrorService>().reportUnhandledError(ex, x); } \
        catch(...) { _CANNOT_REPORT_ERROR_1 } \
    } \
    else throw; \
} \
catch (const std::exception& ex) \
{ \
    if (static_cast<bool>(sender())) \
    { \
        try { ServiceLocator::get<IErrorService>().reportUnhandledError(ex, x); } \
        catch(...) { _CANNOT_REPORT_ERROR_1 } \
    } \
    else throw; \
} \
catch(...) \
{ \
    if (static_cast<bool>(sender())) \
    { \
        try { _LAST_DITCH_CATCH(x) } \
        catch(...) { _CANNOT_REPORT_ERROR_2 } \
    } \
    else throw; \
}

/**
 * @def ADDLE_SLOT_CATCH
 * A catch umbrella for use in the body of a Qt slot. If the slot is invoked by
 * the Qt event system, this will quietly report exceptions to IErrorService. If
 * the slot was called directly, the exceptions will be propagated to the 
 * caller.
 * 
 * Example:
 * ```c++
 * void Spiff::freemSlot()
 * {
 *     try
 *     {
 *        // ...
 *     }
 *     ADDLE_SLOT_CATCH
 * }
 * ```
 */
#define ADDLE_SLOT_CATCH ADDLE_SLOT_CATCH_SEVERITY(UnhandledException::Normal)

#define ADDLE_EVENT_CATCH_SEVERITY(x) \
catch (const AddleException& ex) \
{ \
    try { ServiceLocator::get<IErrorService>().reportUnhandledError(ex, x); } \
    catch(...) { _CANNOT_REPORT_ERROR_1 } \
} \
catch (const std::exception& ex) \
{ \
    try { ServiceLocator::get<IErrorService>().reportUnhandledError(ex, x); } \
    catch(...) { _CANNOT_REPORT_ERROR_1 } \
} \
catch(...) \
{ \
    try { _LAST_DITCH_CATCH(x) } \
    catch(...) { _CANNOT_REPORT_ERROR_2 } \
}

/**
 * @def ADDLE_EVENT_CATCH
 * Similar to ADDLE_SLOT_CATCH. For use in the body of methods that aren't slots 
 * but may be invoked by the Qt event system, such as `QObject::event`,
 * `QRunnable::run` or a `Q_INVOKABLE` method. This will never propagate
 * exceptions to the caller.
 */
#define ADDLE_EVENT_CATCH ADDLE_EVENT_CATCH_SEVERITY(UnhandledException::Normal)

} // namespace Addle

#endif // ERRORS_HPP