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

#include "utilities/configuration/registerqmetatypes.hpp"
#include "utilities/configuration/servicelocatorcontrol.hpp"
#include "utilities/qobject.hpp"

#include "interfaces/services/iapplicationsservice.hpp"

#include "globals.hpp"
#include "utils.hpp"

#include "core/presenters/tools/navigatetoolpresenter.hpp"

#ifdef ADDLE_DEBUG
#include "utilities/debugging/messagehandler.hpp"
#endif

extern "C" void addle_core_config();
extern "C" void addle_widgetsgui_config();

using namespace Addle;

int main(int argc, char *argv[])
{
#ifdef ADDLE_DEBUG
#endif //ADDLE_DEBUG
    registerQMetaTypes();

    QApplication a(argc, argv);
    a.setApplicationName(ADDLE_NAME);
    a.setApplicationVersion(ADDLE_VERSION);

    QTranslator fallbackTranslator;
    QTranslator translator;

    fallbackTranslator.load(":/l10n/en_US.qm");
    a.installTranslator(&fallbackTranslator);

    if (!QLocale().uiLanguages().contains("en_US"))
    {
        translator.load(QLocale(), QString(), QString(), ":/l10n", ".qm");
        a.installTranslator(&translator);
    }

    a.setApplicationDisplayName(qtTrId(ADDLE_NAME_TRID));

#ifdef ADDLE_DEBUG
    //% "Starting Addle. This is a debug build."
    qDebug() << qUtf8Printable(qtTrId("debug-messages.starting-addle"));
#endif

    addle_core_config();
    addle_widgetsgui_config();

#ifdef ADDLE_DEBUG
    DebugBehavior::get(); // initialize flags before installing message handler
    qInstallMessageHandler(&addleMessageHandler);
#endif
    
    IApplicationService& appService = ServiceLocator::get<IApplicationService>();

    if (appService.start())
    {
        connect_interface(&a, SIGNAL(aboutToQuit()), &appService, SLOT(quitting()), Qt::ConnectionType::DirectConnection);
        QObject::connect(&a, &QCoreApplication::aboutToQuit, &ServiceLocatorControl::destroy);
        return a.exec();
    }
    else
    {
        int exitCode = appService.exitCode();
        ServiceLocatorControl::destroy();
        return exitCode;
    }
}
