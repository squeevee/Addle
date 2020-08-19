/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QApplication>
#include <QTranslator>
#include <vector>
#include <memory>

#include "serviceconfiguration.hpp"
#include "utilities/configuration/registerqmetatypes.hpp"
#include "utilities/qtextensions/qobject.hpp"

#include "interfaces/services/iapplicationsservice.hpp"

#include "globals.hpp"
#include "utils.hpp"

#include "utilities/unhandledexceptionrouter.hpp"
#include "core/presenters/tools/navigatetoolpresenter.hpp"

using namespace Addle;

int main(int argc, char *argv[])
{
#ifdef ADDLE_DEBUG
    //qInstallMessageHandler(debugMessageHandler);
#endif //ADDLE_DEBUG
    registerQMetaTypes();

    QApplication a(argc, argv);
    a.setApplicationVersion(ADDLE_VERSION);

    QTranslator fallbackTranslator;
    fallbackTranslator.load(":/l10n/fallback.qm");
    a.installTranslator(&fallbackTranslator);

    QTranslator translator;
    translator.load(QLocale(), QString(), QString(), ":/l10n", ".qm");
    a.installTranslator(&translator);

#ifdef ADDLE_DEBUG
    //% "Starting Addle. This is a debug build."
    qDebug() << qUtf8Printable(qtTrId("debug-messages.starting-addle"));
    if (DebugBehavior::get())
    {
        //% "Debug behavior flag(s) set"
        qDebug() 
            << qUtf8Printable(qtTrId("debug-messages.behavior-flags-set"))
            << DebugBehavior::get();
    }
#endif

    UnhandledExceptionRouter::initialize();

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
        int exitCode = appService.exitCode();
        serviceConfiguration.destroy();
        return exitCode;
    }
}
