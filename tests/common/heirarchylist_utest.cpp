#include <QtTest/QtTest>
#include <QtDebug>
#include <QObject>

#include "utilities/heirarchylist.hpp"

class HeirarchyList_UTest : public QObject
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

    void count1()
    {
        HeirarchyList<QString> list;
        
        for (int i = 0; i < 50; ++i)
        {
            list.addValue(QString::number(i));
        }

        QVERIFY(list.nodeCount() == 50);
    }

    void count2()
    {
        HeirarchyList<QString> list;

        HeirarchyList<QString>::Node* cursor = &list.root();
        for (int i = 0; i < 50; ++i)
        {
            cursor = &cursor->addGroup();
        }

        QVERIFY(list.nodeCount() == 50);
    }

    void count3()
    {
        HeirarchyList<QString> list;

        HeirarchyList<QString>::Node* cursor = &list.root();
        for (int i = 0; i < 50; ++i)
        {
            if ((i % 2) == 0)
                cursor = &cursor->addGroup();
            else
                cursor->addValue(QString::number(i));
        }

        QVERIFY(list.nodeCount() == 50);
    }
    
    void count4()
    {
        HeirarchyList<QString> list;
        
        for (int i = 0; i < 50; ++i)
        {
            list.addValue(QString::number(i));
        }

        for (int i = 0; i < 50; ++i)
        {
            list.remove(0);
        }

        QVERIFY(list.nodeCount() == 0);
    }

    void count5()
    {
        HeirarchyList<QString> list;

        HeirarchyList<QString>::Node* cursor = &list.root();
        for (int i = 0; i < 50; ++i)
        {
            cursor = &cursor->addGroup();
        }

        list.remove(0);

        QVERIFY(list.nodeCount() == 0);
    }

    void count6()
    {
        HeirarchyList<QString> list;

        HeirarchyList<QString>::Node* cursor = &list.root();
        for (int i = 0; i < 50; ++i)
        {
            if ((i % 2) == 0)
                cursor = &cursor->addGroup();
            else
                cursor->addValue(QString::number(i));
        }

        list.remove(0);

        QVERIFY(list.nodeCount() == 0);
    }
    

    void comparisons1()
    {
        HeirarchyList<QString> list;

        auto& a = list.addValue("Spiff");
        auto& b = list.addValue("Freem");

        QVERIFY(a < b);
        QVERIFY(b > a);
        QVERIFY(!(b < a));
        QVERIFY(!(a > b));
    }

    void comparisons2()
    {
        HeirarchyList<QString> list;

        auto& group = list.addGroup();
        auto& value = group.addValue("Spiff");

        QVERIFY(group < value);
        QVERIFY(value > group);
        QVERIFY(!(value < group));
        QVERIFY(!(group > value));
    }
    
    void comparisons3()
    {
        HeirarchyList<QString> list;

        auto& group1 = list.addGroup();
        auto& group2 = group1.addGroup();
        auto& value = group2.addValue("Spiff");

        QVERIFY(group1 < value);
        QVERIFY(value > group1);
        QVERIFY(!(value < group1));
        QVERIFY(!(group1 > value));
    }

    void comparisons4()
    {
        HeirarchyList<QString> list;

        auto& group1 = list.addGroup();
        auto& value1 = group1.addValue("Spiff");

        auto& group2 = group1.addGroup();
        auto& value2 = group2.addValue("Freem");

        QVERIFY(value1 < value2);
        QVERIFY(value2 > value1);
        QVERIFY(!(value2 < value1));
        QVERIFY(!(value1 > value2));
    }

    void comparisons5()
    {
        HeirarchyList<QString> list;

        auto& value = list.addValue("Spiff");

        QVERIFY(!(value < value));
        QVERIFY(!(value > value));
    }

    void removeNodes1()
    {
        HeirarchyList<QString> list;
        QList<HeirarchyList<QString>::Node*> nodes;

        list.addValue("Spiff");
        nodes.append(&list.addValue("Freem"));

        auto removed = list.removeNodes(nodes);

        QVERIFY(removed[0].index() == 1);
        QVERIFY(removed[0].depth() == 1);
        QVERIFY(boost::get<const HeirarchyList<QString>::Node*>(removed[0].parent()) == &list.root());
        QVERIFY(removed[0]->asValue() == "Freem");
    }

    void removeNodes2()
    {
        HeirarchyList<QString> list;
        QList<HeirarchyList<QString>::Node*> nodes;

        nodes.append(&list.addValue("Spiff"));
        list.addValue("Freem");

        auto removed = list.removeNodes(nodes);

        QVERIFY(removed[0].index() == 0);
        QVERIFY(removed[0].depth() == 1);
        QVERIFY(boost::get<const HeirarchyList<QString>::Node*>(removed[0].parent()) == &list.root());
        QVERIFY(removed[0]->asValue() == "Spiff");

        QVERIFY(list[0].asValue() == "Freem");
        QVERIFY(list[0].index() == 0);
    }

    void removeNodes3()
    {
        HeirarchyList<QString> list;
        QList<HeirarchyList<QString>::Node*> nodes;

        auto& group = list.addGroup();
        nodes.append(&group);

        auto& value = group.addValue("Spiff");

        list.addValue("Freem");
        
        auto removed = list.removeNodes(nodes);

        QVERIFY(removed[0].node() == &group);
        QVERIFY(removed[1].node() == &value);
        QVERIFY(boost::get<HeirarchyList<QString>::NodeRemoved>(removed[1].parent()) == removed[0]);

        QVERIFY(removed[0] < removed[1]);
        QVERIFY(!(removed[0] > removed[1]));
        QVERIFY(removed[1] > removed[0]);
        QVERIFY(!(removed[1] < removed[0]));
    }

    // void batchEmancipate()
    // {
    //     HeirarchyList<QString> list;

    //     auto& value1 = list.addValue("Spiff");
    //     auto& value2 = list.addValue("Freem");
    //     auto& group = list.addGroup();
    //     auto& value3 = group.addValue("Zorch");

    //     QList<HeirarchyList<QString>::Node*> nodes = {
    //         &value1, &value3
    //     };

    //     list.emancipateNodes(nodes.begin(), nodes.end());

    //     QVERIFY(list.nodeCount() == 2);
    //     QVERIFY(value1.parent() == &list.const_root());
    //     QVERIFY(value1.index() == 0);
    //     QVERIFY(value3.parent() == &group);

    //     QVERIFY(value2.index() == 0);
    //     QVERIFY(group.index() == 1);
    //     QVERIFY(group.asGroup().size() == 0);

    //     for (auto node : nodes) delete node;
    // }
};

QTEST_MAIN(HeirarchyList_UTest)

#include "heirarchylist_utest.moc"