#ifndef APPLICATIONSERVICE_HPP
#define APPLICATIONSERVICE_HPP

#include "compat.hpp"
#include <QObject>
#include <QUrl>
#include "interfaces/services/iapplicationsservice.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT ApplicationService : public QObject, public IApplicationService
{
    Q_OBJECT
    IAMQOBJECT_IMPL
public:
    ApplicationService()
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
};

} // namespace Addle

#endif // APPLICATIONSERVICE_HPP