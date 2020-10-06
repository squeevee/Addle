#ifndef NOTIMPLEMENTEDEXCEPTION_HPP
#define NOTIMPLEMENTEDEXCEPTION_HPP

#include "addleexception.hpp"

namespace Addle {
    
DECL_LOGIC_ERROR(NotImplementedException);
class NotImplementedException : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(NotImplementedException)
public:
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
