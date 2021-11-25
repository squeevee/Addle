/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef UNHANDLEDEXCEPTION_HPP
#define UNHANDLEDEXCEPTION_HPP

#include "addleexception.hpp"
#include <memory>

namespace Addle {

class UnhandledException : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(UnhandledException)
public:    
    static constexpr bool IsLogicError = true;
    enum Severity
    {
        Normal,
        Serious,
        Trivial,
    };

    inline UnhandledException(Severity severity = Severity::Normal)
        : AddleException(
#ifdef ADDLE_DEBUG
            //% "An unhandled exception occurred."
            qtTrId("debug-messages.unhandled-exception")
#endif
        ),
        _severity(severity)
    {
    }

    inline UnhandledException(const AddleException& innerException, Severity severity = Severity::Normal)
        : UnhandledException(severity)
    {
        _addleException = QSharedPointer<AddleException>(innerException.clone());
    }

    inline UnhandledException(const std::exception& innerException, Severity severity = Severity::Normal)
        : UnhandledException(severity)
    {
        _stdException = QSharedPointer<std::exception>(new std::exception(innerException));
    }

    inline UnhandledException(const UnhandledException& other)
        : UnhandledException(other._severity)
    {
        if (other._addleException)
            _addleException = QSharedPointer<AddleException>(other._addleException->clone());
        if (other._stdException)
            _stdException = QSharedPointer<std::exception>(new std::exception(*other._stdException));
    }

    virtual ~UnhandledException() = default;

    inline bool severity() const { return _severity; }

    inline QSharedPointer<AddleException> addleException() const { return _addleException; }
    inline QSharedPointer<std::exception> stdException() const { return _stdException; }

private:
    QSharedPointer<AddleException> _addleException = nullptr;
    QSharedPointer<std::exception> _stdException = nullptr;
    Severity _severity;
};

} // namespace Addle

Q_DECLARE_METATYPE(QSharedPointer<Addle::UnhandledException>)

#endif // UNHANDLEDEXCEPTION_HPP
