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

#include <QFile>

#include "utilities/datatree/nestedobjectadapter.hpp"
#include "utilities/datatree/addledatatree.hpp"
#include "utilities/datatree/observer.hpp"
#include "utilities/datatree/views.hpp"

#include "utilities/datatree/echo.hpp"

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
        int tally = 0;
        
        AddleDataTree<int> sourceTree(tally++);
        for (int i = 0; i < 6; ++i)
        {
            auto& branch = sourceTree.root().appendChild(tally++);
            
            for (int j = 0; j < 6; ++j)
            {
                auto& branch2 = branch.appendChild(tally++);
                for (int k = 0; k < 6; ++k)
                    branch2.appendChild(tally++);
            }
        }
        
//         AddleDataTree<QString> destTree;
//         
//         auto echoer = aux_datatree::make_bfs_tree_echoer(
//                 sourceTree,
//                 destTree,
//                 [] () {}
//             );
//         
//         qDebug() << sizeof(echoer);
    }
    
//     void dev_eventBuilder1()
//     {
//         DataTreeNodeEventBuilder builder;
//         builder.addChunk({ 0 }, 1);
//         builder.removeChunk({ 999 }, 1);
//         builder.addChunk({ 0 }, 1);
//         builder.removeChunk({ 999 }, 1);
//         builder.addChunk({ 0 }, 1);
//         builder.removeChunk({ 999 }, 1);
//         builder.addChunk({ 0 }, 1);
//         
//         qDebug() << builder.event().mapForward({ 0 });
//         qDebug() << builder.event().mapBackward({ 4 });
//     }
    
    void nodeEvent_mapping1()
    {
        DataTreeNodeEventBuilder builder;
        builder.addChunk({ 1 }, 1);
        
        QCOMPARE(builder.event().mapForward({ 0 }), DataTreeNodeAddress({ 0 }));
        QCOMPARE(builder.event().mapBackward({ 0 }), DataTreeNodeAddress({ 0 }));
        
        QCOMPARE(builder.event().mapForward({ 1 }), DataTreeNodeAddress({ 2 }));
        QCOMPARE(builder.event().mapBackward({ 2 }), DataTreeNodeAddress({ 1 }));
        
        QCOMPARE(builder.event().mapForward({ 2 }), DataTreeNodeAddress({ 3 }));
        QCOMPARE(builder.event().mapBackward({ 3 }), DataTreeNodeAddress({ 2 }));
        
        QCOMPARE(builder.event().mapForward({ 0, 0 }), DataTreeNodeAddress({ 0, 0 }));
        QCOMPARE(builder.event().mapBackward({ 0, 0 }), DataTreeNodeAddress({ 0, 0 }));
        
        QCOMPARE(builder.event().mapForward({ 1, 0 }), DataTreeNodeAddress({ 2, 0 }));
        QCOMPARE(builder.event().mapBackward({ 2, 0 }), DataTreeNodeAddress({ 1, 0 }));
        
        QCOMPARE(builder.event().mapForward({ 2, 0 }), DataTreeNodeAddress({ 3, 0 }));
        QCOMPARE(builder.event().mapBackward({ 3, 0 }), DataTreeNodeAddress({ 2, 0 }));
    }
    
    void nodeEvent_mapping2()
    {
        DataTreeNodeEventBuilder builder;
        builder.addChunk({ 0 }, 4);
        
        QCOMPARE(builder.event().mapForward({ 0 }), DataTreeNodeAddress({ 4 }));
        QCOMPARE(builder.event().mapBackward({ 4 }), DataTreeNodeAddress({ 0 }));
    }
    
    void nodeEvent_mapping3()
    {
        DataTreeNodeEventBuilder builder;
        builder.addChunks({ {{ 0 }, 1 }, {{ 0, 0 }, 1 }});

        QCOMPARE(builder.event().mapForward({ 0, 0 }), DataTreeNodeAddress({ 1, 1 }));
        QCOMPARE(builder.event().mapBackward({ 1, 1 }), DataTreeNodeAddress({ 0, 0 }));
        
        QCOMPARE(builder.event().mapForward({ 1, 0 }), DataTreeNodeAddress({ 2, 0 }));
        QCOMPARE(builder.event().mapBackward({ 2, 0 }), DataTreeNodeAddress({ 1, 0 }));
        
        QCOMPARE(builder.event().mapForward({ 2, 0 }), DataTreeNodeAddress({ 3, 0 }));
        QCOMPARE(builder.event().mapBackward({ 3, 0 }), DataTreeNodeAddress({ 2, 0 }));
    }
    
    void dev_nestedObjectAdapter()
    {
        struct TestObject
        {
            int v = 0;
            QList<TestObject> children;
            
            //const QList<TestObject> children() const { return _children; }
        };
        
        //TestObject t = {0,{{1,{{2,{}},{3,{}}}},{4,{{5,{}}}}}};
        /**
         * 0
         * |--\ `
         * 1   4
         * |\  |
         * 2 3 5
         */
        
        
        QList<TestObject> rootRange = {
                { 0, {{ 1, {} }}}, { 2, {} }, { 3, {{ 4, {} }, { 5, {} }}} 
            };
        
        auto adapter = aux_datatree::make_root_range_nested_object_adapter(rootRange, &TestObject::children);
                
//         using source_handle_t = decltype(adapter)::handle_t;
//         
        AddleDataTree<int> destTree;
        
        aux_datatree::echo_tree(
                adapter,
                destTree,
                &TestObject::v
            );
        
        for (const auto& o : adapter)
            qDebug() << o.value().v;
        
        for (auto& node : destTree)
            qDebug() << node.address() << node.value();
    }
    
    void dev_observedTreeState()
    {
        AddleDataTree<QString> tree;
        aux_datatree::TreeObserver<AddleDataTree<QString>> observer(tree);
        aux_datatree::ObservedTreeState state(observer);
        
        observer.startRecording();
        observer.insertNodes(
                &tree.root(), 
                tree.root().children().begin(),
                { "spiff", "freem" }
            );
        auto e1 = observer.finishRecording();
        QVERIFY(state.event() == e1);
        
        observer.startRecording();
        observer.insertNodes(
                &tree.root(), 
                tree.root().children().begin(),
                { "spiff2", "freem3" }
            );
        auto e2 = observer.finishRecording();
        
        QVERIFY(state.event() == e1);
        qDebug() << state.mapToCurrent({ 0 });
        qDebug() << state.mapFromCurrent({ 4 });
        QVERIFY(state.next());
        
        QVERIFY(state.event() == e2);
        qDebug() << state.mapToCurrent({ 0 });
        qDebug() << state.mapFromCurrent({ 2 });
        QVERIFY(!state.next());
        
        //state.clear();
    }
    
    void dev_addleDataTreeWithObserver()
    {
        AddleDataTree<QString, true> tree;
        
        
    }
    
//     void dev_nestedObjectExtNode()
//     {
//         struct TestObject
//         {
//             int v = 0;
//             QList<TestObject> children;
//         };
//         
//         TestObject t = {0,{{1,{{2,{}},{3,{}}}},{4,{{5,{}}}}}};
//         
//         auto adapter = make_nested_object_adapter(t, &TestObject::children);
//         
//         using ExtNode = aux_datatree::DFSExtendedNode<aux_datatree::node_handle_t<decltype(adapter)>, true, true>;
//         
//         ExtNode root(datatree_root(adapter));
//         
//         auto nodeLookedUp = aux_datatree::node_lookup_address(root, aux_datatree::NodeAddress({0, 1}));
//         
//         qDebug() << (*nodeLookedUp).v;
//     }
    
//     void dev_observer1()
//     {
// 
//     }

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
                aux_datatree::NodeAddressBuilder a1_;
                for (std::size_t i = 0; i < n; ++i)
                    a1_.append(v1);
                
                auto a1 = a1_.address();
                
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
                    
                    aux_datatree::NodeAddressBuilder a1_;
                    aux_datatree::NodeAddressBuilder a2_;
                    for (std::size_t i = 0; i < n; ++i)
                    {
                        a1_.append(v1);
                        a2_.append(v2);
                    }
                    
                    auto a1 = a1_.address();
                    auto a2 = a2_.address();
                    
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
                    
                    aux_datatree::NodeAddressBuilder a1_;
                    aux_datatree::NodeAddressBuilder a2_;
                    for (std::size_t i = 0; i < n; ++i)
                    {
                        a1_.append(v1);
                        a2_.append(v2);
                    }
                    
                    auto a1 = a1_.address();
                    auto a2 = a2_.address();
                    
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
                        
                        aux_datatree::NodeAddressBuilder a1_({ v1 });
                        aux_datatree::NodeAddressBuilder a2_({ v2 });
                        for (std::size_t i = 1; i < n; ++i)
                        {
                            a1_.append(v3);
                            a2_.append(v3);
                        }
                        
                        auto a1 = a1_.address();
                        auto a2 = a2_.address();
                        
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
                        
                        aux_datatree::NodeAddressBuilder a1_;
                        aux_datatree::NodeAddressBuilder a2_;
                        for (std::size_t i = 1; i < n; ++i)
                        {
                            a1_.append(v3);
                            a2_.append(v3);
                        }
                        
                        a1_.append(v1);
                        a2_.append(v2);
                        
                        auto a1 = a1_.address();
                        auto a2 = a2_.address();
                        
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
                        
                        aux_datatree::NodeAddressBuilder a1_;
                        aux_datatree::NodeAddressBuilder a2_;
                        for (std::size_t i = 2; i < n; ++i)
                        {
                            a1_.append(v3);
                            a2_.append(v3);
                        }
                        
                        a1_.append(v1);
                        a1_.append(v3);
                        
                        a2_.append(v2);
                        a2_.append(v3);
                        
                        auto a1 = a1_.address();
                        auto a2 = a2_.address();
                        
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
                    aux_datatree::NodeAddressBuilder a2_({ v1 });
                    for (std::size_t i = 1; i < n; ++i)
                    {
                        a2_.append(0);
                    }
                    
                    auto a2 = a2_.address();
                        
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
                    aux_datatree::NodeAddressBuilder a2_;
                    for (std::size_t i = 1; i < n; ++i)
                    {
                        a2_.append(0);
                    }
                    
                    a2_.append(v1);
                    
                    auto a2 = a2_.address();
                        
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
                aux_datatree::NodeAddressBuilder a1_;
                for (std::size_t i = 0; i < n; ++i)
                {
                    a1_.append(v1);
                }
                
                auto a1 = a1_.address();
                
                for (std::size_t v2 : FASTADDRESS_VALUES )
                {                    
                    aux_datatree::NodeAddress a2 
                        = aux_datatree::NodeAddressBuilder(a1) << v2;
                                            
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
                        aux_datatree::NodeAddressBuilder a1_({ v1 });
                        aux_datatree::NodeAddressBuilder a2_({ v2 });
                        for (std::size_t i = 1; i < n; ++i)
                        {
                            a1_.append(v3);
                        }
                        
                        auto a1 = a1_.address();
                        auto a2 = a2_.address();
                        
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
                        aux_datatree::NodeAddressBuilder a1_({ v1 });
                        aux_datatree::NodeAddressBuilder a2_({ v2 });
                        for (std::size_t i = 1; i < n - 1; ++i)
                        {
                            a1_.append(v3);
                            a2_.append(v3);
                        }
                        a1_.append(v3);
                        
                        auto a1 = a1_.address();
                        auto a2 = a2_.address();
                        
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
                        aux_datatree::NodeAddressBuilder a1_;
                        aux_datatree::NodeAddressBuilder a2_;
                        
                        for (std::size_t i = 1; i < n - 2; ++i)
                        {
                            a1_.append(v3);
                            a2_.append(v3);
                        }
                        a1_.append(v1);
                        a1_.append(v3);
                        
                        a2_.append(v2);
                          
                        auto a1 = a1_.address();
                        auto a2 = a2_.address();
                        
                        //qDebug() << a1 << a2;
                    
                        QVERIFY( a1 != a2 );
                        QVERIFY( a1 < a2 );
                        QVERIFY( !(a1 > a2) );
                    }
                }
            }
        }
    }
    
    
    void benchmark_makeAddresses()
    {
        if (!loadBenchmarkAddressIndices()) 
            QSKIP("No benchmark data available.");
        
        std::vector<NodeAddress> result;
        result.reserve(_benchmarkAddressIndices.size());
        
        QBENCHMARK {
            for (const auto& indices : qAsConst(_benchmarkAddressIndices))
            {
                aux_datatree::NodeAddressBuilder builder;
                builder.reserve(indices.size());
                for (const std::size_t i : indices)
                    builder.append(i);
                result.push_back(std::move(builder));
            }
        }
        
        Q_UNUSED(result);
    }
    
    void benchmark_sortAddresses()
    {
        if (!loadBenchmarkAddressIndices()) 
            QSKIP("No benchmark data available.");
        
        std::vector<NodeAddress> addresses;
        for (const auto& indices : qAsConst(_benchmarkAddressIndices))
        {
            aux_datatree::NodeAddressBuilder builder;
            builder.reserve(indices.size());
            for (const std::size_t i : indices)
                builder.append(i);
            addresses.push_back(std::move(builder));
        }
        
        QBENCHMARK {
            std::sort(addresses.begin(), addresses.end());
        }
    
        QVERIFY(std::is_sorted(addresses.begin(), addresses.end()));
    }
    
    void benchmark_orderedSetOfAddresses()
    {
        if (!loadBenchmarkAddressIndices()) 
            QSKIP("No benchmark data available.");
        
        std::vector<NodeAddress> addresses;
        for (const auto& indices : qAsConst(_benchmarkAddressIndices))
        {
            aux_datatree::NodeAddressBuilder builder;
            builder.reserve(indices.size());
            for (const std::size_t i : indices)
                builder.append(i);
            addresses.push_back(std::move(builder));
        }
        
        std::set<NodeAddress> result;
        
        QBENCHMARK {
            for (const auto& address : addresses)
            {
                result.insert(address);
            }
        }
    
        QVERIFY(std::is_sorted(result.begin(), result.end()));
    }
    
    void benchmark_hashAddresses()
    {
        if (!loadBenchmarkAddressIndices()) 
            QSKIP("No benchmark data available.");
        
        std::vector<NodeAddress> addresses;
        for (const auto& indices : qAsConst(_benchmarkAddressIndices))
        {
            aux_datatree::NodeAddressBuilder builder;
            builder.reserve(indices.size());
            for (const std::size_t i : indices)
                builder.append(i);
            addresses.push_back(std::move(builder));
        }

        QBENCHMARK {
            for (const auto& address : addresses)
            {
                auto result = qHash(address);
                Q_UNUSED(result);
            }
        }
    }
    
    void benchmark_hashSetOfAddresses()
    {
        if (!loadBenchmarkAddressIndices()) 
            QSKIP("No benchmark data available.");
        
        std::vector<NodeAddress> addresses;
        for (const auto& indices : qAsConst(_benchmarkAddressIndices))
        {
            aux_datatree::NodeAddressBuilder builder;
            builder.reserve(indices.size());
            for (const std::size_t i : indices)
                builder.append(i);
            addresses.push_back(std::move(builder));
        }
                
        QSet<NodeAddress> result;
        
        QBENCHMARK {
            for (const auto& address : addresses)
            {
                result.insert(address);
            }
        }
    
        Q_UNUSED(result);
    }
    
private:
    bool loadBenchmarkAddressIndices()
    {
        if (!_benchmarkAddressIndices.empty())
            return true;
        
        QFile f("datatree_utest_addressBenchmarkData.txt");
        if (!f.exists() || !f.open(QIODevice::ReadOnly))
            return false;
        
        while (true)
        {
            QByteArray line = f.readLine();
            std::vector<std::size_t> lineVals;
            
            for (QByteArray segment : line.split(','))
            {
                lineVals.push_back(segment.toULong());
            }
            
            _benchmarkAddressIndices.push_back(std::move(lineVals));
            
            if (line.isEmpty())
                break;
        }
        
        return !_benchmarkAddressIndices.empty();
    }
    
    std::vector<std::vector<std::size_t>> _benchmarkAddressIndices;
};

QTEST_MAIN(DataTree_UTest)

#include "datatree_utest.moc"
