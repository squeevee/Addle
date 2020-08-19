#include <QtDebug>
#include "addleexception.hpp"
#include <QStringBuilder>

using namespace Addle;

#ifdef ADDLE_DEBUG

#include "utilities/debugging.hpp"

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

        if (DebugBehavior::test(DebugBehavior::abort_on_logic_error))
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

        for (auto& loc : _locations)
        {
            locationsString = locationsString 
                //% "%1 (%2:%3)\n"
                % qtTrId("debug-messages.exception-location-formatter")
                    .arg(loc.function)
                    .arg(loc.file)
                    .arg(loc.line);
        }

        //% "%1\n"
        //% "thrown from:\n"
        //% "%2"
        what = qtTrId("debug-messages.exception-location-list-formatter")
            .arg(what)
            .arg(locationsString);
    }
    else
    {
        what = _what;
    }

    _whatBytes = what.toUtf8();
}

#endif