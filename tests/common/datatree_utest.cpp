/**
 * Addle test code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QtTest/QtTest>
#include <QtDebug>
#include <QObject>

#include "utilities/datatree.hpp"

using namespace Addle;

class DataTree_UTest : public QObject
{
    Q_OBJECT
private slots:

    void leavesAndBranches()
    {
        DataTree<QString, void> tree;
        
        int expectedLeafCount = 0;
        int expectedBranchCount = 1; // root
        
        expectedBranchCount += 10;
        for (auto& branch1 : tree.root().addBranchesCount(10))
        {
            expectedBranchCount += 10;
            for (auto& branch2 : branch1.addBranchesCount(10))
            {
                for (int i = 0; i < 5; ++i)
                {
                    branch2.addLeaf("spiff");
                    ++expectedLeafCount;
                }
            }
        }
        
        int actualLeafCount = 0;
        for (auto& node : tree.leaves())
        {
            ++actualLeafCount;
            QVERIFY(node.isLeaf());
        }
        
        int actualBranchCount = 0;
        for (auto& node : tree.branches())
        {
            ++actualBranchCount;
            QVERIFY(node.isBranch());
        }
        
        QVERIFY(expectedLeafCount == actualLeafCount);
        QVERIFY(expectedBranchCount == actualBranchCount);
    }
    
//     void append()
//     {
//         DataTree<QString> list;
//         auto& node1 = list.addValue("Spiff");
//         
//         QVERIFY(list[0].asValue() == "Spiff");
//         QVERIFY(node1 == list[0]);
//     }
// 
//     void prepend()
//     {
//         DataTree<QString> list;
//         auto& node1 = list.addValue("Spiff");
//         auto& node2 = list.addValue("Freem", 0);
//         
//         QVERIFY(list[0].asValue() == "Freem");
//         QVERIFY(node2 == list[0]);
//         QVERIFY(list[1].asValue() == "Spiff");
//         QVERIFY(node1 == list[1]);
//     }
// 
//     void insert()
//     {
//         DataTree<QString> list;
//         auto& node1 = list.addValue("Spiff");
//         auto& node2 = list.addValue("Freem");
//         auto& node3 = list.addValue("Zorch", 1);
//         
//         QVERIFY(list[0].asValue() == "Spiff");
//         QVERIFY(node1 == list[0]);
//         QVERIFY(list[1].asValue() == "Zorch");
//         QVERIFY(node3 == list[1]);
//         QVERIFY(list[2].asValue() == "Freem");
//         QVERIFY(node2 == list[2]);
//     }
// 
//     void remove1()
//     {
//         DataTree<QString> list;
//         list.addValue("Spiff");
//         list.addValue("Freem");
// 
//         list.remove(1);
//         QVERIFY(list.size() == 1);
//         QVERIFY(list[0].asValue() == "Spiff");
//     }
// 
//     void remove2()
//     {
//         DataTree<QString> list;
//         list.addValue("Spiff");
//         list.addValue("Freem");
// 
//         list.remove(0);
//         QVERIFY(list.size() == 1);
//         QVERIFY(list[0].asValue() == "Freem");
//     }
// 
//     void count1()
//     {
//         DataTree<QString> list;
//         
//         for (int i = 0; i < 50; ++i)
//         {
//             list.addValue(QString::number(i));
//         }
// 
//         QVERIFY(list.nodeCount() == 50);
//     }
// 
//     void count2()
//     {
//         DataTree<QString> list;
// 
//         DataTree<QString>::Node* cursor = &list.root();
//         for (int i = 0; i < 50; ++i)
//         {
//             cursor = &cursor->addGroup();
//         }
// 
//         QVERIFY(list.nodeCount() == 50);
//     }
// 
//     void count3()
//     {
//         DataTree<QString> list;
// 
//         DataTree<QString>::Node* cursor = &list.root();
//         for (int i = 0; i < 50; ++i)
//         {
//             if ((i % 2) == 0)
//                 cursor = &cursor->addGroup();
//             else
//                 cursor->addValue(QString::number(i));
//         }
// 
//         QVERIFY(list.nodeCount() == 50);
//     }
//     
//     void count4()
//     {
//         DataTree<QString> list;
//         
//         for (int i = 0; i < 50; ++i)
//         {
//             list.addValue(QString::number(i));
//         }
// 
//         for (int i = 0; i < 50; ++i)
//         {
//             list.remove(0);
//         }
// 
//         QVERIFY(list.nodeCount() == 0);
//     }
// 
//     void count5()
//     {
//         DataTree<QString> list;
// 
//         DataTree<QString>::Node* cursor = &list.root();
//         for (int i = 0; i < 50; ++i)
//         {
//             cursor = &cursor->addGroup();
//         }
// 
//         list.remove(0);
// 
//         QVERIFY(list.nodeCount() == 0);
//     }
// 
//     void count6()
//     {
//         DataTree<QString> list;
// 
//         DataTree<QString>::Node* cursor = &list.root();
//         for (int i = 0; i < 50; ++i)
//         {
//             if ((i % 2) == 0)
//                 cursor = &cursor->addGroup();
//             else
//                 cursor->addValue(QString::number(i));
//         }
// 
//         list.remove(0);
// 
//         QVERIFY(list.nodeCount() == 0);
//     }
//     
// 
//     void comparisons1()
//     {
//         DataTree<QString> list;
// 
//         auto& a = list.addValue("Spiff");
//         auto& b = list.addValue("Freem");
// 
//         QVERIFY(a < b);
//         QVERIFY(b > a);
//         QVERIFY(!(b < a));
//         QVERIFY(!(a > b));
//     }
// 
//     void comparisons2()
//     {
//         DataTree<QString> list;
// 
//         auto& group = list.addGroup();
//         auto& value = group.addValue("Spiff");
// 
//         QVERIFY(group < value);
//         QVERIFY(value > group);
//         QVERIFY(!(value < group));
//         QVERIFY(!(group > value));
//     }
//     
//     void comparisons3()
//     {
//         DataTree<QString> list;
// 
//         auto& group1 = list.addGroup();
//         auto& group2 = group1.addGroup();
//         auto& value = group2.addValue("Spiff");
// 
//         QVERIFY(group1 < value);
//         QVERIFY(value > group1);
//         QVERIFY(!(value < group1));
//         QVERIFY(!(group1 > value));
//     }
// 
//     void comparisons4()
//     {
//         DataTree<QString> list;
// 
//         auto& group1 = list.addGroup();
//         auto& value1 = group1.addValue("Spiff");
// 
//         auto& group2 = group1.addGroup();
//         auto& value2 = group2.addValue("Freem");
// 
//         QVERIFY(value1 < value2);
//         QVERIFY(value2 > value1);
//         QVERIFY(!(value2 < value1));
//         QVERIFY(!(value1 > value2));
//     }
// 
//     void comparisons5()
//     {
//         DataTree<QString> list;
// 
//         auto& value = list.addValue("Spiff");
// 
//         QVERIFY(!(value < value));
//         QVERIFY(!(value > value));
//     }
// 
//     void removeNodes1()
//     {
//         DataTree<QString> list;
//         QList<DataTree<QString>::Node*> nodes;
// 
//         list.addValue("Spiff");
//         nodes.append(&list.addValue("Freem"));
// 
//         auto removed = list.removeNodes(nodes);
// 
//         QVERIFY(removed[0].index() == 1);
//         QVERIFY(removed[0].depth() == 1);
//         QVERIFY(boost::get<const DataTree<QString>::Node*>(removed[0].parent()) == &list.root());
//         QVERIFY(removed[0]->asValue() == "Freem");
//     }
// 
//     void removeNodes2()
//     {
//         DataTree<QString> list;
//         QList<DataTree<QString>::Node*> nodes;
// 
//         nodes.append(&list.addValue("Spiff"));
//         list.addValue("Freem");
// 
//         auto removed = list.removeNodes(nodes);
// 
//         QVERIFY(removed[0].index() == 0);
//         QVERIFY(removed[0].depth() == 1);
//         QVERIFY(boost::get<const DataTree<QString>::Node*>(removed[0].parent()) == &list.root());
//         QVERIFY(removed[0]->asValue() == "Spiff");
// 
//         QVERIFY(list[0].asValue() == "Freem");
//         QVERIFY(list[0].index() == 0);
//     }
// 
//     void removeNodes3()
//     {
//         DataTree<QString> list;
//         QList<DataTree<QString>::Node*> nodes;
// 
//         auto& group = list.addGroup();
//         nodes.append(&group);
// 
//         auto& value = group.addValue("Spiff");
// 
//         list.addValue("Freem");
//         
//         auto removed = list.removeNodes(nodes);
// 
//         QVERIFY(removed[0].node() == &group);
//         QVERIFY(removed[1].node() == &value);
//         QVERIFY(boost::get<DataTree<QString>::NodeRemoved>(removed[1].parent()) == removed[0]);
// 
//         QVERIFY(removed[0] < removed[1]);
//         QVERIFY(!(removed[0] > removed[1]));
//         QVERIFY(removed[1] > removed[0]);
//         QVERIFY(!(removed[1] < removed[0]));
//     }

    // void batchEmancipate()
    // {
    //     DataTree<QString> list;

    //     auto& value1 = list.addValue("Spiff");
    //     auto& value2 = list.addValue("Freem");
    //     auto& group = list.addGroup();
    //     auto& value3 = group.addValue("Zorch");

    //     QList<DataTree<QString>::Node*> nodes = {
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
    
    void fastAddressCorrectness()
    {
        // Representing the fast address scheme with F, and addresses a1 of
        // length n and a2 of length m, the address scheme is incorrect if
        // F(a1) < F(a2) and a1 >= a2.
        // 
        // Cases of interest to this test are:
        //    a1 > a2 and n == m
        //    a1 > a2 and n < m
        //    a1 > a2 and n > m
        //
        // F produces identical outputs for identical inputs, and (assuming all
        // preconditions for numerical limits are met) distinct outputs for all 
        // distinct inputs. Verifying this trivially dismisses the other cases 
        // where F is incorrect.
        //
        // This test is also interested in verifying that the numerical limits
        // of fast addresses are respected, and that slow addresses are used
        // when these limits are exceeded.
        //
        // The total fast address space (64 bits) is much too large for a casual
        // unit test to verify exhaustively so we will focus on the edge values 
        // 0, 1, 254, and 255; and addresses with length of <= 9
        
        const std::size_t VALUES[] = { 0, 1, 254, 255 };
        const std::size_t MAX_LENGTH = 9;
        
        using address_t = aux_datatree::NodeAddress;
        
        // # Verify that identical addresses are equivalent
        
        for (std::size_t n = 0; n <= MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : VALUES )
            {
                address_t a1;
                for (std::size_t i = 0; i < n; ++i)
                {
                    a1 = std::move(a1) << v1;
                }
                
                //qDebug() << a1;
                
                QVERIFY( a1 == a1 );
                QVERIFY( !(a1 < a1) );
                QVERIFY( !(a1 > a1) );
                
                address_t a2(a1);
                
                QVERIFY( a1 == a2 );
                QVERIFY( !(a1 < a2) );
                QVERIFY( !(a1 > a2) );
            }
        }
        
        // # Verify that non-identical addresses are not equivalent
        for (std::size_t n = 1; n <= MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : VALUES )
            {
                for (std::size_t v2 : VALUES )
                {
                    if (v1 == v2)
                        continue;
                    
                    address_t a1;
                    address_t a2;
                    for (std::size_t i = 0; i < n; ++i)
                    {
                        a1 = std::move(a1) << v1;
                        a2 = std::move(a2) << v2;
                    }
                    
                    //qDebug() << a1 << a2;
                    
                    QVERIFY( a1 != a2 );
                    QVERIFY( a1 < a2 || a1 > a2 );
                    QVERIFY( !(a1 < a2 && a1 > a2) );
                }
            }
        }
        
        // # Verify that F(a1) < F(a2) if a1 < a2 and n == m
        // ## a1[i] < a2[i] for i <= n
        
        for (std::size_t n = 1; n <= MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : VALUES )
            {
                for (std::size_t v2 : VALUES )
                {
                    if ( v1 >= v2 )
                        continue;
                    
                    address_t a1;
                    address_t a2;
                    for (std::size_t i = 0; i < n; ++i)
                    {
                        a1 = std::move(a1) << v1;
                        a2 = std::move(a2) << v2;
                    }
                    
                    //qDebug() << a1 << a2;
                    
                    QVERIFY( a1 != a2 );
                    QVERIFY( a1 < a2 );
                    QVERIFY( !(a1 > a2) );
                }
            }
        }
        
        // ## a1[0] < a2[0], all other indices are identical
        
        for (std::size_t n = 1; n <= MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : VALUES )
            {
                for (std::size_t v2 : VALUES )
                {
                    for (std::size_t w : VALUES )
                    {
                        if ( v1 >= v2 )
                            continue;
                        
                        address_t a1({ v1 });
                        address_t a2({ v2 });
                        for (std::size_t i = 1; i < n; ++i)
                        {
                            a1 = std::move(a1) << w;
                            a2 = std::move(a2) << w;
                        }
                        
                        //qDebug() << a1 << a2;
                        
                        QVERIFY( a1 != a2 );
                        QVERIFY( a1 < a2 );
                        QVERIFY( !(a1 > a2) );
                    }
                }
            }
        }
        
        // ## a1[n-1] < a2[n-1] (last indices), all other indices are identical
        
        for (std::size_t n = 1; n <= MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : VALUES )
            {
                for (std::size_t v2 : VALUES )
                {
                    for (std::size_t w : VALUES )
                    {
                        if ( v1 >= v2 )
                            continue;
                        
                        address_t a1;
                        address_t a2;
                        for (std::size_t i = 1; i < n; ++i)
                        {
                            a1 = std::move(a1) << w;
                            a2 = std::move(a2) << w;
                        }
                        
                        a1 = std::move(a1) << v1;
                        a2 = std::move(a2) << v2;
                        
                        //qDebug() << a1 << a2;
                        
                        QVERIFY( a1 != a2 );
                        QVERIFY( a1 < a2 );
                        QVERIFY( !(a1 > a2) );
                    }
                }
            }
        }
        
        // ## a1[n-2] < a2[n-2] (next-to-last indices), all other indices are
        // identical
        
        for (std::size_t n = 2; n <= MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : VALUES )
            {
                for (std::size_t v2 : VALUES )
                {
                    for (std::size_t w : VALUES )
                    {
                        if ( v1 >= v2 )
                            continue;
                        
                        address_t a1;
                        address_t a2;
                        for (std::size_t i = 2; i < n; ++i)
                        {
                            a1 = std::move(a1) << w;
                            a2 = std::move(a2) << w;
                        }
                        
                        a1 = std::move(a1) << v1;
                        a1 = std::move(a1) << w;
                        
                        a2 = std::move(a2) << v2;
                        a2 = std::move(a2) << w;
                        
                        //qDebug() << a1 << a2;
                        
                        QVERIFY( a1 != a2 );
                        QVERIFY( a1 < a2 );
                        QVERIFY( !(a1 > a2) );
                    }
                }
            }
        }
        
        // # Verify that F(a1) < F(a2) if a1 < a2 and n < m
        
        // ## a1 is the empty address and a2 is a value with trailing 0's
        {
            address_t a1;
            
            for (std::size_t n = 1; n <= MAX_LENGTH; ++n)
            {
                for (std::size_t v1 : VALUES )
                {
                    address_t a2({ v1 });
                    for (std::size_t i = 1; i < n; ++i)
                    {
                        a2 = std::move(a2) << 0;
                    }
                    
                    //qDebug() << a1 << a2;
                    
                    QVERIFY( a1 != a2 );
                    QVERIFY( a1 < a2 );
                    QVERIFY( !(a1 > a2) );
                }
            }
        }
        
        // ## a1 is the empty address and a2 is a value with leading 0's
        {
            address_t a1;
            
            for (std::size_t n = 1; n <= MAX_LENGTH; ++n)
            {
                for (std::size_t v1 : VALUES )
                {
                    address_t a2;
                    for (std::size_t i = 1; i < n; ++i)
                    {
                        a2 = std::move(a2) << 0;
                    }
                    
                    a2 = std::move(a2) << v1;
                    
                    //qDebug() << a1 << a2;
                    
                    QVERIFY( a1 != a2 );
                    QVERIFY( a1 < a2 );
                    QVERIFY( !(a1 > a2) );
                }
            }
        }
        
        // ## a1 is a sequence of identical values. a2 is a1 with an additional
        // value appended.
    
        for (std::size_t n = 1; n <= MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : VALUES )
            {
                address_t a1;
                for (std::size_t i = 0; i < n; ++i)
                {
                    a1 = std::move(a1) << v1;
                }
                
                for (std::size_t v2 : VALUES )
                {
                    address_t a2(a1);
                    
                    a2 = std::move(a2) << v2;
                    
                    //qDebug() << a1 << a2;
                    
                    QVERIFY( a1 != a2 );
                    QVERIFY( a1 < a2 );
                    QVERIFY( !(a1 > a2) );
                }
            }
        }
        
        // # Verify that F(a1) < F(a2) if a1 < a2 and n > m
        // ## m = 1, a1[0] < a2[0], the rest are identical values
        
        for (std::size_t n = 2; n <= MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : VALUES )
            {
                for (std::size_t v2 : VALUES )
                {
                    if ( v1 >= v2 )
                        continue;
                    
                    for (std::size_t w : VALUES )
                    {
                        address_t a1({ v1 });
                        address_t a2({ v2 });
                        
                        for (std::size_t i = 1; i < n; ++i)
                        {
                            a1 = std::move(a1) << w;
                        }
                        
                        //qDebug() << a1 << a2;
                    
                        QVERIFY( a1 != a2 );
                        QVERIFY( a1 < a2 );
                        QVERIFY( !(a1 > a2) );
                    }
                }
            }
        }
        
        // ## m = n - 1, a1[0] < a2[0], the rest are identical values
        
        for (std::size_t n = 2; n <= MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : VALUES )
            {
                for (std::size_t v2 : VALUES )
                {
                    if ( v1 >= v2 )
                        continue;
                    
                    for (std::size_t w : VALUES )
                    {
                        address_t a1({ v1 });
                        address_t a2({ v2 });
                        
                        for (std::size_t i = 1; i < n - 1; ++i)
                        {
                            a1 = std::move(a1) << w;
                            a2 = std::move(a2) << w;
                        }
                        a1 = std::move(a1) << w;
                        
                        //qDebug() << a1 << a2;
                    
                        QVERIFY( a1 != a2 );
                        QVERIFY( a1 < a2 );
                        QVERIFY( !(a1 > a2) );
                    }
                }
            }
        }
        
        // ## m = n - 1, a1[m-1] < a2[m-1], the rest are identical values
        
        for (std::size_t n = 2; n <= MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : VALUES )
            {
                for (std::size_t v2 : VALUES )
                {
                    if ( v1 >= v2 )
                        continue;
                    
                    for (std::size_t w : VALUES )
                    {
                        address_t a1;
                        address_t a2;
                        
                        for (std::size_t i = 1; i < n - 2; ++i)
                        {
                            a1 = std::move(a1) << w;
                            a2 = std::move(a2) << w;
                        }
                        a1 = std::move(a1) << v1;
                        a1 = std::move(a1) << w;
                        
                        a2 = std::move(a2) << v2;
                          
                        //qDebug() << a1 << a2;
                    
                        QVERIFY( a1 != a2 );
                        QVERIFY( a1 < a2 );
                        QVERIFY( !(a1 > a2) );
                    }
                }
            }
        }
    }
};

QTEST_MAIN(DataTree_UTest)

#include "datatree_utest.moc"
