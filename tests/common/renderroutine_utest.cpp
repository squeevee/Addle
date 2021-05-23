#include <QtTest/QtTest>
#include <QtDebug>
#include <QObject>

#include "utilities/render/renderroutine.hpp"

using namespace Addle;

void render_test(const RenderHandle&, const void*) noexcept
{
}

class RenderRoutine_UTest : public QObject
{
    Q_OBJECT
private slots:
    void dev_1()
    {
        RenderRoutine r1;
        RenderRoutine r2;
        r1.addSubRoutine(r2);
        r1.addMask(QPainterPath(), -100);
        r1.addMask(QRegion(), Q_INFINITY);
        r1.addFunction(&render_test, nullptr);
        r1.addFunction(&render_test, nullptr, 99999);
        r1.addFunction(&render_test, nullptr, -1);
        r1.addFunction(&render_test, nullptr, 13);
        
        r1.visit(
                [](double z, auto&& v) { qDebug() << typeid(v).name() << z; },
                -2,
                Q_INFINITY
            );
        
        for (auto f : r1.functions(-Q_INFINITY, 13))
        {
            qDebug() << f.second.function << f.second.context << f.first;
        }
        //qDebug() << r1.functionsCount(-1, 13);
    }
};

QTEST_MAIN(RenderRoutine_UTest)

#include "renderroutine_utest.moc"
