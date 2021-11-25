#include <QtTest/QtTest>
#include <QtDebug>
#include <QObject>

#include "utilities/image/rastertile.hpp"
#include "utilities/render/renderroutine.hpp"

#include "src/core/editing/rastersurface2.hpp"

#include "utilities/config3/addleconfig.hpp"
#include "utilities/config3/factory.hpp"
#include "utilities/multi_array_util.hpp"


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
        auto factory = aux_config3::Factory<IRasterSurface2>(_addleConfig);
        std::unique_ptr<RasterSurface2> surface(dynamic_cast<RasterSurface2*>(factory.make()));
        assert(surface);
        
        
        
//         surface->paintTiles(QRect(0,0,100,100));
        auto q = 0;
        for (auto&& painter : surface->paintTiles(QRect(-100, -100, 200, 200)))
        {
            ++q;
            
//             if (q == 1 || q == 4)
                painter.setBrush(Qt::lightGray);
//             else
//                 painter.setBrush(Qt::white);
            
            painter.setPen(Qt::NoPen);
            painter.setRenderHint(QPainter::RenderHint::Antialiasing);
            painter.drawEllipse(QPoint(0, 0), 100, 100);
        }
        
        qDebug() << surface->region();
        QVarLengthArray<const aux_render::Entity*> entities;
        auto r = surface->renderRoutine();
        for (auto c : r.depthFirstSearch())
        {
            if (c.isEntity()) 
            {
                entities.append(&c.asEntity());
            }
        }
        
        {
            QImage dest(1024, 1024, QImage::Format_ARGB32);
            
            QPainter painter(&dest);
            QTransform t;
            
            t.scale(4, 4);
            t.translate(128, 128);
            t.rotate(44);
            
            painter.setTransform(t);
            painter.setRenderHints(QPainter::RenderHint::SmoothPixmapTransform 
                                 | QPainter::RenderHint::Antialiasing);
            
            qDebug() << t.map(QPolygonF(QRectF(-256, -256, 512, 512)));
            
            bool error = false;
            
            for (auto i = entities.rbegin(); i != entities.rend(); ++i)
            {
                auto&& entity = **i;
                assert(entity.draw_fn);
                entity.draw(&painter, QRect(-256, -256, 512, 512), &error);
            }
            
            dest.save("render.png");
        }
    }
};

QTEST_MAIN(RasterSurface_UTest)

#include "rastersurface_utest.moc"
// #include "mocks/rendering/moc_mockrenderable.cpp"
