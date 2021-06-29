#pragma once

#include <algorithm>
#include <any>
#include <exception>
#include <deque>
#include <optional>
#include <memory>
#include <map>
#include <vector>

#include <stdint.h> // SIZE_MAX

#include <QAtomicInt>
#include <QSharedData>
#include <QReadWriteLock>
#include <QMutex>

#include <QMetaType>

#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include "./aux.hpp"

#ifdef ADDLE_TEST
class DataTree_UTest;
#endif

namespace Addle {
namespace aux_datatree {
    
template<typename Tree>
class TreeObserver;

class TreeObserverData;
class ObservedTreeState;

template<bool additive>
extern NodeAddress mapSingle_impl(
        const NodeAddress& from,
        const NodeChunk& rel,
        bool* terminal = nullptr);

/**
 * Describes a change in the node structure of a data tree. Nodes may be added,
 * removed, or moved; and the event may describe multiple such operations
 * performed in sequence. 
 * 
 * The NodeEvent is agnostic of the implementation of the tree and does not rely
 * on references to any internal data structure of any tree, making it safe to 
 * use to process structural changes after they have already happened.
 * 
 * NodeEvent can be created automatically by performing operations on a tree 
 * through TreeObserver. Events created this way are "coordinated" with each 
 * other and the observer that created them. This can allow for greater async
 * safety, such as read/write locking the tree and fully safe access to the
 * underlying tree data.
 * 
 * NodeEvent can also be created with NodeEventBuilder, in which case they are
 * not coordinated.
 */
class NodeEvent
{
    class Step;

    // sorted set
    using chunk_list_t = std::vector<NodeChunk>;
    // collection of multiple sorted sets of sibling nodes, arranged by parent
    using chunk_map_t = std::map<NodeAddress, chunk_list_t>;
    
    using initial_child_counts_t = std::unordered_map<NodeAddress, std::size_t>;
    
public:    
    enum NodeOperation 
    {
        Add,
        Remove,
        Move
    };
    
    NodeEvent() = default;
    
    template<
        typename NodeHandle, 
        typename std::enable_if_t<
            aux_datatree::is_node_handle<NodeHandle>::value,
        void*> = nullptr>
    NodeEvent(NodeHandle root)
        : _data(new Data (root))
    {
    }
    
    NodeEvent(const NodeEvent&) = default;
    NodeEvent& operator=(const NodeEvent&) = default;
    
    NodeEvent(NodeEvent&&) = default;
    NodeEvent& operator=(NodeEvent&&) = default;
    
    bool operator==(const NodeEvent& other) const
    {
        return _data.data() == other._data.data()
            || (
                _data->steps == other._data->steps
                && _data->totalChunkCount == other._data->totalChunkCount
            );
    }
    
    bool operator!=(const NodeEvent& other) const { return !(*this == other); }
    
    NodeAddress mapForward(
            NodeAddress from, 
            std::size_t progress = SIZE_MAX,
            bool* wasDeleted = nullptr) const  
    {
        if (wasDeleted)
            *wasDeleted = false;
        
        if (!_data || from.isRoot() || !progress) 
            return from;

        progress = std::min(_data->totalChunkCount, progress);
        
        NodeAddress result(from);
        
        for (const Step& step : _data->steps)
        {
            result = step.mapForward(result, progress, wasDeleted);
            
            if (progress <= step.chunkCount())
                break;
            else
                progress -= step.chunkCount();
        }
        
        return result;
    }

    NodeAddress mapBackward(
            NodeAddress from, 
            std::size_t progress = SIZE_MAX,
            bool* wasAdded = nullptr) const
    {
        using namespace boost::adaptors;
        
        if (wasAdded)
            *wasAdded = false;
        
        if (!_data || from.isRoot()) 
            return from;
        
        progress = std::min(_data->totalChunkCount, progress);
        std::size_t progressCount = 0;
        
        NodeAddress result(from);
        
        {
            auto i = _data->steps.begin();
            auto end = _data->steps.end();
            
            for (; i != end; ++i)
            {
                if (progressCount + (*i).chunkCount() > progress)
                    break;
                else
                    progressCount += (*i).chunkCount();
            }
                
            auto j =    std::make_reverse_iterator(i);
            auto rend = std::make_reverse_iterator(_data->steps.begin());
            for(; j != rend; ++j)
            {
                result = (*j).mapBackward(result, progress, wasAdded);
                progress -= (*j).chunkCount();
            }
        }
        
        return result;
    }
    
    inline bool wasAdded(NodeAddress address, 
            std::size_t progress = SIZE_MAX) const
    {
        bool result;
        mapBackward(address, progress, &result);
        
        return result;
    }
    
    inline bool isDeleted(NodeAddress address, 
            std::size_t progress = SIZE_MAX) const
    {
        bool result;
        mapForward(address, progress, &result);
        
        return result;
    }
    
    // TODO: The amount of information we track is probably insufficient to be
    // useful and the amount that would be useful is probably impractical.
    std::size_t childCount(
            NodeAddress parent, 
            std::size_t progress = SIZE_MAX,
            bool* wasAffected = nullptr) const
    {
        if (wasAffected)
            *wasAffected = false;
        
        if (!_data) return 0;
        
        std::ptrdiff_t result = 0;
        
        for (const Step& step : _data->steps)
        {
            auto find = step.initialChildCounts().find(parent);
            if (find != step.initialChildCounts().end())
                result = (*find).second;
            
            auto offset = step.childCountOffset(parent, progress);
            
            if (wasAffected && offset != 0)
                *wasAffected = true;
            
            result += offset;
            
            bool wasDeleted = false;
            parent = step.mapForward(
                    std::move(parent),
                    SIZE_MAX,
                    &wasDeleted
                );
            
            if (wasDeleted)
            {
                if (wasAffected)
                    *wasAffected = true;
                return 0;
            }
            
            if (progress > step.chunkCount())
                progress -= step.chunkCount();
            else
                break;
        }
        
        return std::max(result, (std::ptrdiff_t)0);
    }
    
private:
    class Step
    {
    public:
        Step(NodeOperation operation)
            : _operation(operation)
        {
        }
        
        bool operator==(const Step& other) const
        {
            return _operation       == other._operation
                && _chunkCount      == other._chunkCount
                && (
                    _operation != Move || (
                        _moveSource == other._moveSource 
                        && _moveDest == other._moveDest
                    )
                )
                && _primaryChunks   == other._primaryChunks
                && _secondaryChunks == other._secondaryChunks;
        }
        
        inline NodeOperation operation() const { return _operation; }
        
        inline std::size_t chunkCount() const { return _chunkCount; }
        
        const chunk_map_t& primaryChunks() const { return _primaryChunks; }
        const chunk_list_t& secondaryChunks() const { return _secondaryChunks; }
        
        const initial_child_counts_t& initialChildCounts() const { return _initialChildCounts; }
        
        NodeAddress mapForward(
                const NodeAddress& from, 
                std::size_t progress = SIZE_MAX,
                bool* wasDeleted = nullptr) const
        {
            if (from.isRoot()) return NodeAddress();
                        
            switch(_operation)
            {
                case NodeOperation::Add:
                    return primaryMap_impl<true>(from, progress);
                case NodeOperation::Remove:
                    return primaryMap_impl<false, true>(from, progress, wasDeleted);
                
        //                 case NodeOperation::Move: // TODO
                    
                default:
                    Q_UNREACHABLE();
            }
        }
        
        NodeAddress mapBackward(
                const NodeAddress& from, 
                std::size_t progress = SIZE_MAX,
                bool* wasAdded = nullptr) const
        {
            if (_primaryChunks.empty())
                return from;
            
            switch(_operation)
            {
                case NodeOperation::Add:
                    return primaryMap_impl<false>(from, progress, wasAdded);
                case NodeOperation::Remove:
                    return primaryMap_impl<true, true>(from, progress);
                
        //                 case NodeOperation::Move: // TODO
                    
                default:
                    Q_UNREACHABLE();
            }
        }
        
        std::ptrdiff_t childCountOffset(
                NodeAddress parent, 
                std::size_t progress = SIZE_MAX) const;
        
        // Adjusts relevant addresses in this Add step as if `rel` had been 
        // inserted first. (this is occasionally necessary if merging `rel` 
        // would require an address with a negative index.)
        void rebase(const NodeChunk& rel);
        
        // Adds a chunk to this step, eliding or merging it with existing chunks
        // as necessary.
        void addPrimaryChunk(NodeChunk&& chunk);
        
    private:
                
        template<bool additive, bool progressReversed = false>
        NodeAddress primaryMap_impl(
                const NodeAddress& from, 
                std::size_t progress = SIZE_MAX, 
                bool* terminal = nullptr
            ) const;
        NodeOperation _operation;
        
        std::size_t _chunkCount = 0;
        
        chunk_map_t _primaryChunks;
        chunk_list_t _secondaryChunks;
        
        initial_child_counts_t _initialChildCounts;
        
        NodeChunk _moveSource;
        NodeAddress _moveDest;
    };
    
    class StepBuilder
    {
    public:
        StepBuilder(Step& step, std::size_t& totalChunkCount, bool isNew = false)
            : _step(step), _totalChunkCount(totalChunkCount), _isNew(isNew)
        {
        }
        
        void addPrimaryChunk(NodeChunk chunk);
        
    private:
        Step& _step;
        std::size_t& _totalChunkCount;
        bool _isNew;
    };
    
    struct Data : QSharedData
    {
        QList<Step> steps;
        std::size_t totalChunkCount = 0;
    };
    
    void initData()
    {
        if (!_data)
            _data = new Data;
        else
            _data.detach();
    }
    
    StepBuilder addStep(NodeOperation type);    
    
    inline void addChunk_impl(NodeAddress address, std::size_t length)
    {
        auto handle = addStep(NodeOperation::Add);
        handle.addPrimaryChunk({ std::move(address), length });
    }
    
    template<typename Range>
    void addChunks_impl(Range&& range);
    
    inline void removeChunk_impl(NodeAddress address, std::size_t length)
    {
        auto handle = addStep(NodeOperation::Remove);
        handle.addPrimaryChunk({ std::move(address), length });
    }
    
    template<typename Range>
    void removeChunks_impl(Range&& range);
    
//     void setInitialChildCount_impl(
//             NodeAddress address, 
//             std::size_t size, 
//             bool ignoreIfSet = true)
//     {
//         initData();
//         
//         if (!ignoreIfSet || !_data->initialChildCounts.contains(address))
//             _data->initialChildCounts[address] = size;
//     }
    
    QSharedDataPointer<Data> _data;
    
    template<typename> friend class TreeObserver;
    template<class> friend class NodeEventTreeHelper;
    friend class TreeObserverData;
    friend class ObservedTreeState;
    friend class NodeEventBuilder;
    
#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
};

template<typename Range>
void NodeEvent::addChunks_impl(Range&& range)
{
    static_assert(
        std::is_convertible_v<
            typename boost::range_reference<
                boost::remove_cv_ref_t<Range>
            >::type,
            NodeChunk
        >,
        "This method may only be used for ranges of NodeChunk"
    );
    
    if (!boost::empty(range))
    {
        auto handle = addStep(NodeOperation::Add);
        
        QVarLengthArray<NodeChunk, 64> buffer(
                std::begin(range), 
                std::end(range)
            );
        std::sort(buffer.begin(), buffer.end());
        // sorting beforehand is by no means required, but it guarantees a
        // best-case performance for addPrimaryChunk
        
        for (NodeChunk& entry : buffer)
            handle.addPrimaryChunk(std::move(entry));
    }
}

template<typename Range>
void NodeEvent::removeChunks_impl(Range&& range)
{
    static_assert(
        std::is_convertible_v<
            typename boost::range_reference<
                boost::remove_cv_ref_t<Range>
            >::type,
            NodeChunk
        >,
        "This method may only be used for ranges of NodeChunk"
    );
    
    if (!boost::empty(range))
    {
        auto handle = addStep(NodeOperation::Remove);
        
        QVarLengthArray<NodeChunk, 64> buffer(
                std::begin(range), 
                std::end(range)
            );
        std::sort(buffer.begin(), buffer.end());
        
        for (NodeChunk& entry : buffer)
            handle.addPrimaryChunk(std::move(entry));
    }
}

class NodeEventBuilder
{
public:
    NodeEventBuilder() = default;
    
    template< typename Tree_>
    NodeEventBuilder(Tree_& tree)
        : _event(::Addle::aux_datatree::tree_root(tree))
    {
    }
    
    NodeEventBuilder(NodeEventBuilder&&) = default;
    NodeEventBuilder& operator=(NodeEventBuilder&&) = default;
    
    NodeEvent event() const { return _event; }
    
    operator const NodeEvent& () const & { return _event; }
    operator NodeEvent () && { return std::move(_event); }
    
    inline NodeEventBuilder& addAddress(DataTreeNodeAddress address)
    {
        _event.addChunk_impl(std::move(address), 1);
        return *this;
    }
    
    template<typename Range>
    inline NodeEventBuilder& addAddresses(Range&& range)
    {
        static_assert(
            std::is_convertible_v<
                typename boost::range_reference<
                    boost::remove_cv_ref_t<Range>
                >::type,
                DataTreeNodeAddress
            >,
            "This method may only be used for ranges of NodeAddress"
        );
        
        if (!boost::empty(range))
        {
            using namespace boost::adaptors;
            _event.addChunks_impl(
                    range 
                    | transformed([] (auto&& address) { 
                        return aux_datatree::NodeChunk {
                                std::forward<decltype(address)>(address), 
                                1 
                            };
                    })
                );
        }
        
        return *this;
    }
    
    inline NodeEventBuilder& addAddresses(
            const std::initializer_list<DataTreeNodeAddress>& addresses
        )
    {
        return addAddresses<
            const std::initializer_list<DataTreeNodeAddress>&
            >(addresses);
    }
    
    inline NodeEventBuilder& addChunk(
            DataTreeNodeAddress address,
            std::size_t length
        )
    {
        _event.addChunk_impl(std::move(address), length);
        return *this;
    }
    
    template<typename Range>
    inline NodeEventBuilder& addChunks(Range&& range)
    {
        _event.addChunks_impl(std::forward<Range>(range));
        return *this;
    }
    
    inline NodeEventBuilder& addChunks(
            const std::initializer_list<aux_datatree::NodeChunk>& chunks
        )
    {
        return addChunks<
                const std::initializer_list<aux_datatree::NodeChunk>&
            >(chunks);
    }
    
    inline NodeEventBuilder& removeAddress(DataTreeNodeAddress address)
    {
        _event.removeChunk_impl(std::move(address), 1);
        return *this;
    }
    
    template<typename Range>
    inline NodeEventBuilder& removeAddresses(Range&& range)
    {
        static_assert(
            std::is_convertible_v<
                typename boost::range_reference<
                    boost::remove_cv_ref_t<Range>
                >::type,
                DataTreeNodeAddress
            >,
            "This method may only be used for ranges of NodeAddress"
        );
        
        if (!boost::empty(range))
        {
            using namespace boost::adaptors;
            _event.removeChunks_impl(
                    range 
                    | transformed([] (auto&& address) { 
                        return aux_datatree::NodeChunk {
                                std::forward<decltype(address)>(address), 
                                1 
                            };
                    })
                );
        }
        
        return *this;
    }
    
    inline NodeEventBuilder& removeAddresses(
            const std::initializer_list<DataTreeNodeAddress>& addresses
        )
    {
        return removeAddresses<
                const std::initializer_list<DataTreeNodeAddress>&
            >(addresses);
    }
    
    inline NodeEventBuilder& removeChunk(
            DataTreeNodeAddress address, 
            std::size_t length
        )
    {
        _event.removeChunk_impl(std::move(address), length);
        return *this;
    }
    
    template<typename Range>
    inline NodeEventBuilder& removeChunks(Range&& range)
    {
        _event.removeChunks_impl(std::forward<Range>(range));
        return *this;
    }
    
    inline NodeEventBuilder& removeChunks(
            const std::initializer_list<aux_datatree::NodeChunk>& chunks
        )
    {
        return removeChunks<
                const std::initializer_list<aux_datatree::NodeChunk>&
            >(chunks);
    }
    
private:
   NodeEvent _event;
};

// TODO: rename
// Modifies a tree and records those modifications in an event.
template<class Tree>
class NodeEventTreeHelper
{
public:
    using handle_t = aux_datatree::node_handle_t<Tree>;
    using child_handle_t = aux_datatree::child_node_handle_t<handle_t>;
    
    NodeEventTreeHelper(handle_t root, NodeEvent& event)
        : _root(root),
        _event(event)
    {
        assert(_root);
    }
    
    NodeEventTreeHelper(const NodeEventTreeHelper&) = delete;
    NodeEventTreeHelper(NodeEventTreeHelper&&) = delete;
    NodeEventTreeHelper& operator=(const NodeEventTreeHelper&) = delete;
    NodeEventTreeHelper& operator=(NodeEventTreeHelper&&) = delete;
    
    // Because tree operations have the potential to throw exceptions, we must
    // avoid actually modifying the state of the event until *after* performing
    // all tree operations.
    
    template<typename Range, 
        typename std::enable_if_t<std::is_convertible_v<
            typename boost::range_category<boost::remove_cv_ref_t<Range>>::type,
            std::forward_iterator_tag
            >,
        void*> = nullptr>
    auto insertNodes(handle_t parent, child_handle_t before, Range&& nodeValues);
    
    template<typename Range, 
        typename std::enable_if_t<!std::is_convertible_v<
            typename boost::range_category<boost::remove_cv_ref_t<Range>>::type,
            std::forward_iterator_tag
        >,
        void*> = nullptr>
    auto insertNodes(handle_t parent, child_handle_t before, Range&& nodeValues)
    {
        using buffer_t = QVarLengthArray<
                typename boost::range_value<boost::remove_cv_ref_t<Range>>::type, 
                64
            >;
        using forward_range_t = boost::iterator_range<
                std::move_iterator<typename buffer_t::iterator>
            >;
        
        buffer_t buffer(std::begin(nodeValues), std::end(nodeValues));
        
        return insertNodes<forward_range_t>(
                parent,
                before,
                forward_range_t(buffer.begin(), buffer.end())
            );
    }
    
    void removeNodes(aux_datatree::ChildNodeRange<Tree> nodes)
    {
        if (Q_UNLIKELY(boost::empty(nodes))) return;
        
        handle_t parent = aux_datatree::node_parent(
                static_cast<handle_t>(nodes.begin())
            );
        std::size_t startIndex = aux_datatree::node_index(
                static_cast<handle_t>(nodes.begin())
            );
        
        removeNodes_impl(parent, startIndex, nodes.size(), 
                         nodes.begin(), nodes.end());
    }
    
    void removeNodes(handle_t parent, std::size_t startIndex, std::size_t count = 1)
    {
        if (Q_UNLIKELY(!count)) return;
        
        assert( count <= aux_datatree::node_child_count(parent) );
        
        if constexpr (
            aux_datatree::_handle_is_iterator_with_category<
                aux_datatree::child_node_handle_t<handle_t>,
                std::random_access_iterator_tag
            >::value
        )
        {
            child_handle_t begin = aux_datatree::node_children_begin(parent);
            removeNodes_impl(
                    parent, 
                    startIndex, 
                    count, 
                    begin + startIndex,
                    begin + startIndex + count
                );
        }
        else // child_node_handle_t is not a random access iterator
        {
            child_handle_t begin = aux_datatree::node_children_begin(parent);
            
            for (std::size_t i = 0; i < startIndex; ++i)
                aux_datatree::node_sibling_increment(begin);
            
            child_handle_t end = begin;
            for (std::size_t i = 0; i < count; ++i)
                aux_datatree::node_sibling_increment(end);
            
            removeNodes_impl(parent, startIndex, count, begin, end);
        }
    }
    
private:
    void removeNodes_impl(
            handle_t parent, 
            std::size_t startIndex,
            std::size_t count,
            child_handle_t begin,
            child_handle_t end
        );
    
    handle_t _root;
    NodeEvent& _event;
};

template<class Tree>
template<typename Range, 
    typename std::enable_if_t<std::is_convertible_v<
        typename boost::range_category<boost::remove_cv_ref_t<Range>>::type,
        std::forward_iterator_tag
        >,
    void*>>
auto NodeEventTreeHelper<Tree>::insertNodes(
        handle_t parent, 
        child_handle_t before, 
        Range&& nodeValues
    )
{
    std::size_t valuesCount = boost::size(nodeValues);
    std::size_t initialChildCount = aux_datatree::node_child_count(parent);
    std::size_t startIndex =
        (before == aux_datatree::node_children_end(parent)) ?
            aux_datatree::node_child_count(parent) :
            aux_datatree::node_index(static_cast<handle_t>(before));
            
    NodeAddress parentAddress = aux_datatree::node_address(parent);
    
    auto result = aux_datatree::node_insert_children(
            parent,
            before,
            std::forward<Range>(nodeValues)
        );
    
//     _event.setInitialChildCount_impl(
//             _event.mapBackward(parentAddress), initialChildCount
//         );
    _event.addChunk_impl( 
            NodeAddressBuilder(parentAddress) << startIndex,
            valuesCount 
        );  
    
    return result;
}

template<class Tree>
void NodeEventTreeHelper<Tree>::removeNodes_impl(
        handle_t parent, 
        std::size_t startIndex,
        std::size_t count,
        child_handle_t begin,
        child_handle_t end
    )
{
    NodeAddress parentAddress = aux_datatree::node_address(parent);
    std::size_t initialChildCount = aux_datatree::node_child_count(parent);
    
    // TODO: secondary nodes
    
    aux_datatree::node_remove_children(
            parent,
            begin,
            end
        );
    
//     _event.setInitialChildCount_impl(
//             _event.mapBackward(parentAddress), initialChildCount
//         );
    _event.removeChunk_impl(
            NodeAddressBuilder(parentAddress) << startIndex, 
            count
        );
}
    
class TreeObserverData
{
    struct EventEntryRefCounter
    {
        QAtomicInteger<unsigned> count;
        QAtomicInteger<unsigned> metaCount;
        
        // only access with a read/write lock
        bool deletedByObserver = false;
    };
    
    struct EventEntry
    {
        NodeEvent event;
        EventEntryRefCounter* refCounter;
    };
    
    using node_event_list = std::list<EventEntry>;
    using node_event_iterator = node_event_list::const_iterator;
    
public:
    
//     // TODO: is this necessary anymore?
//     class EventCollectionIterator 
//         : public boost::iterator_adaptor<
//             EventCollectionIterator,
//             node_event_list::const_iterator,
//             NodeEvent,
//             boost::use_default, // Category 
//             const NodeEvent&
//         >
//     {
//         using base_t = node_event_list::const_iterator;
//         using adaptor_t = boost::iterator_adaptor<
//                 EventCollectionIterator,
//                 node_event_list::const_iterator,
//                 NodeEvent,
//                 boost::use_default,
//                 const NodeEvent&
//             >;
//         
//     public:
//         EventCollectionIterator() = default;
//         EventCollectionIterator(const EventCollectionIterator&) = default;
//         
//     private:
//         EventCollectionIterator(base_t base)
//             : adaptor_t(base)
//         {
//         }
//         
//         const NodeEvent& dereference() const { return (*base()).event; }
//         
//         friend class boost::iterator_core_access;
//         friend class TreeObserverData;
//     };
//     
//     using EventCollection = boost::iterator_range<EventCollectionIterator>;
    
    TreeObserverData(const TreeObserverData&) = delete;
    TreeObserverData(TreeObserverData&&) = delete;
    
    inline ~TreeObserverData();

    inline void startRecording();
    
    inline const NodeEvent& finishRecording();
    
    void onTreeDeleted()
    {
        const QWriteLocker locker(&_lock);
        
        assert(!_isDeleted);
        _isDeleted = true;
    }
    
//     EventCollection events() const
//     {
//         return EventCollection(
//                 EventCollectionIterator(_events.begin()), 
//                 EventCollectionIterator(_current)
//             );
//     }
//     
//     EventCollectionIterator eventsEnd() const { return EventCollectionIterator(_current); }
//     
//     EventCollectionIterator findEvent(const NodeEvent& event) const
//     {
//         if (!event._data || !_eventIndex.contains(event._data->id))
//             return EventCollectionIterator(_current);
//         else
//             return EventCollectionIterator(_eventIndex[event._data->id]);
//     }
    
    std::unique_ptr<QReadLocker> lockForRead() const
    {
        return std::make_unique<QReadLocker>(&_lock);
    }
    
    std::unique_ptr<QWriteLocker> lockForWrite() const
    {
        return std::make_unique<QWriteLocker>(&_lock);
    }
    
private:
    inline TreeObserverData(std::any root);
    void releaseNodeEvent(node_event_iterator id);
    
    std::any _root;
    
    node_event_list _events;
    node_event_list::iterator _current;
    
    bool _isDeleted = false;
    bool _isRecording = false;
    
    NodeEvent _recording;
    
    // semi-public read/write lock corresponding to the read/write lock state
    // of the tree
    mutable QReadWriteLock _lock = QReadWriteLock(QReadWriteLock::Recursive);
    
    // private mutex guarding access to the events list
    mutable QMutex _eventListMutex;
    
    template<typename> friend class TreeObserver;
    friend class ObservedTreeState;
#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
};

inline TreeObserverData::TreeObserverData(std::any root)
    : _root(root)
{
    // One event entry is always kept at the end of the history, essentially
    // representing the "current" state of the tree. After a change to the tree
    // is recorded, this end entry becomes the entry for that change and a new 
    // end entry is added.
    //
    // This way, ObservedTreeState always tracks a distinct position in the 
    // observer's event list with a valid reference counter, even when it is the
    // current state. By changing this observer data, all "current" tree states
    // can quietly be made historical at any time.
    
    _current = _events.emplace(_events.end());
    (*_current).refCounter = new EventEntryRefCounter;
    (*_current).refCounter->metaCount.ref();
}
    
inline TreeObserverData::~TreeObserverData()
{
#ifdef ADDLE_DEBUG
    const QWriteLocker locker(&_lock);
    const QMutexLocker eventListLocker(&_eventListMutex);
    assert(_isDeleted && _current == _events.begin());
#endif
    
    delete (*_current).refCounter;
}

inline void TreeObserverData::startRecording()
{
    const QWriteLocker locker(&_lock);
    
    assert(!_isRecording);
    
    _isRecording = true;
    _recording = NodeEvent();
}
    
inline const NodeEvent& TreeObserverData::finishRecording()
{
    const QWriteLocker locker(&_lock);
    
    assert(_isRecording);
    
    _isRecording = false;
    
    _recording.initData();
    
    const auto& result = ((*_current).event = std::move(_recording));
    
    _eventListMutex.lock();
        
    _current = _events.emplace(_events.end());
    (*_current).refCounter = new EventEntryRefCounter;
    (*_current).refCounter->metaCount.ref();
    
    _eventListMutex.unlock();
    
    return result;
}

template<typename Tree>
class TreeObserver
{
public:
    using handle_t = aux_datatree::node_handle_t<Tree>;
    using child_handle_t = aux_datatree::child_node_handle_t<handle_t>;
    
    TreeObserver(Tree& tree)
        : _root(::Addle::aux_datatree::tree_root(tree)),
        _data(new TreeObserverData(_root))
    {
    }
    
    TreeObserver(const TreeObserver&) = delete;
    TreeObserver(TreeObserver&&) = delete;
    TreeObserver& operator=(const TreeObserver&) = delete;
    TreeObserver& operator=(TreeObserver&&) = delete;
    
    ~TreeObserver()
    {
        _data->onTreeDeleted(); 
    }
    
    void startRecording()
    {
        _data->startRecording();
    }
    
    NodeEvent finishRecording()
    {
        return _data->finishRecording();
    }
    
//     NodeEvent lastNodeEvent() const
//     {
//         const auto lock = _data->lockForRead();
//         
//         if (_data->_current != _data->_events.begin())
//         {
//             auto it = _data->_current;
//             --it;
//             
//             return (*it).event;
//         }
//         else
//         {
//             return NodeEvent();
//         }
//     }
    
    template<typename Range>
    auto insertNodes(
            handle_t parent, 
            child_handle_t before, 
            Range&& nodeValues
         )
    {
        assert(_data->_isRecording);
        
        return NodeEventTreeHelper<Tree>(_root, _data->_recording)
            .template insertNodes<Range>(
                    parent, 
                    before, 
                    std::forward<Range>(nodeValues)
                );
    }
    
    auto insertNodes(
            handle_t parent, 
            child_handle_t before, 
            const std::initializer_list<aux_datatree::node_value_t<Tree>>& values
         )
    {
        return insertNodes<
                const std::initializer_list<aux_datatree::node_value_t<Tree>>&
            >(
                parent, 
                before, 
                values
            );
    }
    
    template<typename Range>
    auto prependNodes(
            handle_t parent, 
            Range&& nodeValues
         )
    {
        return insertNodes(
                parent, 
                aux_datatree::node_children_begin(parent),
                std::forward<Range>(nodeValues)
            );
    }
    
    auto prependNodes(
            handle_t parent, 
            const std::initializer_list<aux_datatree::node_value_t<Tree>>& values
         )
    {
        return prependNodes<
                const std::initializer_list<aux_datatree::node_value_t<Tree>>&
            >(
                parent, 
                values
            );
    }
    
    template<typename Range>
    auto appendNodes(
            handle_t parent, 
            Range&& nodeValues
         )
    {
        return insertNodes(
                parent, 
                aux_datatree::node_children_end(parent),
                std::forward<Range>(nodeValues)
            );
    }
    
    auto appendNodes(
            handle_t parent, 
            const std::initializer_list<aux_datatree::node_value_t<Tree>>& values
         )
    {
        return appendNodes<
                const std::initializer_list<aux_datatree::node_value_t<Tree>>&
            >(
                parent, 
                values
            );
    }
    
    void removeNodes(
            handle_t parent,
            std::size_t startIndex,
            std::size_t count = 1
        )
    {
        assert(_data->_isRecording);
        NodeEventTreeHelper<Tree>(_root, _data->_recording)
            .removeNodes(
                parent, 
                startIndex, 
                count
            );
    }
    
    template<typename Range>
    void removeNodes(Range&& range)
    {
        assert(_data->_isRecording);
        NodeEventTreeHelper<Tree>(_root, _data->_recording)
            .removeNodes(
                std::forward<Range>(range)
            );
    }
    
private:
    ::Addle::aux_datatree::node_handle_t<Tree> _root = {};
    QSharedPointer<TreeObserverData> _data;
    
    friend class ObservedTreeState;
#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
};

template<typename Tree>
using _datatree_observer_t = decltype( datatree_observer(std::declval<Tree>()) );

template<typename Tree, typename std::enable_if_t<boost::mp11::mp_valid<_datatree_observer_t, Tree&&>::value, void*> = nullptr>
inline decltype(auto) tree_observer(Tree&& tree) { return datatree_observer(std::forward<Tree>(tree)); }

// move up?

/**
 * @class ObservedTreeState
 * Represents a distinct state of the structure of a data tree. This state may
 * be current or historical, and will remain consistent even if the tree is 
 * modified. If the state is historical it can safely marshal access to the 
 * current tree.
 *
 * In general, a given ObservedTreeState should be updated as frequently as
 * changes are made to the tree, and at most be behind the current state by 
 * e.g., as many changes as had occurred since the last cycle of a relevant 
 * event loop.
 * 
 * @note The memory cost of DataTreeObserver increases in proportion to the
 * number of events between the **most out-of-date** ObservedTreeState and the
 * current tree. This memory is released only as it ceases to be needed by any 
 * ObservedTreeState objects (i.e., because they have been updated or
 * destroyed), even if the DataTreeObserver itself is destroyed. Use the 
 * `clear()` function to nullify an inactive ObservedTreeState and release its 
 * claim on shared DataTreeObserver memory.
 */
class ObservedTreeState
{
public:
    ObservedTreeState() = default;
    
    template<class Tree>
    ObservedTreeState(const TreeObserver<Tree>& observer)
        : _observerData(observer._data)
    {
        const auto lock = _observerData->lockForRead();
        
        _eventIt = observer._data->_current;
        ref(_eventIt);
    }
    
    ObservedTreeState(const ObservedTreeState& other)
        : _observerData(other._observerData),
        _eventIt(other._eventIt)
    {
        ref(_eventIt);
    }
    
    ObservedTreeState(ObservedTreeState&& other)
        : _eventIt(std::move(other._eventIt))
    {
        _observerData.swap(other._observerData);
    }
    
    ~ObservedTreeState() { deref(_eventIt); }
    
    ObservedTreeState& operator=(const ObservedTreeState& other)
    {
        deref(_eventIt);
        
        _observerData = other._observerData;
        _eventIt = other._eventIt;
        
        ref(_eventIt);
        
        return *this;
    }
    
    ObservedTreeState& operator=(ObservedTreeState&& other)
    {
        _eventIt = std::move(other._eventIt);
        _observerData.swap(other._observerData);
        other._observerData = nullptr;
        
        return *this;
    }
    
    void clear()
    {
        if (_observerData)
        {
            deref(_eventIt);
            _observerData = nullptr;
        }
    }
    
    bool isCurrent() const
    {
        if (!_observerData) return false;
        
        auto lock = _observerData->lockForRead();
        return _eventIt == _observerData->_current;
    }
    
    bool next();
    
    NodeEvent event() const
    {
        if (!_observerData) return NodeEvent();
        
        auto lock = _observerData->lockForRead();
        
        if (_eventIt == _observerData->_current)
            return NodeEvent();
        else
            return (*_eventIt).event;
    }
    
    NodeAddress mapToCurrent(NodeAddress address, bool* wasDeleted = nullptr) const;
    NodeAddress mapFromCurrent(NodeAddress address, bool* wasAdded = nullptr) const;
    
    std::unique_ptr<QReadLocker> lockObserverForRead() const
    {
        if (_observerData)
            return _observerData->lockForRead();
        else
            return {};
    }
    
    std::unique_ptr<QWriteLocker> lockObserverForWrite() const
    {
        if (_observerData)
            return _observerData->lockForWrite();
        else
            return {};
    }
    
private:
    inline void ref(TreeObserverData::node_event_iterator);
    inline void deref(TreeObserverData::node_event_iterator);
    
    QSharedPointer<TreeObserverData> _observerData;
    TreeObserverData::node_event_iterator _eventIt;
};

inline void ObservedTreeState::ref(TreeObserverData::node_event_iterator i)
{
    if (Q_UNLIKELY(!_observerData)) return;

    auto refCounter = (*i).refCounter;
    
    assert(refCounter);
    refCounter->count.ref();
    refCounter->metaCount.ref();
}
        
inline void ObservedTreeState::deref(TreeObserverData::node_event_iterator i)
{
    if (Q_UNLIKELY(!_observerData)) return;
    
    auto refCounter = (*i).refCounter;
    assert(refCounter);
    
    if (!refCounter->count.deref())
    {
        const auto lock = _observerData->lockForRead();
        if (Q_LIKELY(
                i != _observerData->_current
                && !refCounter->deletedByObserver
            ))
        {
            _observerData->releaseNodeEvent(i);
        }
    }
    
    if (!refCounter->metaCount.deref())
    {
        delete refCounter;
    }
}

}

template<class Tree>
using DataTreeObserver = aux_datatree::TreeObserver<Tree>;

using DataTreeNodeEvent = aux_datatree::NodeEvent;
using DataTreeNodeEventBuilder = aux_datatree::NodeEventBuilder;

}

Q_DECLARE_METATYPE(Addle::DataTreeNodeEvent)
