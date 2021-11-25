#include "globals.hpp"

#include <QtDebug>

#include "interfaces/presenters/messages/inotificationpresenter.hpp"
#include "interfaces/presenters/messages/ifileissuepresenter.hpp"

#include "main/maineditorview.hpp"
#include "main/messages/notificationdialog.hpp"
#include "main/messages/fileissuedialog.hpp"

//#include "utilities/config/serviceconfig.hpp"

// #include "utilities/config/config.hpp"

// #include "utilities/config/injector.hpp"
// #include "utilities/config/factory.hpp"

// #include "utilities/config/injectorconfig.hpp"

using namespace Addle;

// void widgetsgui_config(InjectorConfig& config)
// {
//     static_assert(
//         std::is_same<
//             aux_view::presenter_of_t<Addle::IMainEditorView>,
//             Addle::IMainEditorPresenter
//         >::value
//     );
//     
//     
//     using namespace Config;
//     config.extend(
//         fill_deferred_binding<IMainEditorView, MainEditorView>()
//     );
// }

// boost::di::injector<IFactory<IMainEditorView>&> config_widgetsgui ()
// {   
//     return boost::di::make_injector(
//         boost::di::bind<IFactory<IMainEditorView>>().to(Addle::Config::FactoryBinding<MainEditorView>())
//     );
// }

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
