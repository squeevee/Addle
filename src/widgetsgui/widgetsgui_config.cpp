#include "globals.hpp"

#include <QtDebug>

#include "interfaces/presenters/messages/inotificationpresenter.hpp"
#include "interfaces/presenters/messages/ifileissuepresenter.hpp"

#include "main/maineditorview.hpp"
#include "main/messages/notificationdialog.hpp"
#include "main/messages/fileissuedialog.hpp"

//#include "utilities/config/serviceconfig.hpp"

// #include "utilities/config/config.hpp"

// namespace Addle { namespace Config {
// 
// template<>
// BindingSet* get_module_binding<STATIC_ID_WRAPPER(Modules::WidgetsGui)>()
// {
//     auto config = new BindingSet();
//     return config;
// }
//     
// }}

// using namespace Addle;

// extern "C" void addle_widgetsgui_config(ServiceContainer& container)
// {
//     auto config = new ServiceConfig(container, Modules::WidgetsGui);
//     
//     config->addFactory<IMainEditorView, MainEditorView>();
//     config->addFactory<IMessageView, NotificationDialog>();
    
//     config->addAutoFactory<IMainEditorView, MainEditorView>();
//     
//     config->addAutoFactory<IMessageView, NotificationDialog>(
//         ServiceConfig::Filter()
//             .byArg<INotificationPresenter>([](const INotificationPresenter& p) -> bool {
//                 return p.isUrgent();
//             })
//     );
//     
//     config->addAutoFactory<IMessageView, FileIssueDialog>(
//         ServiceConfig::Filter()
//             .byArg<IFileIssuePresenter>([](const IFileIssuePresenter& p) -> bool {
//                 return p.isUrgent();
//             })
//     );
    
//     config->commit();
// }
