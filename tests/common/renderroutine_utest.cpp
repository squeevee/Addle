#include <QtTest/QtTest>
#include <QtDebug>
#include <QObject>

#include "utilities/render/renderroutine.hpp"
#include "interfaces/rendering/irenderentity.hpp"

using namespace Addle;

class TestEntity : public IRenderEntity
{
public:
    virtual ~TestEntity() = default;
    QUuid id() const override { return QUuid::createUuidV3( QUuid(), QByteArrayLiteral("spiff")); }
}; 

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
        QSharedPointer<IRenderEntity> e(new TestEntity);
       
        RenderRoutine r1, r2;
        
        r2.setId( QUuid::createUuidV3( QUuid(), QByteArrayLiteral("freem")) );
        r2.addEntity(e, aux_render::EntityFlag_Draws, -100);
        r1.addSubRoutine(r2); 
        
        
        RenderRoutineChangedEvent ev;
        ev.addNodeChunks({ {{ 0 },1}, {{ 0, 0 }, 1} });
        ev.setZValues({ 0 }, { 10, 9, 8 });
        //ev.populateZValues(r1);
        
        for (auto z : ev.zValues())
            qDebug() << z;
        
        for (auto z : ev.zValues({ 0 }))
            qDebug() << z;
    }
};

QTEST_MAIN(RenderRoutine_UTest)

#include "renderroutine_utest.moc"
