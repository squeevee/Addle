/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#include <QApplication>
#include <QTranslator>
#include <vector>
#include <memory>

#include "utilities/config/registerqmetatypes.hpp"
#include "utilities/qobject.hpp"

#include "interfaces/services/iapplicationservice.hpp"

#include "globals.hpp"
#include "utils.hpp"

#include "core/presenters/tools/navigatetoolpresenter.hpp"

#ifdef ADDLE_DEBUG
#include "utilities/debugging/messagehandler.hpp"
#endif

#include "utilities/config/injectorconfig.hpp"

using namespace Addle;

extern void widgetsgui_config(InjectorConfig& config);

int main(int argc, char *argv[])
{   
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
    
    DebugBehavior::get(); // initialize flags before installing message handler
    qInstallMessageHandler(&addleMessageHandler);
#endif
    
    auto config = core_config();
    widgetsgui_config(*config);
    
    auto& appService = config->getSingleton<IApplicationService>();
        
    
    if (appService.start())
    {
        connect_interface(&a, SIGNAL(aboutToQuit()), &appService, SLOT(quitting()), Qt::ConnectionType::DirectConnection);
        return a.exec();
    }
    else
    {
        int exitCode = appService.exitCode();
        return exitCode;
    }
}
