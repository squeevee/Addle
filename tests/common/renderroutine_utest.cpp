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

#include "utilities/image/tilesurface.hpp"
#include "utilities/image/rastertile.hpp"

#include "utilities/multi_array_util.hpp"

static QRegion fromCorner(QPoint corner)
{
    QThread::msleep(10);
    return QRect(corner, QSize(100, 100));
}

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
        RasterTile r(QImage::Format_ARGB32, QPoint(0, 0));
        r.buffer();
        
        //TileSurface s1;
        
//         for (auto&& p : s1.paintTiles(QRect(-100, -100, 200, 200)))
//         {
//             p.setBrush(Qt::blue);
//             p.setPen(Qt::NoPen);
//             p.setRenderHint(QPainter::Antialiasing);
//             p.drawEllipse(QPoint(0, 0), 100, 100);
//         }
//         
//         s1.clear();
//         
//         auto n = 0;
//         for (const auto& tile : aux_multi_array_util::traverse_rows(s1.tiles()))
//         {
//             if (!tile.isNull())
//             {
//                 qDebug() << tile.offset();
//                 //tile.asImage().save(QStringLiteral("tile-1-%1.png").arg(n, 3, 10, QChar('0')));
//             }
//             ++n;
//         }
//         
        //qDebug() << b.allocated();
        
        
//         auto b = aux_render::RoutineBuilder();
//         
//         int x = 0;
//         for (int i = 0; i < 2000; ++i)
//         {
//             auto sName = QByteArrayLiteral("subroutine-") + QByteArray::number(i);
//             auto s = aux_render::RoutineBuilder().setName(sName);
//             
//             for (int j = 0; j < 1; ++j)
//             {
//                 auto eName = sName + QByteArrayLiteral("-entity-") + QByteArray::number(j);
//                 
//                 auto e = QSharedPointer<MockRenderEntity>::create(eName);
//                 e->_region = QRect(x, 0, 1, 1000);
//                 s.addEntity(j, std::move(e));
//                 
//                 ++x;
//             }
//             
//             b.addSubRoutine(i, std::move(s));
//         }
//             
//         auto renderable = std::make_unique<MockRenderable>();
//         
//         renderable->_renderRoutine = std::move(b);
//             
//         noDetach(renderable->renderRoutine()._data)->setRenderable_p(renderable.get());
//         
//         qDebug() << renderable->renderRoutine().region({500});
//         qDebug() << renderable->renderRoutine().region();
    }
    
    void benchmark_populate()
    {
        TileSurface s1;
        QBENCHMARK {
            s1.paintTiles(QRect(0, 0, 1280, 720));
            s1.clear();
        }
    }
};

QTEST_MAIN(RenderRoutine_UTest)

#include "renderroutine_utest.moc"
#include "mocks/rendering/moc_mockrenderable.cpp"
