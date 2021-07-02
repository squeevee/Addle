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
    
    NodeEvent(NodeEvent&& other)
    {
        _data.swap(other._data);
    }
    
    NodeEvent& operator=(NodeEvent&& other)
    {
        _data.swap(other._data);
        other._data = {};
        
        return *this;
    }
    
    void swap(NodeEvent& other)
    {
        _data.swap(other._data);
    }
    
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

    explicit operator bool() const { return !isNull(); }
    bool operator!() const { return isNull(); }
    bool isNull() const
    { 
        return !_data 
            || Q_UNLIKELY(_data->steps.isEmpty())
            || Q_UNLIKELY(std::all_of(
                    _data->steps.cbegin(),
                    _data->steps.cend(),
                    [] (const Step& step) -> bool {
                        switch(step.operation())
                        {
                            case Add:
                            case Remove:
                                return step.primaryChunks().empty();
                            
                            default:
                                Q_UNREACHABLE();
                        }
                    }
                ));
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

class GenericNodeRefData;
    
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
    TreeObserverData(const TreeObserverData&) = delete;
    TreeObserverData(TreeObserverData&&) = delete;
    
    inline ~TreeObserverData();

    void startRecording();
    
    NodeEvent finishRecording();
    
    void onTreeDeleted()
    {
        const QWriteLocker locker(&_lock);
        
        assert(!_isDeleted);
        _isDeleted = true;
    }
    
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
    void removeNodeRefData(const GenericNodeRefData* data);
    
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
    
    // private mutex guarding access to the node ref map
    mutable QMutex _nodeRefMutex;

    std::multimap<NodeAddress, GenericNodeRefData*> _nodeRefs;
    
    template<typename> friend class TreeObserver;
    friend class ObservedTreeState;
    template<class, bool> friend class NodeRef;
    
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
    
    bool isRecording() const { return _data->_isRecording; }
    
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
    
    void passiveAddNodes(
            handle_t parent,
            std::size_t startIndex,
            std::size_t count = 1
         )
    {
        assert(_data->_isRecording);
        _data->_recording.addChunk_impl(
                NodeAddressBuilder(::Addle::aux_datatree::node_address(parent)) << startIndex,
                count
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
    
    void passiveRemoveNodes(
            handle_t parent,
            std::size_t startIndex,
            std::size_t count = 1
         )
    {
        assert(_data->_isRecording);
        _data->_recording.removeChunk_impl(
                NodeAddressBuilder(::Addle::aux_datatree::node_address(parent)) << startIndex,
                count
            );
    }
    
    std::unique_ptr<QReadLocker> lockForRead() const { return _data->lockForRead(); }
    std::unique_ptr<QWriteLocker> lockForWrite() const { return _data->lockForWrite(); }
    
private:
    ::Addle::aux_datatree::node_handle_t<Tree> _root = {};
    QSharedPointer<TreeObserverData> _data;
    
    friend class ObservedTreeState;
    template<class, bool> friend class NodeRef;
#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
};

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
    
    ~ObservedTreeState()
    { 
        if (_observerData)
        {
            deref(_eventIt); 
        }
    }
    
    ObservedTreeState& operator=(const ObservedTreeState& other)
    {
        if (_observerData)
        {
            deref(_eventIt);
        }
        
        _observerData = other._observerData;
        _eventIt = other._eventIt;
        
        ref(_eventIt);
        
        return *this;
    }
    
    ObservedTreeState& operator=(ObservedTreeState&& other)
    {
        if (_observerData)
        {
            deref(_eventIt);
        }
        
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

// Data object for the generic implementation of NodeRef (which is coordinated
// directly with DataTreeObserver). Other implementations may use their own data
// object types.
class GenericNodeRefData
{
public:
    virtual ~GenericNodeRefData() noexcept = default;
    
    virtual const std::type_info& type() const noexcept = 0;
    
    virtual NodeAddress address() const noexcept = 0;
    virtual void setAddress(NodeAddress address) noexcept = 0;
    
    virtual bool isDeleted() const noexcept = 0;
    virtual void markDeleted() noexcept = 0;
};

template<typename Handle>
class GenericNodeRefData_impl : public GenericNodeRefData
{
public:
    virtual ~GenericNodeRefData_impl() noexcept = default;
    
    const std::type_info& type() const noexcept override { return *_type; }
    
    NodeAddress address() const noexcept override { return _address; }
    void setAddress(NodeAddress address) noexcept override { _address = std::move(address); }
    
    bool isDeleted() const noexcept override { return _isDeleted; }
    void markDeleted() noexcept override { _isDeleted = true; }
    
private:
    QSharedPointer<TreeObserverData> _treeObserverData;
    
    NodeAddress _address;
    bool _isDeleted = false;
    
    QAtomicInteger<unsigned> _refCount;
    
    Handle _handle;
    const std::type_info* _type;
    
    template<class, bool> friend class NodeRef;
};

template<class Tree, bool IsConst>
class NodeRef
{
public:
    using handle_t = node_handle_t<Tree>;
    using observer_t = boost::mp11::mp_if_c<
            IsConst,
            const TreeObserver<Tree>,
            TreeObserver<Tree>
        >;
    
    NodeRef() : _data(nullptr) {}
    
    NodeRef(observer_t& tree, handle_t node);
    
    NodeRef(const NodeRef& other)
        : _data(other._data)
    {
        if (_data)
            _data->_refCount.ref();
    }
    
    NodeRef(NodeRef&& other)
        : _data(other._data)
    {
        other._data = nullptr;
    }
    
    ~NodeRef()
    {
        if (_data && !_data->_refCount.deref())
        {
            _data->_treeObserverData->removeNodeRefData(_data);
            delete _data;
        }
    }
    
    NodeRef& operator=(const NodeRef& other)
    {
        if (_data && !_data->_refCount.deref())
        {
            _data->_treeObserverData->removeNodeRefData(_data);
            delete _data;
        }
        
        if ((_data = other._data))
            _data->_refCount.ref();
        
        return *this;
    }
        
    NodeRef& operator=(NodeRef&& other)
    {
        if (_data && !_data->_refCount.deref())
        {
            _data->_treeObserverData->removeNodeRefData(_data);
            delete _data;
        }
        
        _data = other._data;
        other._data = nullptr;
        
        return *this;
    }
    
    bool operator==(const NodeRef& other) const { return _data == other._data; }
    bool operator!=(const NodeRef& other) const { return _data != other._data; }
    
    friend bool operator==(const handle_t& handle, const NodeRef& ref) { return handle == ref.get(); }
    friend bool operator!=(const handle_t& handle, const NodeRef& ref) { return handle != ref.get(); }
    friend bool operator==(const NodeRef& ref, const handle_t& handle) { return ref.get() == handle; }
    friend bool operator!=(const NodeRef& ref, const handle_t& handle) { return ref.get() != handle; }
    
    explicit operator bool () const { return isValid(); }
    bool operator! () const { return !isValid(); }
    
//     template<typename ConstHandle, 
//         std::enable_if_t<std::is_same_v<ConstHandle, const_node_handle_t<Tree>>
//             && !std::is_same_v<ConstHandle, handle_t>, void*> = nullptr>
//     friend bool operator==(const ConstHandle& handle, const NodeRef& ref) { return handle == ref.get(); }
//     
//     template<typename ConstHandle, 
//         std::enable_if_t<std::is_same_v<ConstHandle, const_node_handle_t<Tree>>
//             && !std::is_same_v<ConstHandle, handle_t>, void*> = nullptr>
//     friend bool operator==(const ConstHandle& handle, const NodeRef& ref) { return handle != ref.get(); }
//     
//     template<typename ConstHandle, 
//         std::enable_if_t<std::is_same_v<ConstHandle, const_node_handle_t<Tree>>
//             && !std::is_same_v<ConstHandle, handle_t>, void*> = nullptr>
//     friend bool operator==(const NodeRef& ref, const ConstHandle& handle) { return ref.get() == handle; }
//             
//     template<typename ConstHandle, 
//         std::enable_if_t<std::is_same_v<ConstHandle, const_node_handle_t<Tree>>
//             && !std::is_same_v<ConstHandle, handle_t>, void*> = nullptr>
//     friend bool operator==(const NodeRef& ref, const ConstHandle& handle) { return ref.get() != handle; }
    
    std::unique_ptr<QReadLocker> lockTreeObserverForRead() const
    { 
        if (_data)
            return _data->_treeObserverData->lockForRead();
        else
            return {};
    }
    
    std::unique_ptr<QWriteLocker> lockTreeObserverForWrite() const
    { 
        if (_data)
            return _data->_treeObserverData->lockForWrite();
        else
            return {};
    }
    
    bool isValid() const
    {
        if (!_data) return false;
        
        const QMutexLocker lock(&_data->_treeObserverData->_nodeRefMutex);
        return !(_data->_isDeleted);
    }
    
    handle_t get() const
    {
        if (!_data) return {};
        
        const QMutexLocker lock(&_data->_treeObserverData->_nodeRefMutex);
        return _data->_handle;
    }
    
    const_node_handle_t<Tree> getConst() const 
    { 
        return static_cast<const_node_handle_t<Tree>>(get()); 
    }
    
    decltype(auto) operator*() const
    {
        assert(isValid());
        
        handle_t node = get();
        assert(node);
        return *node;
    }
    
private:
    using data_t = GenericNodeRefData_impl<node_handle_t<Tree>>;
    data_t* _data;
};

template<class Tree, bool IsConst>
NodeRef<Tree, IsConst>::NodeRef(observer_t& tree, handle_t node)
{
    if (!node)
    {
        _data = nullptr;
        return;
    }
    
    NodeAddress address = aux_datatree::node_address(node);
    
    const QMutexLocker lock(&tree._data->_nodeRefMutex);
    
    auto i = tree._data->_nodeRefs.lower_bound(address);
    auto end = tree._data->_nodeRefs.upper_bound(address);
    
    for (; i != end; ++i)
    {
        if ((*i).second->type() != typeid(handle_t)) continue;
        
#ifdef ADDLE_DEBUG
        assert(static_cast<data_t*>((*i).second)->_handle == node);
#endif
        _data = static_cast<data_t*>((*i).second);
        _data->_refCount.ref();
        return;
    }
    
    _data = new data_t;
    _data->_address = address;
    _data->_handle = node;
    _data->_type = &typeid(handle_t);
    _data->_treeObserverData = tree._data;
    _data->_refCount.ref();
    
    tree._data->_nodeRefs.insert(std::make_pair(address, _data));
}
    
}

template<class Tree>
using DataTreeObserver = aux_datatree::TreeObserver<Tree>;

using DataTreeNodeEvent = aux_datatree::NodeEvent;
//using DataTreeNodeEventBuilder = aux_datatree::NodeEventBuilder;

}

Q_DECLARE_METATYPE(Addle::DataTreeNodeEvent)
