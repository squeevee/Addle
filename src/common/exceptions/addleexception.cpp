/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QtDebug>
#include "addleexception.hpp"
#include <QStringBuilder>

#ifdef ADDLE_DEBUG

#include "utilities/debugging/debugbehavior.hpp"

using namespace Addle;

AddleException::AddleException(const QString& what)
    : _what(what)
{
    updateWhat();
}

void AddleException::debugRaise(const char* function, const char* file, const int line)
{
    Location loc;
    loc.function = function;
    loc.file = file;
    loc.line = line;
    _locations.push(loc);
    updateWhat();

    if (isLogicError())
    {
        if (_locations.size() <= 1)
        {
            // Don't repeat this warning for re-thrown exceptions.
            
            qWarning() << qUtf8Printable(
                //% "A logic error occurred:\n"
                //% "%1"
                qtTrId("debug-messages.logic-error-occurred")
                    .arg(what())
            );
        }

        if (DebugBehavior::test(DebugBehavior::AbortOnLogicErrorThrown))
        {
            std::abort();
        }
    }

    raise();
}

void AddleException::updateWhat()
{
    QString what;
    if (!_locations.isEmpty())
    {
        QString locationsString;

        int i = 1;
        for (auto& loc : _locations)
        {
            locationsString = locationsString 
                //% " %1: %2 (%3:%4)\n"
                % qtTrId("debug-messages.exception-location-formatter")
                    .arg(i, 2)
                    .arg(loc.function)
                    .arg(loc.file)
                    .arg(loc.line);
            ++i;
        }

        //% "%1\n"
        //% "  thrown from:\n"
        //% "%2"
        what = qtTrId("debug-messages.exception-location-list-formatter")
            .arg(_what)
            .arg(locationsString);
    }
    else
    {
        what = _what;
    }

    _whatBytes = what.toUtf8();
}

#endif // ADDLE_DEBUG