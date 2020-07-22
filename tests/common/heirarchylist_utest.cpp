#include <QtTest/QtTest>
#include <QtDebug>
#include <QObject>

#include "utilities/heirarchylist.hpp"

class HeirarchyListTest_UTest : public QObject
{
    Q_OBJECT
private slots:

    void pointerCorrectness1()
    {
        HeirarchyList<QString>::NodePointer pointer;
        QVERIFY(!pointer);

        {
            HeirarchyList<QString> list;
            pointer = list.addValue("Spiff");

            QVERIFY(pointer);
        }

        QVERIFY(!pointer);
    }

    void pointerCorrectness2()
    {
        HeirarchyList<QString>::NodePointer pointer;
        QVERIFY(!pointer);

        HeirarchyList<QString> list;
        auto group = list.addGroup();
        pointer = group->addValue("Spiff");

        QVERIFY(pointer);

        *group = "Freem";

        QVERIFY(!pointer);
    }

    void pointerCorrectness3()
    {
        HeirarchyList<QString> list1;

        auto pointer = list1.addValue("Spiff");

        HeirarchyList<QString> list2(list1);

        QVERIFY(pointer == &list2.at(0));
    }

    void pointerCorrectness4()
    {
        HeirarchyList<QString> list;
        
        auto group = list.addGroup();
        auto pointer1 = group->addValue("Spiff");
        auto pointer2 = group->addValue("Freem");

        group->remove(0);

        QVERIFY(!pointer1);
        QVERIFY(pointer2 == &group->at(0));
        QVERIFY(pointer2->asValue() == "Freem");
    }

    void comparisonCorrectness1()
    {
        HeirarchyList<QString> list;

        auto a = list.addValue("Spiff");
        auto b = list.addValue("Freem");

        QVERIFY(a < b);
        QVERIFY(b > a);
        QVERIFY(!(b < a));
        QVERIFY(!(a > b));
    }

    void comparisonCorrectness2()
    {
        HeirarchyList<QString> list;

        auto group = list.addGroup();
        auto value = group->addValue("Spiff");

        QVERIFY(group < value);
        QVERIFY(value > group);
        QVERIFY(!(value < group));
        QVERIFY(!(group > value));
    }
    
    void comparisonCorrectness3()
    {
        HeirarchyList<QString> list;

        auto group1 = list.addGroup();
        auto group2 = group1->addGroup();
        auto value = group2->addValue("Spiff");

        QVERIFY(group1 < value);
        QVERIFY(value > group1);
        QVERIFY(!(value < group1));
        QVERIFY(!(group1 > value));
    }

    void comparisonCorrectness4()
    {
        HeirarchyList<QString> list;

        auto group1 = list.addGroup();
        auto value1 = group1->addValue("Spiff");

        auto group2 = group1->addGroup();
        auto value2 = group2->addValue("Freem");

        QVERIFY(value1 < value2);
        QVERIFY(value2 > value1);
        QVERIFY(!(value2 < value1));
        QVERIFY(!(value1 > value2));
    }

    void emancipation1()
    {
        HeirarchyList<QString> list2;
        HeirarchyList<QString>::NodePointer group1;
        HeirarchyList<QString>::NodePointer value1;
        HeirarchyList<QString>::NodePointer value2;
        {
            HeirarchyList<QString> list1;
            group1 = list1.addGroup();
            value1 = list1.addValue("Spiff");

            value2 = group1->addValue("Freem");

            list2.root().adoptNode(*group1);
        }

        QVERIFY(!value1);

        QVERIFY(group1);
        QVERIFY(value2);
    }
};

QTEST_MAIN(HeirarchyListTest_UTest)

#include "heirarchylist_utest.moc"