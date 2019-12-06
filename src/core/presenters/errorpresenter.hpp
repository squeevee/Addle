#ifndef ERRORPRESENTER_HPP
#define ERRORPRESENTER_HPP

#include "interfaces/presenters/ierrorpresenter.hpp"

#include "utilities/initializehelper.hpp"

class ErrorPresenter : public IErrorPresenter
{
public:
    ErrorPresenter() : _initHelper(this) {}
    virtual ~ErrorPresenter() = default;

    void initialize(QString message = QString(), Severity severity = Severity::warning);

    Severity getSeverity() { _initHelper.assertInitialized(); return _severity; }
    void setSeverity(Severity severity) { _initHelper.assertInitialized(); _severity = severity; }

    QString getMessage() { _initHelper.assertInitialized(); return _message; }
    void setMessage(QString message) { _initHelper.assertInitialized(); _message = message; }

private:
    Severity _severity;
    QString _message;

    InitializeHelper<ErrorPresenter> _initHelper;
};

#endif // ERRORPRESENTER_HPP