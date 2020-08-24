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

GenericLogicError::GenericLogicError(const char* message, const char* expression)
    : AddleException([message, expression]() -> QString {
        if (message && expression)
        {
            //% "%1\n"
            //% "Assertion failed: `%2`"
            return qtTrId("debug-messages.assert-failed-m")
                .arg(qtTrId(message))
                .arg(expression);
        }
        else if (!message && expression)
        {
            //% "Assertion failed: `%1`"
            return qtTrId("debug-messages.assert-failed")
                .arg(expression);
        }
        else if (message && !expression)
        {
            return qtTrId(message);
        }
        else // !message && !expression
        {
            //% "(No message was given)"
            return qtTrId("debug-messages.generic-logic-error");
        }
    }()), _expression(expression), _message(message)
{
}

#endif // ADDLE_DEBUG
