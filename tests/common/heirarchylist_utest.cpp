#include <QtTest/QtTest>
#include <QtDebug>
#include <QObject>

#include "utilities/heirarchylist.hpp"

class HeirarchyListTest_UTest : public QObject
{
    Q_OBJECT
private slots:

    void append()
    {
        HeirarchyList<QString> list;
        auto& node1 = list.addValue("Spiff");
        
        QVERIFY(list[0].asValue() == "Spiff");
        QVERIFY(node1 == list[0]);
    }

    void prepend()
    {
        HeirarchyList<QString> list;
        auto& node1 = list.addValue("Spiff");
        auto& node2 = list.addValue("Freem", 0);
        
        QVERIFY(list[0].asValue() == "Freem");
        QVERIFY(node2 == list[0]);
        QVERIFY(list[1].asValue() == "Spiff");
        QVERIFY(node1 == list[1]);
    }

    void insert()
    {
        HeirarchyList<QString> list;
        auto& node1 = list.addValue("Spiff");
        auto& node2 = list.addValue("Freem");
        auto& node3 = list.addValue("Zorch", 1);
        
        QVERIFY(list[0].asValue() == "Spiff");
        QVERIFY(node1 == list[0]);
        QVERIFY(list[1].asValue() == "Zorch");
        QVERIFY(node3 == list[1]);
        QVERIFY(list[2].asValue() == "Freem");
        QVERIFY(node2 == list[2]);
    }

    void remove1()
    {
        HeirarchyList<QString> list;
        list.addValue("Spiff");
        list.addValue("Freem");

        list.remove(1);
        QVERIFY(list.size() == 1);
        QVERIFY(list[0].asValue() == "Spiff");
    }

    void remove2()
    {
        HeirarchyList<QString> list;
        list.addValue("Spiff");
        list.addValue("Freem");

        list.remove(0);
        QVERIFY(list.size() == 1);
        QVERIFY(list[0].asValue() == "Freem");
    }

    // void pointers1()
    // {
    //     HeirarchyList<QString>::Node& pointer;
    //     QVERIFY(!pointer);

    //     {
    //         HeirarchyList<QString> list;
    //         pointer = list.addValue("Spiff");

    //         QVERIFY(pointer);
    //     }

    //     QVERIFY(!pointer);
    // }

    // void pointers2()
    // {
    //     HeirarchyList<QString>::Node& pointer;
    //     QVERIFY(!pointer);

    //     HeirarchyList<QString> list;
    //     auto group = list.addGroup();
    //     pointer = group->addValue("Spiff");

    //     QVERIFY(pointer);

    //     *group = "Freem";

    //     QVERIFY(!pointer);
    // }

    // void pointers3()
    // {
    //     HeirarchyList<QString> list1;

    //     auto pointer = list1.addValue("Spiff");

    //     HeirarchyList<QString> list2(list1);

    //     QVERIFY(pointer == &list2.at(0));
    // }

    // void pointers4()
    // {
    //     HeirarchyList<QString> list;
        
    //     auto group = list.addGroup();
    //     auto pointer1 = group->addValue("Spiff");
    //     auto pointer2 = group->addValue("Freem");

    //     group->remove(0);

    //     QVERIFY(!pointer1);
    //     QVERIFY(pointer2 == &group->at(0));
    //     QVERIFY(pointer2->asValue() == "Freem");
    // }

    // void comparisons1()
    // {
    //     HeirarchyList<QString> list;

    //     auto a = list.addValue("Spiff");
    //     auto b = list.addValue("Freem");

    //     QVERIFY(a < b);
    //     QVERIFY(b > a);
    //     QVERIFY(!(b < a));
    //     QVERIFY(!(a > b));
    // }

    // void comparisons2()
    // {
    //     HeirarchyList<QString> list;

    //     auto group = list.addGroup();
    //     auto value = group.addValue("Spiff");

    //     QVERIFY(group < value);
    //     QVERIFY(value > group);
    //     QVERIFY(!(value < group));
    //     QVERIFY(!(group > value));
    // }
    
    // void comparisons3()
    // {
    //     HeirarchyList<QString> list;

    //     auto group1 = list.addGroup();
    //     auto group2 = group1.addGroup();
    //     auto value = group2.addValue("Spiff");

    //     QVERIFY(group1 < value);
    //     QVERIFY(value > group1);
    //     QVERIFY(!(value < group1));
    //     QVERIFY(!(group1 > value));
    // }

    // void comparisons4()
    // {
    //     HeirarchyList<QString> list;

    //     auto group1 = list.addGroup();
    //     auto value1 = group1.addValue("Spiff");

    //     auto group2 = group1.addGroup();
    //     auto value2 = group2.addValue("Freem");

    //     QVERIFY(value1 < value2);
    //     QVERIFY(value2 > value1);
    //     QVERIFY(!(value2 < value1));
    //     QVERIFY(!(value1 > value2));
    // }

    // void emancipation1()
    // {
    //     HeirarchyList<QString> list2;
    //     HeirarchyList<QString>::Node& group1;
    //     HeirarchyList<QString>::Node& value1;
    //     HeirarchyList<QString>::Node& value2;
    //     {
    //         HeirarchyList<QString> list1;
    //         group1 = list1.addGroup();
    //         value1 = list1.addValue("Spiff");

    //         value2 = group1.addValue("Freem");

    //         list2.root().adoptNode(*group1);
    //     }

    //     QVERIFY(!value1);

    //     QVERIFY(group1);
    //     QVERIFY(value2);
    // }
};

QTEST_MAIN(HeirarchyListTest_UTest)

#include "heirarchylist_utest.moc"