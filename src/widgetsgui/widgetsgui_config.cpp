#include "globals.hpp"

#include <QtDebug>
#include "utilities/configuration/serviceconfig.hpp"

#include "main/maineditorview.hpp"
#include "main/applicationerrorview.hpp"

using namespace Addle;

extern "C" void addle_widgetsgui_config()
{
    auto config = new ServiceConfig(Modules::WidgetsGui);
    
    config->addAutoFactory<IMainEditorView, MainEditorView>();
    config->addAutoFactory<IApplicationErrorView, ApplicationErrorView>();
    
    config->commit();
}
