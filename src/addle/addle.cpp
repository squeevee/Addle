/**
 * Addle source code
 * Copyright 2019 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#include <QtDebug>

#include <QApplication>
#include <vector>
#include <memory>

#include "serviceconfiguration.hpp"
#include "common/utilities/configuration/registerqmetatypes.hpp"
#include "common/utilities/qt_extensions/qobject.hpp"

#include "common/interfaces/services/iapplicationsservice.hpp"

#include "common/globalconstants.hpp"

#ifdef ADDLE_DEBUG
#include "common/utilities/debugging.hpp"
#endif //ADDLE_DEBUG

int main(int argc, char *argv[])
{
#ifdef ADDLE_DEBUG
    qInstallMessageHandler(debugMessageHandler);
#endif //ADDLE_DEBUG
    registerQMetaTypes();

    QApplication a(argc, argv);
    a.setApplicationVersion(GlobalConstants::ADDLE_VERSION);

#ifdef ADDLE_DEBUG
    qDebug() << qUtf8Printable(QCoreApplication::translate(
        "main",
        "Starting Addle."
    ));
    qDebug() << qUtf8Printable(QCoreApplication::translate(
        "main",
        "This is a debug build."
    ));
    if (DebugBehavior::get())
    {
        qDebug() << qUtf8Printable(QCoreApplication::translate(
            "main",
            "Debug behavior flag(s) set"
        )) << DebugBehavior::get();
    }
#endif

    ServiceConfiguration serviceConfiguration;
    serviceConfiguration.initialize();
    
    IApplicationService& appService = ServiceLocator::get<IApplicationService>();

    if (appService.start())
    {
        connect_interface(&a, SIGNAL(aboutToQuit()), &appService, SLOT(quitting()), Qt::ConnectionType::DirectConnection);
        QObject::connect(&a, &QCoreApplication::aboutToQuit, [&] () {
            serviceConfiguration.destroy();
        });
        return a.exec();
    }
    else
    {
        int exitCode = appService.getExitCode();
        serviceConfiguration.destroy();
        return exitCode;
    }
}
