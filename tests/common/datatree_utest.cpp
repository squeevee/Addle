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

#include "utilities/datatree/nestedobjectadapter.hpp"
#include "utilities/datatree/basicdatatree.hpp"
//#include "utilities/datatree/observers.hpp"
#include "utilities/datatree/observers2.hpp"
#include "utilities/datatree/views.hpp"

using namespace Addle;

const std::size_t FASTADDRESS_VALUES[] = { 0, 1, 254, 255 };
const std::size_t FASTADDRESS_MAX_LENGTH = 9;

using aux_datatree::NodeAddress;
    
class DataTree_UTest : public QObject
{
    Q_OBJECT
private slots:
    void dev_basicDataTree1()
    {
        BasicDataTree<QString> tree("spiff");
        
        auto& branch1 = tree.root().addChild();
        tree.root().addChild("freem");
        
        auto i = branch1.children().end();
        i = branch1.insertChild(i, "zorg");
        
        auto& branch2 = branch1.addChild("garg");
        branch2.addChildren({ "1", "2", "3", "4", "5", "6" });
        
        branch2.removeChildren(0, 3);
        
        for (QString s : tree)
        {
            //qDebug() << s;
        }
    }
    
    void dev_eventBuilder1()
    {
//         BasicDataTree<QString> tree("spiff");
//         aux_datatree2::NodeEventBuilder<BasicDataTree<QString>> eventBuilder(tree);
        
    }
    
    void nodeEvent_mapping1()
    {
        {
            DataTreeNodeEventBuilder builder;
            builder.addChunk({ 1 }, 1);
            
            QVERIFY(builder.event().mapForward({ 0 }) == DataTreeNodeAddress({ 0 }));
            QVERIFY(builder.event().mapForward({ 1 }) == DataTreeNodeAddress({ 2 }));
            QVERIFY(builder.event().mapForward({ 2 }) == DataTreeNodeAddress({ 3 }));
            QVERIFY(builder.event().mapForward({ 0, 0 }) == DataTreeNodeAddress({ 0, 0 }));
            QVERIFY(builder.event().mapForward({ 1, 0 }) == DataTreeNodeAddress({ 2, 0 }));
            QVERIFY(builder.event().mapForward({ 2, 0 }) == DataTreeNodeAddress({ 3, 0 }));
        }
        {
            DataTreeNodeEventBuilder builder;
            builder.addChunk({ 1 }, 1);
            
            QVERIFY(builder.event().mapBackward({ 0 }) == DataTreeNodeAddress({ 0 }));
            QVERIFY(builder.event().mapBackward({ 2 }) == DataTreeNodeAddress({ 1 }));
            QVERIFY(builder.event().mapBackward({ 3 }) == DataTreeNodeAddress({ 2 }));
            QVERIFY(builder.event().mapBackward({ 0, 0 }) == DataTreeNodeAddress({ 0, 0 }));
            QVERIFY(builder.event().mapBackward({ 2, 0 }) == DataTreeNodeAddress({ 1, 0 }));
            QVERIFY(builder.event().mapBackward({ 3, 0 }) == DataTreeNodeAddress({ 2, 0 }));
        }
    }
    
    void nodeEvent_mapping2()
    {
        {
            DataTreeNodeEventBuilder builder;
            builder.addChunk({ 0 }, 4);
            
            QVERIFY(builder.event().mapForward({ 0 }) == DataTreeNodeAddress({ 4 }));
        }
        {
            DataTreeNodeEventBuilder builder;
            builder.addChunk({ 0 }, 4);
            
            QVERIFY(builder.event().mapBackward({ 4 }) == DataTreeNodeAddress({ 0 }));
        }
    }
    
    void nodeEvent_mapping3()
    {
        {
            DataTreeNodeEventBuilder builder;
            builder.addChunks({ {{ 0 }, 1 }, {{ 0, 0 }, 1 }});

            QVERIFY(builder.event().mapForward({ 0, 0 }) == DataTreeNodeAddress({ 1, 1 }));
            QVERIFY(builder.event().mapForward({ 1, 0 }) == DataTreeNodeAddress({ 2, 0 }));
            QVERIFY(builder.event().mapForward({ 2, 0 }) == DataTreeNodeAddress({ 3, 0 }));
        }
        {
            DataTreeNodeEventBuilder builder;
            builder.addChunks({ {{ 0 }, 1 }, {{ 0, 0 }, 1 }});

            QVERIFY(builder.event().mapBackward({ 1, 1 }) == DataTreeNodeAddress({ 0, 0 }));
            QVERIFY(builder.event().mapBackward({ 2, 0 }) == DataTreeNodeAddress({ 1, 0 }));
            QVERIFY(builder.event().mapBackward({ 3, 0 }) == DataTreeNodeAddress({ 2, 0 }));
        }
    }
    
    void dev_nostalgicHelper()
    {
        DataTreeNodeEventBuilder builder;
        
        //builder.passiveAddChunks({ {{ 0 }, 1 }, {{ 0, 0 }, 1} });
        builder.addChunks({ {{ 12 }, 1 }, {{ 10 }, 1 }, {{ 8 }, 1 }, {{ 6 }, 1 }, {{ 4 }, 1 }, {{ 2 }, 1 }, {{ 0 }, 1 } });
        
        aux_datatree2::NostalgicNodeEventHelper nostalgia(std::move(builder).event());
        
        do
        {
            qDebug() << nostalgia.operatingChunk();
        } while (nostalgia.next());
    }
    
    void dev_nestedObjectAdapter()
    {
        struct TestObject
        {
            int v = 0;
            QList<TestObject> children;
            
            //QList<TestObject> children() const { return _children; }
        };
        
        TestObject t = {0,{{1,{{2,{}},{3,{}}}},{4,{{5,{}}}}}};
        /**
         * 0
         * |--\ 
         * 1   4
         * |\  |
         * 2 3 5
         */
        
//         auto f = [](const TestObject&) { return QList<TestObject>(); };
        
//         qDebug() << sizeof(make_nested_object_adapter(t, &TestObject::children));
//         qDebug() << sizeof(make_nested_object_adapter(t, f));
        
//         auto a = make_nested_object_adapter(
//                 t, 
//                 &TestObject::children,
//                 [] (
//                     TestObject& parent,
//                     QList<TestObject>::iterator pos,
//                     const TestObject& child
//                 ) {
//                     parent._children.insert(pos, child);
//                 }
//             );
        
        {
            auto adapter = make_nested_object_adapter(t, &TestObject::children);
            auto root = datatree_root(adapter);
            
            auto i = ::Addle::aux_datatree::node_dfs_begin(root);
            auto end = ::Addle::aux_datatree::node_dfs_end(root);
            
            for (; i != end; ::Addle::aux_datatree::node_dfs_increment(i))
            {
                qDebug() << (*i).v;
//                 qDebug() 
//                     << ::Addle::aux_datatree::node_index(i)
//                     << ::Addle::aux_datatree::node_depth(i);
            }
        }
        
//         for (auto&& o : make_nested_object_adapter(t, &TestObject::children))
//         {
// //             qDebug() << o.v;/
//         }
    }
    
    void dev_nestedObjectExtNode()
    {
        struct TestObject
        {
            int v = 0;
            QList<TestObject> children;
        };
        
        TestObject t = {0,{{1,{{2,{}},{3,{}}}},{4,{{5,{}}}}}};
        
        auto adapter = make_nested_object_adapter(t, &TestObject::children);
        
        using ExtNode = aux_datatree::DFSExtendedNode<aux_datatree::node_handle_t<decltype(adapter)>, true, true>;
        
        ExtNode root(datatree_root(adapter));
        
        auto nodeLookedUp = aux_datatree::node_lookup_address(root, aux_datatree::NodeAddress({0, 1}));
        
        qDebug() << (*nodeLookedUp).v;
    }
    
    void dev_observer1()
    {
        BasicDataTree<QString> tree("spiff");
        aux_datatree2::TreeObserver<BasicDataTree<QString>> observer(tree);
        
        
    }
    
    /* === FAST ADDRESS VERIFICATION TESTS === */
    
    // Representing the fast address scheme with F, and addresses a1 of
    // length n and a2 of length m, the address scheme is incorrect if
    // F(a1) < F(a2) and a1 >= a2.
    // 
    // Cases of interest to these tests are:
    //    a1 > a2 and n == m
    //    a1 > a2 and n < m
    //    a1 > a2 and n > m
    //
    // F produces identical outputs for identical inputs, and (assuming all
    // preconditions for numerical limits are met) distinct outputs for all 
    // distinct inputs. Verifying this trivially dismisses the other cases 
    // where F is incorrect.
    //
    // These tests are also interested in verifying that the numerical limits
    // of fast addresses are respected, and that slow addresses are used
    // when these limits are exceeded.
    //
    // The total fast address space (approx. 64 bits) is much too large for  
    // casual unit tests to verify exhaustively so we will focus on the edge 
    // values 0, 1, 254, and 255; and addresses with length <= 9.
    
    void verifyFastAddress_identicalEquivalent()
    {
        // # Verify that identical addresses are equivalent
        
        for (std::size_t n = 0; n <= FASTADDRESS_MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : FASTADDRESS_VALUES )
            {
                aux_datatree::NodeAddress a1;
                for (std::size_t i = 0; i < n; ++i)
                {
                    a1 = std::move(a1) << v1;
                }
                
                //qDebug() << a1;
                
                QVERIFY( a1 == a1 );
                QVERIFY( !(a1 < a1) );
                QVERIFY( !(a1 > a1) );
                
                aux_datatree::NodeAddress a2(a1);
                
                QVERIFY( a1 == a2 );
                QVERIFY( !(a1 < a2) );
                QVERIFY( !(a1 > a2) );
            }
        }
    }
    
    void verifyFastAddress_nonIdenticalNotEquivalent()
    {
        // # Verify that non-identical addresses are not equivalent
        // This is more of a sanity check, not likely to fail unless something
        // really funny is going on.
        
        for (std::size_t n = 1; n <= FASTADDRESS_MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : FASTADDRESS_VALUES )
            {
                for (std::size_t v2 : FASTADDRESS_VALUES )
                {
                    if (v1 == v2)
                        continue;
                    
                    aux_datatree::NodeAddress a1;
                    aux_datatree::NodeAddress a2;
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
    }
    
    // # Verify that F(a1) < F(a2) if a1 < a2 and n == m
    
    void verifyFastAddress_equalLength1()
    {
        // ## a1[i] < a2[i] for i <= n
        
        for (std::size_t n = 1; n <= FASTADDRESS_MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : FASTADDRESS_VALUES )
            {
                for (std::size_t v2 : FASTADDRESS_VALUES )
                {
                    if ( v1 >= v2 )
                        continue;
                    
                    aux_datatree::NodeAddress a1;
                    aux_datatree::NodeAddress a2;
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
    }
    
    void verifyFastAddress_equalLength2()
    {
        // ## a1[0] < a2[0], all other indices are identical
        
        for (std::size_t n = 1; n <= FASTADDRESS_MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : FASTADDRESS_VALUES )
            {
                for (std::size_t v2 : FASTADDRESS_VALUES )
                {
                    for (std::size_t v3 : FASTADDRESS_VALUES )
                    {
                        if ( v1 >= v2 )
                            continue;
                        
                        aux_datatree::NodeAddress a1({ v1 });
                        aux_datatree::NodeAddress a2({ v2 });
                        for (std::size_t i = 1; i < n; ++i)
                        {
                            a1 = std::move(a1) << v3;
                            a2 = std::move(a2) << v3;
                        }
                        
                        //qDebug() << a1 << a2;
                        
                        QVERIFY( a1 != a2 );
                        QVERIFY( a1 < a2 );
                        QVERIFY( !(a1 > a2) );
                    }
                }
            }
        }
    }
    
    void verifyFastAddress_equalLength3()
    {
        // ## a1[n-1] < a2[n-1] (last indices), all other indices are identical
        
        for (std::size_t n = 1; n <= FASTADDRESS_MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : FASTADDRESS_VALUES )
            {
                for (std::size_t v2 : FASTADDRESS_VALUES )
                {
                    for (std::size_t v3 : FASTADDRESS_VALUES )
                    {
                        if ( v1 >= v2 )
                            continue;
                        
                        aux_datatree::NodeAddress a1;
                        aux_datatree::NodeAddress a2;
                        for (std::size_t i = 1; i < n; ++i)
                        {
                            a1 = std::move(a1) << v3;
                            a2 = std::move(a2) << v3;
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
    }
    
    void verifyFastAddress_equalLength4()
    {
        // ## a1[n-2] < a2[n-2] (next-to-last indices), all other indices are
        // identical
        
        for (std::size_t n = 2; n <= FASTADDRESS_MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : FASTADDRESS_VALUES )
            {
                for (std::size_t v2 : FASTADDRESS_VALUES )
                {
                    for (std::size_t v3 : FASTADDRESS_VALUES )
                    {
                        if ( v1 >= v2 )
                            continue;
                        
                        aux_datatree::NodeAddress a1;
                        aux_datatree::NodeAddress a2;
                        for (std::size_t i = 2; i < n; ++i)
                        {
                            a1 = std::move(a1) << v3;
                            a2 = std::move(a2) << v3;
                        }
                        
                        a1 = std::move(a1) << v1;
                        a1 = std::move(a1) << v3;
                        
                        a2 = std::move(a2) << v2;
                        a2 = std::move(a2) << v3;
                        
                        //qDebug() << a1 << a2;
                        
                        QVERIFY( a1 != a2 );
                        QVERIFY( a1 < a2 );
                        QVERIFY( !(a1 > a2) );
                    }
                }
            }
        }
    }
    
    // # Verify that F(a1) < F(a2) if a1 < a2 and n < m
    
    void verifyFastAddress_lesserShorterThanGreater1()
    {
        // ## a1 is the empty address and a2 is a value with trailing 0's
        {
            aux_datatree::NodeAddress a1;
            
            for (std::size_t n = 1; n <= FASTADDRESS_MAX_LENGTH; ++n)
            {
                for (std::size_t v1 : FASTADDRESS_VALUES )
                {
                    aux_datatree::NodeAddress a2({ v1 });
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
    }
    
    void verifyFastAddress_lesserShorterThanGreater2()
    {
        // ## a1 is the empty address and a2 is a value with leading 0's
        {
            aux_datatree::NodeAddress a1;
            
            for (std::size_t n = 1; n <= FASTADDRESS_MAX_LENGTH; ++n)
            {
                for (std::size_t v1 : FASTADDRESS_VALUES )
                {
                    aux_datatree::NodeAddress a2;
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
    }
    
    void verifyFastAddress_lesserShorterThanGreater3()
    {
        // ## a1 is a sequence of identical values. a2 is a1 with an additional
        // value appended.
    
        for (std::size_t n = 1; n <= FASTADDRESS_MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : FASTADDRESS_VALUES )
            {
                aux_datatree::NodeAddress a1;
                for (std::size_t i = 0; i < n; ++i)
                {
                    a1 = std::move(a1) << v1;
                }
                
                for (std::size_t v2 : FASTADDRESS_VALUES )
                {
                    aux_datatree::NodeAddress a2(a1);
                    
                    a2 = std::move(a2) << v2;
                    
                    //qDebug() << a1 << a2;
                    
                    QVERIFY( a1 != a2 );
                    QVERIFY( a1 < a2 );
                    QVERIFY( !(a1 > a2) );
                }
            }
        }
    }
    
    // # Verify that F(a1) < F(a2) if a1 < a2 and n > m
    
    void verifyFastAddress_lesserLongerThanGreater1()
    {
        // ## m = 1, a1[0] < a2[0], the rest are identical values
        
        for (std::size_t n = 2; n <= FASTADDRESS_MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : FASTADDRESS_VALUES )
            {
                for (std::size_t v2 : FASTADDRESS_VALUES )
                {
                    if ( v1 >= v2 )
                        continue;
                    
                    for (std::size_t v3 : FASTADDRESS_VALUES )
                    {
                        aux_datatree::NodeAddress a1({ v1 });
                        aux_datatree::NodeAddress a2({ v2 });
                        
                        for (std::size_t i = 1; i < n; ++i)
                        {
                            a1 = std::move(a1) << v3;
                        }
                        
                        //qDebug() << a1 << a2;
                    
                        QVERIFY( a1 != a2 );
                        QVERIFY( a1 < a2 );
                        QVERIFY( !(a1 > a2) );
                    }
                }
            }
        }
    }
    
    void verifyFastAddress_lesserLongerThanGreater2()
    {
        // ## m = n - 1, a1[0] < a2[0], the rest are identical values
        
        for (std::size_t n = 2; n <= FASTADDRESS_MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : FASTADDRESS_VALUES )
            {
                for (std::size_t v2 : FASTADDRESS_VALUES )
                {
                    if ( v1 >= v2 )
                        continue;
                    
                    for (std::size_t v3 : FASTADDRESS_VALUES )
                    {
                        aux_datatree::NodeAddress a1({ v1 });
                        aux_datatree::NodeAddress a2({ v2 });
                        
                        for (std::size_t i = 1; i < n - 1; ++i)
                        {
                            a1 = std::move(a1) << v3;
                            a2 = std::move(a2) << v3;
                        }
                        a1 = std::move(a1) << v3;
                        
                        //qDebug() << a1 << a2;
                    
                        QVERIFY( a1 != a2 );
                        QVERIFY( a1 < a2 );
                        QVERIFY( !(a1 > a2) );
                    }
                }
            }
        }
    }
    
    void verifyFastAddress_lesserLongerThanGreater3()
    {
        // ## m = n - 1, a1[m-1] < a2[m-1], the rest are identical values
        
        for (std::size_t n = 2; n <= FASTADDRESS_MAX_LENGTH; ++n)
        {
            for (std::size_t v1 : FASTADDRESS_VALUES )
            {
                for (std::size_t v2 : FASTADDRESS_VALUES )
                {
                    if ( v1 >= v2 )
                        continue;
                    
                    for (std::size_t v3 : FASTADDRESS_VALUES )
                    {
                        aux_datatree::NodeAddress a1;
                        aux_datatree::NodeAddress a2;
                        
                        for (std::size_t i = 1; i < n - 2; ++i)
                        {
                            a1 = std::move(a1) << v3;
                            a2 = std::move(a2) << v3;
                        }
                        a1 = std::move(a1) << v1;
                        a1 = std::move(a1) << v3;
                        
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
