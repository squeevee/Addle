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
#include <QEvent>

#include "compat.hpp"

#include "interfaces/services/ierrorservice.hpp"

#include "exceptions/addleexception.hpp"
#include "exceptions/unhandledexception.hpp"

#include "servicelocator.hpp"

namespace Addle {

#ifdef ADDLE_DEBUG

/**
 * @brief A general-purpose AddleException for logic errors.
 */
class ADDLE_COMMON_EXPORT GenericLogicError : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(GenericLogicError);
public:
    static constexpr bool IsLogicError = true;
    GenericLogicError(const char* expression = nullptr, QString message = QString());
    virtual ~GenericLogicError() = default;

private:
    const char* const _expression;
    QString _message;
};

/**
 * @def
 * @brief Asserts that the given expression resolves to true.
 * 
 * If the assertion fails, a logic error AddleException with the given message
 * is thrown.
 */
#define ADDLE_ASSERT_M(expression, message) \
if (Q_UNLIKELY(!static_cast<bool>(expression))) { ADDLE_THROW(GenericLogicError(#expression, message)); }

/**
 * @def
 * @brief Asserts that the given expression resolves to true.
 * 
 * If the assertion fails, a logic error AddleException is thrown.
 */
#define ADDLE_ASSERT(expression) \
ADDLE_ASSERT_M(expression, QString())

/**
 * @def
 * Indiscriminately throws a logic error AddleException with the given message.
 */
#define ADDLE_LOGIC_ERROR_M(message) \
ADDLE_THROW(GenericLogicError(nullptr, message))

/**
 * @def
 * @brief Denotes a code path that is logically unreachable.
 * 
 * In debug builds this throws a logic error. In release builds, this is
 * equivalent to Q_UNREACHABLE().
 */
//% "A code path that was designated logically unreachable was reached."
#define ADDLE_LOGICALLY_UNREACHABLE() ADDLE_THROW(GenericLogicError(nullptr, qtTrId("debug-messages.bad-logically-unreachable")))

//% "An exception of an unknown type was caught."
#define _LAST_DITCH_CATCH(x) ServiceLocator::get<IErrorService>().reportUnhandledError(GenericLogicError(nullptr, qtTrId("debug-messages.last-ditch-catch")), x);

[[noreturn]] void ADDLE_COMMON_EXPORT _cannotReportError_impl(const std::exception* primaryEx);

#define ADDLE_SLOT_CATCH_SEVERITY(x) \
catch (const AddleException& ex) \
{ \
    if (static_cast<bool>(sender())) \
    { \
        try { ServiceLocator::get<IErrorService>().reportUnhandledError(ex, x); } \
        catch(...) { _cannotReportError_impl(&ex); } \
    } \
    else throw; \
} \
catch (const std::exception& ex) \
{ \
    if (static_cast<bool>(sender())) \
    { \
        try { ServiceLocator::get<IErrorService>().reportUnhandledError(ex, x); } \
        catch(...) { _cannotReportError_impl(&ex); } \
    } \
    else throw; \
} \
catch(...) \
{ \
    if (static_cast<bool>(sender())) \
    { \
        try { _LAST_DITCH_CATCH(x) } \
        catch(...) { _cannotReportError_impl(nullptr); } \
    } \
    else throw; \
}

#define ADDLE_MISC_CATCH_SEVERITY(x, y) \
catch (const AddleException& ex) \
{ \
    { y } \
    try { ServiceLocator::get<IErrorService>().reportUnhandledError(ex, x); } \
    catch(...) { _cannotReportError_impl(&ex); } \
} \
catch (const std::exception& ex) \
{ \
    { y } \
    try { ServiceLocator::get<IErrorService>().reportUnhandledError(ex, x); } \
    catch(...) { _cannotReportError_impl(&ex); } \
} \
catch(...) \
{ \
    { y } \
    try { _LAST_DITCH_CATCH(x) } \
    catch(...) { _cannotReportError_impl(nullptr); } \
}

#else // ADDLE_DEBUG

class ADDLE_COMMON_EXPORT GenericLogicError : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(GenericLogicError);
public:
    virtual ~GenericLogicError() = default;
};

#define ADDLE_ASSERT_M(expression, message) if (Q_UNLIKELY(!static_cast<bool>(expression))) { ADDLE_THROW(GenericLogicError()); }
#define ADDLE_ASSERT(expression) ADDLE_ASSERT_M(expression, NULL)

#define ADDLE_LOGIC_ERROR_M(message) ADDLE_THROW(GenericLogicError())

#define ADDLE_LOGICALLY_UNREACHABLE() Q_UNREACHABLE()

#define ADDLE_SLOT_CATCH_SEVERITY(x) \
catch(...) \
{ \
    if (static_cast<bool>(sender())) \
    { \
        try { ServiceLocator::get<IErrorService>().reportUnhandledError(GenericLogicError(), x); } \
        catch(...) { std::terminate(); } \
    } \
    else throw; \
}

#define ADDLE_MISC_CATCH_SEVERITY(x, y) \
catch(...) \
{ \
    { y } \
    try { ServiceLocator::get<IErrorService>().reportUnhandledError(GenericLogicError(), x); } \
    catch(...) { std::terminate(); } \
}

#endif //ADDLE_DEBUG

/**
 * @def
 * @brief A catch umbrella for use in the body of a Qt slot.
 * 
 * If the slot is invoked by the Qt event system, this will quietly report
 * exceptions to IErrorService. If the slot was called directly, the exceptions
 * will be propagated to the caller.
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
 * 
 * A corresponding ADDLE_..._CATCH_SEVERITY macro is available for this and the
 * other similar macros, in case a different severity level besieds Normal is
 * desired.
 */
#define ADDLE_SLOT_CATCH ADDLE_SLOT_CATCH_SEVERITY(UnhandledException::Normal)

#define ADDLE_EVENT_CATCH_SEVERITY(x, y) \
ADDLE_MISC_CATCH_SEVERITY(x, static_cast<QEvent*>(y)->ignore();)

/**
 * @def
 * Similar to ADDLE_SLOT_CATCH. For use in the body of QEvent handlers. This
 * will mark the given event as ignored and never propagate exceptions to the
 * caller.
 */
#define ADDLE_EVENT_CATCH(event) ADDLE_EVENT_CATCH_SEVERITY(UnhandledException::Normal, event)

/**
 * @def
 * Similar to ADDLE_SLOT_CATCH and ADDLE_EVENT_CATCH. For use in other settings
 * that may be called from the Qt event loop, such as Q_INVOKABLE 
 * or QRunnable. It will never propagate exceptions to the caller.
 * 
 * The second parameter of ADDLE_MISC_CATCH_SEVERITY is a section of code that
 * will be injected into the catch block before the error is reported to
 * IErrorService.
 *
 * Use this with caution as 1) the local scope where this is injected  varies
 * between exception types as well as between debug and release builds, and
 * 2) the preprocessor can have trouble parsing statements that contain commas.
 * The conventional "no-op" `((void)0);` may be used if no response is desired.
 */
#define ADDLE_MISC_CATCH ADDLE_MISC_CATCH_SEVERITY(UnhandledException::Normal, ((void)0); )

} // namespace Addle

#endif // ERRORS_HPP
