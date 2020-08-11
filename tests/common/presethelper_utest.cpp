#include <QtTest/QtTest>
#include <QtDebug>
#include <QObject>

#include "utilities/presethelper.hpp"

class PresetHelper_UTest : public QObject
{
    Q_OBJECT
private slots:
    void nearest1()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nearest(0.0) == 0);
    }

    void nearest2()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nearest(5.0) == 0);
    }

    void nearest3()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nearest(-5.0) == 0);
    }

    void nearest4()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nearest(55.0) == 0);
    }

    void nearest5()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nearest(35.0) == 2);
    }

    void nearest6()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nearest(25.0) == 2);
    }

    void nearest7()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        });

        QVERIFY(presets.nearest(-10, 5) == -1);
    }

    void nearest8()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        });

        QVERIFY(presets.nearest(55, 5) == -1);
    }

    void nearest9()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        });

        QVERIFY(presets.nearest(20, 2) == -1);
    }

    void nextUp1()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextUp(0) == 1);
    }

    void nextUp2()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextUp(0.0) == 1);
    }

    void nextUp3()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextUp(0.01) == 1);
    }

    void nextUp4()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextUp(14.99) == 1);
    }

    void nextUp5()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextUp(1) == 2);
    }

    void nextUp6()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextUp(15.0) == 2);
    }

    void nextUp7()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextUp(-5.0) == 0);
    }
    
    void nextUp8()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextUp(55.0) == 0);
    }

    void nextUp9()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        });

        QVERIFY(presets.nextUp(-5.0) == 0);
    }
    
    void nextUp10()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        });

        QVERIFY(presets.nextUp(55.0) == -1);
    }

    void nextDown1()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextDown(1) == 0);
    }

    void nextDown2()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextDown(15.0) == 0);
    }

    void nextDown3()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextDown(14.99) == 0);
    }

    void nextDown4()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextDown(15.01) == 1);
    }

    void nextDown5()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextDown(0) == 3);
    }

    void nextDown6()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextDown(0.1) == 0);
    }

    void nextDown7()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextDown(-5.0) == 3);
    }
    
    void nextDown8()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        }, true);

        QVERIFY(presets.nextDown(55.0) == 3);
    }

    void nextDown9()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        });

        QVERIFY(presets.nextDown(-5.0) == -1);
    }
    
    void nextDown10()
    {
        PresetHelper<> presets({
            { 0, 0.0 },
            { 1, 15.0 },
            { 2, 30.0 },
            { 3, 45.0 }
        });

        QVERIFY(presets.nextDown(55.0) == 3);
    }
};

QTEST_MAIN(PresetHelper_UTest)

#include "presethelper_utest.moc"