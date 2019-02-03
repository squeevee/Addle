#include "views/mainwindow.h"
#include <QApplication>
#include <vector>
#include <memory>

#include "interfaces/services/icanvasservice.h"
#include "services/canvasservice.h"

#include "configure/serviceregistry.h"
#include "configure/configureserviceregistry.h"


int main(int argc, char *argv[])
{
    configure::ConfigureServiceRegistry::initialize();
    REGISTER_SERVICE(ICanvasService, new CanvasService());

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    configure::ConfigureServiceRegistry::destroy();

    return a.exec();
}
