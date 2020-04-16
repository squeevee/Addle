#ifndef APPLICATIONSERVICE_HPP
#define APPLICATIONSERVICE_HPP

#include <QObject>
#include <QUrl>
#include "servicebase.hpp"

#include "core/compat.hpp"
#include "interfaces/services/iapplicationsservice.hpp"

#include "helpers/servicethreadhelper.hpp"

class ADDLE_CORE_EXPORT ApplicationService : public QObject, public virtual ServiceBase, public virtual IApplicationService
{
    Q_OBJECT
    Q_INTERFACES(IApplicationService)
public:
    ApplicationService() : _threadHelper(this, ServiceThreadHelper_options::ThreadOption::application_thread)
    {
    }
    virtual ~ApplicationService();

    bool start();

    StartupMode getStartupMode() { return _startupMode; }

    int getExitCode() { return _exitCode; }

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

#endif // APPLICATIONSERVICE_HPP