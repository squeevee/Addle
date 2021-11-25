#ifndef NOTIMPLEMENTEDEXCEPTION_HPP
#define NOTIMPLEMENTEDEXCEPTION_HPP

#include "addleexception.hpp"

namespace Addle {
    
class NotImplementedException : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(NotImplementedException)
public:
    static constexpr bool IsLogicError = true;
    NotImplementedException(QString note = QString())
#ifdef ADDLE_DEBUG
        : AddleException([=]() -> QString {
            return note.isEmpty() ?
                //% "This feature is not yet implemented."
                qtTrId("debug-message.not-implemented-excpetion.no-note")
                //% "The feature \"%1\" is not yet implemented."
                : qtTrId("debug-messages.not-implemented-exception.note")
                    .arg(note);
        }())
#endif
    {
        Q_UNUSED(note);
    }
};
    
} // namepace Addle

#endif // NOTIMPLEMENTEDEXCEPTION_HPP
