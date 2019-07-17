/**
 * Addle source code
 * Copyright 2019 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#include "views/mainwindow.h"
#include <QApplication>
#include <vector>
#include <memory>

#include "serviceconfiguration.h"
#include "utilities/registerqmetatypes.h"

int main(int argc, char *argv[])
{
    configure::registerQMetaTypes();
    configure::ServiceConfiguration serviceConfiguration;
    serviceConfiguration.initialize();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    serviceConfiguration.destroy();

    return a.exec();
}
