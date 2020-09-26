#include "globals.hpp"

#include <QtDebug>
#include "utilities/configuration/serviceconfig.hpp"

#include "interfaces/presenters/messages/inotificationpresenter.hpp"

#include "main/maineditorview.hpp"
//#include "main/applicationerrorview.hpp"
#include "main/messages/notificationdialog.hpp"

using namespace Addle;

extern "C" void addle_widgetsgui_config()
{
    auto config = new ServiceConfig(Modules::WidgetsGui);
    
    config->addAutoFactory<IMainEditorView, MainEditorView>();
    //config->addAutoFactory<IApplicationErrorView, ApplicationErrorView>();
    
    config->addAutoFactory<IMessageView, NotificationDialog>(
        ServiceConfig::Filter()
            .byArg<INotificationPresenter>([](const INotificationPresenter& p) -> bool {
                return p.isUrgent();
            })
    );
    
    config->commit();
}
