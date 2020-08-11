#ifndef APPLICATIONSERVICE_HPP
#define APPLICATIONSERVICE_HPP

#include "compat.hpp"
#include <QObject>
#include <QUrl>
#include "servicebase.hpp"

#include "interfaces/services/iapplicationsservice.hpp"

#include "helpers/servicethreadhelper.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT ApplicationService : public QObject, public virtual ServiceBase, public virtual IApplicationService
{
    Q_OBJECT
public:
    ApplicationService() : _threadHelper(this, ServiceThreadHelper_options::ThreadOption::application_thread)
    {
    }
    virtual ~ApplicationService();

    bool start();

    StartupMode startupMode() { return _startupMode; }

    int exitCode() { return _exitCode; }

public slots:
    void quitting();

private:
    void parseCommandLine();
    void startGraphicalApplication();

    QUrl _startingUrl;
    QString _startingFilename;

    StartupMode _startupMode;
    int _exitCode;

    ServiceThreadHelper<ApplicationService> _threadHelper;
};

} // namespace Addle

#endif // APPLICATIONSERVICE_HPP