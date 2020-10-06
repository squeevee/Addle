#include "globals.hpp"

#include <QtDebug>
#include "utilities/configuration/serviceconfig.hpp"

#include "interfaces/presenters/messages/inotificationpresenter.hpp"
#include "interfaces/presenters/messages/ifileissuepresenter.hpp"

#include "main/maineditorview.hpp"
#include "main/messages/notificationdialog.hpp"
#include "main/messages/fileissuedialog.hpp"

using namespace Addle;

extern "C" void addle_widgetsgui_config()
{
    auto config = new ServiceConfig(Modules::WidgetsGui);
    
    config->addAutoFactory<IMainEditorView, MainEditorView>();
    
    config->addAutoFactory<IMessageView, NotificationDialog>(
        ServiceConfig::Filter()
            .byArg<INotificationPresenter>([](const INotificationPresenter& p) -> bool {
                return p.isUrgent();
            })
    );
    
    config->addAutoFactory<IMessageView, FileIssueDialog>(
        ServiceConfig::Filter()
            .byArg<IFileIssuePresenter>([](const IFileIssuePresenter& p) -> bool {
                return p.isUrgent();
            })
    );
    
    config->commit();
}
