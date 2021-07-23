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

#include <boost/range/adaptor/reversed.hpp>

#include "utilities/datatree/nestedobjectadapter.hpp"
#include "utilities/datatree/addledatatree.hpp"
#include "utilities/datatree/observer.hpp"
#include "utilities/datatree/views.hpp"

#include "utilities/datatree/echo.hpp"

using namespace Addle;

using aux_datatree::NodeAddress;
    
class DataTree_UTest : public QObject
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
    
    void dev_basicDataTree1()
    {
        int tally = 0;
        
        AddleDataTree<int> sourceTree(tally++);
        //sourceTree.root().setValencyHint(aux_datatree::ValencyHint_Leaf);
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
        
        for (const auto& s : DataTreeNodeChunk { { 1, 2 }, 4 })
            qDebug() << s;
        
        
        
            
            
        //sourceTree.nodeAt({ 99, 99, 99, 99, 99, 99 });
        
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
    
    void dev_mapChunks()
    {
        DataTreeNodeEvent event;
        event.addChunk({ 0 }, 6);
        event.removeChunks({
                {{ 1 }, 1},
                {{ 3 }, 1},
                {{ 5 }, 1},
                {{ 7 }, 1}
            });
        
        event.addChunk({ 0, 1 }, 1);
        
        qDebug() << event.chunksAdded();
        
//         qDebug() << event.affectedParents();
//         qDebug() << event.affectedParentsFromEnd();
    }
    
    void nodeEvent_mapping1()
    {
        DataTreeNodeEvent event;
        event.addChunk({ 1 }, 1);
        
        QCOMPARE(event.mapForward({ 0 }), DataTreeNodeAddress({ 0 }));
        QCOMPARE(event.mapBackward({ 0 }), DataTreeNodeAddress({ 0 }));
        
        QCOMPARE(event.mapForward({ 1 }), DataTreeNodeAddress({ 2 }));
        QCOMPARE(event.mapBackward({ 2 }), DataTreeNodeAddress({ 1 }));
        
        QCOMPARE(event.mapForward({ 2 }), DataTreeNodeAddress({ 3 }));
        QCOMPARE(event.mapBackward({ 3 }), DataTreeNodeAddress({ 2 }));
        
        QCOMPARE(event.mapForward({ 0, 0 }), DataTreeNodeAddress({ 0, 0 }));
        QCOMPARE(event.mapBackward({ 0, 0 }), DataTreeNodeAddress({ 0, 0 }));
        
        QCOMPARE(event.mapForward({ 1, 0 }), DataTreeNodeAddress({ 2, 0 }));
        QCOMPARE(event.mapBackward({ 2, 0 }), DataTreeNodeAddress({ 1, 0 }));
        
        QCOMPARE(event.mapForward({ 2, 0 }), DataTreeNodeAddress({ 3, 0 }));
        QCOMPARE(event.mapBackward({ 3, 0 }), DataTreeNodeAddress({ 2, 0 }));
        
//         for (auto c : event)
//             qDebug() << c.operatingChunk;
    }
    
    void nodeEvent_mapping2()
    {
        DataTreeNodeEvent event;
        event.addChunk({ 0 }, 4);
        
        QCOMPARE(event.mapForward({ 0 }), DataTreeNodeAddress({ 4 }));
        QCOMPARE(event.mapBackward({ 4 }), DataTreeNodeAddress({ 0 }));
        
//         for (auto c : event)
//             qDebug() << c.operatingChunk;
    }
    
//     void nodeEvent_mapping3()
//     {
//         DataTreeNodeEvent event;
//         event.addChunks({ {{ 0 }, 1 }, {{ 0, 0 }, 1 }});
// 
//         QCOMPARE(event.mapForward({ 0, 0 }), DataTreeNodeAddress({ 1, 1 }));
//         QCOMPARE(event.mapBackward({ 1, 1 }), DataTreeNodeAddress({ 0, 0 }));
//         
//         QCOMPARE(event.mapForward({ 1, 0 }), DataTreeNodeAddress({ 2, 0 }));
//         QCOMPARE(event.mapBackward({ 2, 0 }), DataTreeNodeAddress({ 1, 0 }));
//         
//         QCOMPARE(event.mapForward({ 2, 0 }), DataTreeNodeAddress({ 3, 0 }));
//         QCOMPARE(event.mapBackward({ 3, 0 }), DataTreeNodeAddress({ 2, 0 }));
//         
// //         for (auto c : event)
// //             qDebug() << c.operatingChunk;
//     }
    
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
    
    void dev_echoSubTree()
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
            
        /**
         * 0 - 1
         * 2
         * 3 -\ `
         *    |- 4
         *    |- 5
         */
        
        auto adapter = aux_datatree::make_root_range_nested_object_adapter(rootRange, &TestObject::children);
        
        AddleDataTree<int> destTree;
        
        destTree.root().appendChildren({ 9, 8, 7, 6 });
        
        auto r = aux_datatree::echo_subtree<decltype(adapter), AddleDataTree<int>>(
                aux_datatree::tree_root(adapter),
                &destTree.root(),
                destTree.root().children().begin() + 3,
                &TestObject::v
            );
        
//         for (auto& n : destTree.root().children())
//         {
//             qDebug() << n.address() << n.value();
//         }
//         
//         for (auto& n : destTree)
//         {
//             qDebug() << n.address() << n.value();
//         }
        
        for (auto& n : r)
        {
            qDebug() << n.address() << n.value();
        }
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
        qDebug() << *state.mapToCurrent({ 0 });
        qDebug() << *state.mapFromCurrent({ 4 });
        QVERIFY(state.next());
        
        QVERIFY(state.event() == e2);
        qDebug() << *state.mapToCurrent({ 0 });
        qDebug() << *state.mapFromCurrent({ 2 });
        QVERIFY(!state.next());
        
        //state.clear();
    }
    
    void dev_addleDataTreeWithObserver()
    {
        AddleDataTree<QString, true> tree;
        
        tree.observer().startRecording();
        
        tree.root().appendChild("foo");
        
        auto e = tree.observer().finishRecording();
        
        qDebug() << "foo";
    }
    
    void dev_constView()
    {
        struct TestObject { QString v; };
        
        AddleDataTree<QSharedPointer<TestObject>, true> tree1;
        DataTreeCastView<AddleDataTree<QSharedPointer<TestObject>, true>, QSharedPointer<const TestObject>> tree2(tree1);
        
        tree1.root().appendChild( QSharedPointer<TestObject>(new TestObject { "Spiff" }) );
        tree1.root().appendChild( QSharedPointer<TestObject>(new TestObject { "Freem" }) );
        tree1.root().appendChild( QSharedPointer<TestObject>(new TestObject { "Zarg" }) );
        
        auto i = tree2.root().children().begin();
        auto end = tree2.root().children().end();
        for (; i != end; ++i)
        {
            qDebug() << (*i).value()->v;
        }
        
        aux_datatree::NodeRef<
            DataTreeCastView<AddleDataTree<QSharedPointer<TestObject>, true>, QSharedPointer<const TestObject>>,
            true
        > ref(tree2.observer(), tree2.root()[0].node());
        
        QVERIFY(ref.isValid());
        tree1.root().removeChildren(0, 1);
        QVERIFY(!ref.isValid());
    }
    
    void dev_nodeRef()
    {
        AddleDataTree<QString, true> tree;
        
        auto& node = tree.root().appendChild("Spiff");
        
        auto ref = node.nodeRef();
        
        QVERIFY(ref.isValid());
        QCOMPARE((*ref).value(), "Spiff");
        
        //tree.observer().startRecording();
        tree.root().removeChildren(0, 1);
        //tree.observer().finishRecording();
        
        QVERIFY(!ref.isValid());
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
