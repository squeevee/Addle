#ifndef ERRORPRESENTER_HPP
#define ERRORPRESENTER_HPP

#include "compat.hpp"
#include "interfaces/presenters/ierrorpresenter.hpp"

#include "utilities/initializehelper.hpp"
namespace Addle {

class ADDLE_CORE_EXPORT ErrorPresenter : public IErrorPresenter
{
public:
    ErrorPresenter() : _initHelper(this) {}
    virtual ~ErrorPresenter() = default;

    void initialize(QString message = QString(), Severity severity = Severity::warning);

    Severity getSeverity() { _initHelper.check(); return _severity; }
    void setSeverity(Severity severity) { _initHelper.check(); _severity = severity; }

    QString getMessage() { _initHelper.check(); return _message; }
    void setMessage(QString message) { _initHelper.check(); _message = message; }

private:
    Severity _severity;
    QString _message;

    InitializeHelper<ErrorPresenter> _initHelper;
};

} // namespace Addle
#endif // ERRORPRESENTER_HPP