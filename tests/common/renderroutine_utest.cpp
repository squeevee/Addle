#include <QtTest/QtTest>
#include <QtDebug>
#include <QObject>
#include <QThread>
#include <QtConcurrent/QtConcurrent>

#include <QFutureWatcher>

#include "utilities/render/renderroutine.hpp"
#include "interfaces/rendering/irenderentity.hpp"
#include "interfaces/rendering/irenderable.hpp"

#include "mocks/rendering/mockrenderable.hpp"
#include "mocks/rendering/mockrenderentity.hpp"

using namespace Addle;

class RenderRoutine_UTest : public QObject
{
    Q_OBJECT
private:
    QTranslator fallbackTranslator;
    
private slots:
    void initTestCase()
    {   
        fallbackTranslator.load(":/l10n/en_US.qm");
        QCoreApplication::instance()->installTranslator(&fallbackTranslator);
    }
    
    void dev_1()
    {
    }
    
    void benchmark_populate()
    {
    }
};

QTEST_MAIN(RenderRoutine_UTest)

#include "renderroutine_utest.moc"
#include "mocks/rendering/moc_mockrenderable.cpp"
