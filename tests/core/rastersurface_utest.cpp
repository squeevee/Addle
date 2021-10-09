#include <QtTest/QtTest>
#include <QtDebug>
#include <QObject>

#include "utilities/image/tilesurface.hpp"
#include "utilities/image/rastertile.hpp"

#include "src/core/editing/rastersurface2.hpp"

#include "utilities/config3/addleconfig.hpp"
#include "utilities/config3/factory.hpp"

using namespace Addle;

class RasterSurface_UTest : public QObject
{
    Q_OBJECT
private:
    QTranslator _fallbackTranslator;
    aux_config3::AddleConfig _addleConfig;
    
private slots:
    void initTestCase()
    {   
        _fallbackTranslator.load(":/l10n/en_US.qm");
        QCoreApplication::instance()->installTranslator(&_fallbackTranslator);
        
        _addleConfig.addInjector(
                boost::di::bind<IRasterSurface2>().to<RasterSurface2>()
            );
    }
    
    void dev_1()
    {
    }
};

QTEST_MAIN(RasterSurface_UTest)

#include "rastersurface_utest.moc"
// #include "mocks/rendering/moc_mockrenderable.cpp"
