/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "errors.hpp"

using namespace Addle;

#ifdef ADDLE_DEBUG

#include <QtDebug>

GenericLogicError::GenericLogicError(const char* expression, QString message)
    : AddleException([message, expression]() -> QString {
        if (!message.isEmpty() && expression)
        {
            //% "%1\n"
            //% "Assertion failed: `%2`"
            return qtTrId("debug-messages.assert-failed-m")
                .arg(message)
                .arg(expression);
        }
        else if (message.isEmpty() && expression)
        {
            //% "Assertion failed: `%1`"
            return qtTrId("debug-messages.assert-failed")
                .arg(expression);
        }
        else if (!message.isEmpty() && !expression)
        {
            return message;
        }
        else // !message && !expression
        {
            //% "(No message was given)"
            return qtTrId("debug-messages.generic-logic-error");
        }
    }()), _expression(expression), _message(message)
{
}

void Addle::_cannotReportError_impl(const std::exception* primaryEx)
{
    //% "An error (primary) occurred. While attempting to report it, another error (secondary) occurred."
    qWarning() << qUtf8Printable(qtTrId("debug-messages.cannot-report-error"));
    if (primaryEx)
    {
        //% "  The primary error:"
        qWarning() << qUtf8Printable(qtTrId("debug-messages.cannot-report-error.primary-error-header"));
        qWarning() << primaryEx->what();
    }
    else
    {
        //% "The primary error was not of type std::exception"
        qWarning() << qUtf8Printable(qtTrId("debug-messages.cannot-report-error.no-primary-error"));
    }
    
    try
    {
        auto secondaryExPtr = std::current_exception();
        if (secondaryExPtr)
            std::rethrow_exception(secondaryExPtr);
    }
    catch(const std::exception& secondaryEx)
    {
        //% "  The secondary error:"
        qWarning() << qUtf8Printable(qtTrId("debug-messages.cannot-report-error.secondary-error-header"));
        qWarning() << secondaryEx.what();
    }
    catch(...)
    {
        //% "The secondary error was not of type std::exception"
        qWarning() << qUtf8Printable(qtTrId("debug-messages.cannot-report-error.no-secondary-error"));
    }

    //% "This is an unrecoverable state. The application will terminate immediately."
    qWarning() << qUtf8Printable(qtTrId("debug-messages.cannot-report-error.terminating"));

    std::terminate();
}

#endif // ADDLE_DEBUG
