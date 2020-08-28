/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef GENERICERRORPRESENTER_HPP
#define GENERICERRORPRESENTER_HPP

#include "compat.hpp"
#include "interfaces/presenters/errors/ierrorpresenter.hpp"

namespace Addle {

class ADDLE_COMMON_EXPORT GenericErrorPresenter : public IErrorPresenter
{
public:
    GenericErrorPresenter() = default;
    GenericErrorPresenter(QString message, Severity severity = Severity::Warning)
        : _severity(severity), _message(message)
    {
    }
    virtual ~GenericErrorPresenter() = default;

    Severity severity() const { return _severity; }
    void setSeverity(Severity severity) { _severity = severity; }
    
    QString message() const { return _message; }
    void setMessage(QString message) { _message = message; }

    QSharedPointer<AddleException> exception() const { return _exception; }
    void setException(QSharedPointer<AddleException> exception) { _exception = exception; }

private:
    Severity _severity = Severity::Warning;
    QString _message;

    QSharedPointer<AddleException> _exception;
};

} // namespace Addle

#endif // GENERICERRORPRESENTER_HPP
