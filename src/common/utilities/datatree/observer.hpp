#pragma once

#include <any>
#include <algorithm>
#include <exception>
#include <deque>
#include <optional>
#include <memory>
#include <map>
#include <vector>
#include <typeindex>

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

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
#include <QtDebug>      
#include "utilities/debugging/qdebug_extensions.hpp"
// QDebug operator<< for std::optional<NodeAddress>

#include "utilities/debugging/debugstring.hpp"
// QTEST_TOSTRING_IMPL_BY_QDEBUG for std::optional<NodeAddress>
#endif

#ifdef ADDLE_TEST
class DataTree_UTest;   // friend access
#endif

namespace Addle {
namespace aux_datatree {
    
class TreeObserverData;
class ObservedTreeState;

// TODO: NodeAddress and NodeChunk should be passed by value and not reference
// in many cases.

template<bool additive>
extern std::optional<NodeAddress> mapSingle_impl(
        const NodeAddress& from,
        const NodeChunk& rel);

template<bool additive>
extern std::pair<NodeChunk, NodeChunk> mapSingleChunk_impl(
        const NodeChunk& from,
        const NodeChunk& rel);

/**
 * Describes a change in the node structure of a data tree. Nodes may be added,
 * removed, or moved; and the event may describe multiple such operations
 * performed in sequence.
 */
class NodeEvent
{
    class Step;

    using step_list_t = QList<Step>;
    
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
                _data && other._data
                && _data->steps == other._data->steps
                && _data->totalChunkCount == other._data->totalChunkCount
            );
    }
    
    bool operator!=(const NodeEvent& other) const { return !(*this == other); }
    
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
            // Honestly it should just not be possible to have a 
            // data-initialized but invalid event.
    }
    
    // Given in terms of the event's end state
    QList<NodeChunk> chunksAdded() const;
    
    // Given in terms of the event's begin state
    QList<NodeChunk> chunksRemoved() const;
    
    QList<NodeAddress> affectedParents() const;
    QList<NodeAddress> affectedParentsFromEnd() const;
    
    bool isAffectedParent(NodeAddress parent) const;
    bool isAffectedParentFromEnd(NodeAddress parent) const;
    
    std::optional<NodeAddress> mapForward(NodeAddress from) const  
    {
        if (!_data || from.isRoot()) 
            return from;
        
        NodeAddress result(from);
        
        for (const Step& step : _data->steps)
        {
            auto next = step.mapForward(result);
            if (!next) return {};
            
            result = std::move(*next);
        }
        
        return result;
    }
    
    QList<NodeChunk> mapChunkForward(NodeChunk from) const
    {
        return mapChunksForward_impl({ from });
    }
    
    QList<NodeChunk> mapChunkForward(NodeAddress address, std::size_t length) const
    {
        return mapChunksForward_impl({{ address, length }});
    }
    
    QList<NodeChunk> mapChunksForward(QList<NodeChunk> from) const
    {
        return mapChunksForward_impl(std::move(from));
    }
    
    std::optional<NodeAddress> mapBackward(NodeAddress from) const
    {
        using namespace boost::adaptors;
        
        if (!_data || from.isRoot()) 
            return from;
        
        NodeAddress result(from);
        
        for (const Step& step : _data->steps | reversed)
        {
            auto next = step.mapBackward(result);
            if (!next) return {};
            
            result = std::move(*next);
        }
        
        return result;
    }
    
    QList<NodeChunk> mapChunkBackward(NodeChunk from) const
    {
        return mapChunksBackward_impl({ from });
    }
    
    QList<NodeChunk> mapChunkBackward(NodeAddress address, std::size_t length) const
    {
        return mapChunksBackward_impl({{ address, length }});
    }
    
    QList<NodeChunk> mapChunksBackward(QList<NodeChunk> from) const
    {
        return mapChunksBackward_impl(std::move(from));
    }
    
//     // TODO: The amount of information we track is probably insufficient to be
//     // useful and the amount that would be useful is probably impractical.
//     std::pair<std::size_t, bool> childCount(
//             NodeAddress parent, 
//             std::size_t progress = SIZE_MAX) const
//     {
//         if (!_data) return std::make_pair(0, false);
//         
//         bool wasAffected = false;
//         std::ptrdiff_t result = 0;
//         
//         for (const Step& step : _data->steps)
//         {
//             auto find = step.initialChildCounts().find(parent);
//             if (find != step.initialChildCounts().end())
//                 result = (*find).second;
//             
//             auto offset = step.childCountOffset(parent, progress);
//             
//             wasAffected |= (offset != 0);
//             
//             result += offset;
//             
//             auto nextParent = step.mapForward(parent);
//             
//             if (!nextParent)
//                 return std::make_pair(0, true);
//             
//             parent = std::move(*nextParent);
//             
//             if (progress > step.chunkCount())
//                 progress -= step.chunkCount();
//             else
//                 break;
//         }
//         
//         return std::make_pair(std::max(result, (std::ptrdiff_t)0), wasAffected);
//     }
    
    // Extends the event to indicate that `chunk` was added to the underlying
    // tree.
    void addChunk(NodeChunk chunk);
    
    void addChunk(NodeAddress address, std::size_t length)
    {
        addChunk(NodeChunk { address, length });
    }

    void addChunks(QList<NodeChunk> chunks);
    
    // Extends the event to indicate that `chunk` was removed from the 
    // underlying tree.
    void removeChunk(NodeChunk chunk);
    
    void removeChunk(NodeAddress address, std::size_t length)
    {
        removeChunk(NodeChunk { address, length });
    }
    
    void removeChunks(QList<NodeChunk> chunks);
    
    // void merge(NodeEvent event);
    
    // Prepends `rel` to the address of every chunk in this event. References
    // that were previously to the root become references to `rel`, references 
    // to all other nodes become references to analogous descendants of `rel`.
    void moveRoot(NodeAddress rel)
    {
        if (!_data) 
            return;
        else
            _data.detach();
        
        for (auto& step : _data->steps)
            step.moveRoot(rel);
    }
    
    NodeEvent movedRoot(NodeAddress rel) const &
    {
        NodeEvent result(*this);
        result.moveRoot(std::move(rel));
        return result;
    }
    
    NodeEvent movedRoot(NodeAddress rel) &&
    {
        moveRoot(rel);
        return std::move(*this);
    }
    
    // Truncates `rel` from the address of every chunk in this event. Chunks or 
    // addresses which are not descendants of `rel` are removed. Move operations
    // for which the source or destination are not descendants of `rel` are 
    // changed into Add or Remove operations respectively.
    // void truncateRoot(NodeAddress rel);
    //
    
    /**
     * Iterator and StagingHandle can be used to examine the full contents of 
     * the NodeEvent, intended for reproducing the event in another tree.
     */
    class StagingHandle
    {
    public:
        NodeOperation operation() const { return (*_step).operation(); }
        NodeChunk operatingChunk() const
        {
            switch(operation())
            {
                case Add:
                    return *_fChunk;
                    
                case Remove:
                    return *_rFChunk;
                
                default:
                    Q_UNREACHABLE();
            }
        }
        
        std::optional<NodeAddress> mapPastForward(NodeAddress from) const;
        std::optional<NodeAddress> mapCurrentBackward(NodeAddress from) const;
        
        std::optional<NodeAddress> mapCurrentForward(NodeAddress from) const;
        std::optional<NodeAddress> mapFutureBackward(NodeAddress from) const;
        
        // past chunks forward
        QList<NodeChunk> mapPastChunkForward(NodeChunk from) const
        {
            return mapPastChunksForward_impl({ from });
        }
        
        QList<NodeChunk> mapPastChunkForward(NodeAddress address, std::size_t length) const
        {
            return mapPastChunksForward_impl({{ address, length }});
        }
        
        QList<NodeChunk> mapPastChunksForward(QList<NodeChunk> from) const
        {
            return mapPastChunksForward_impl(std::move(from));
        }
        
        // current chunks backward
        QList<NodeChunk> mapCurrentChunkBackward(NodeChunk from) const
        {
            return mapCurrentChunksBackward_impl({ from });
        }
        
        QList<NodeChunk> mapCurrentChunkBackward(NodeAddress address, std::size_t length) const
        {
            return mapCurrentChunksBackward_impl({{ address, length }});
        }
        
        QList<NodeChunk> mapCurrentChunksBackward(QList<NodeChunk> from) const
        {
            return mapCurrentChunksBackward_impl(std::move(from));
        }
        
        // current chunks forward
        QList<NodeChunk> mapCurrentChunkForward(NodeChunk from) const
        {
            return mapCurrentChunksForward_impl({ from });
        }
        
        QList<NodeChunk> mapCurrentChunkForward(NodeAddress address, std::size_t length) const
        {
            return mapCurrentChunksForward_impl({{ address, length }});
        }
        
        QList<NodeChunk> mapCurrentChunksForward(QList<NodeChunk> from) const
        {
            return mapCurrentChunksForward_impl(std::move(from));
        }
        
        // future chunks backward
        QList<NodeChunk> mapFutureChunkBackward(NodeChunk from) const
        {
            return mapFutureChunksBackward_impl({ from });
        }
        
        QList<NodeChunk> mapFutureChunkBackward(NodeAddress address, std::size_t length) const
        {
            return mapFutureChunksBackward_impl({{ address, length }});
        }
        
        QList<NodeChunk> mapFutureChunksBackward(QList<NodeChunk> from) const
        {
            return mapFutureChunksBackward_impl(std::move(from));
        }
        
    protected:
        using step_iterator = step_list_t::const_iterator;
        
        using coarse_chunk_iterator = chunk_map_t::const_iterator;
        using fine_chunk_iterator = chunk_list_t::const_iterator;
        
        using r_coarse_chunk_iterator = std::reverse_iterator<coarse_chunk_iterator>;
        using r_fine_chunk_iterator = std::reverse_iterator<fine_chunk_iterator>;
        
        StagingHandle() = default;
        StagingHandle(const StagingHandle&) = default;
        StagingHandle& operator=(const StagingHandle&) = default;
        
        step_iterator _stepBegin    = {};
        step_iterator _step         = {};
        step_iterator _stepEnd      = {};
        
        std::size_t _chunkOrd       = 0;
                
        coarse_chunk_iterator   _cChunk     = {};
        fine_chunk_iterator     _fChunk     = {};
        r_coarse_chunk_iterator _rCChunk    = {};
        r_fine_chunk_iterator   _rFChunk    = {};
        
    private:
        QList<NodeChunk> mapPastChunksForward_impl(QList<NodeChunk> from) const;
        QList<NodeChunk> mapCurrentChunksBackward_impl(QList<NodeChunk> from) const;
        QList<NodeChunk> mapCurrentChunksForward_impl(QList<NodeChunk> from) const;
        QList<NodeChunk> mapFutureChunksBackward_impl(QList<NodeChunk> from) const;
        
#ifdef ADDLE_TEST
        friend class ::DataTree_UTest;
#endif
    };
    
    class Iterator : public boost::iterator_facade<
            Iterator,
            const StagingHandle,
            std::bidirectional_iterator_tag
        >, 
        private StagingHandle
    {   
    public:
        Iterator() = default;
        Iterator(const Iterator&) = default;
        Iterator& operator=(const Iterator&) = default;
        
        void swap(Iterator& other)
        {
            std::swap(_chunkOrd, other._chunkOrd);
            
            std::swap(_step,    other._step);
            std::swap(_stepEnd, other._stepEnd);
            
            std::swap(_cChunk,  other._cChunk);
            std::swap(_fChunk,  other._fChunk);
            std::swap(_rCChunk, other._rCChunk);
            std::swap(_rFChunk, other._rFChunk);
        }
        
    private:
        Iterator(
                step_iterator stepBegin,
                step_iterator step,
                step_iterator stepEnd)
        {
            _stepBegin  = stepBegin;
            _step       = step;
            _stepEnd    = stepEnd;
            
            if (_step != _stepEnd)
                initStepFront();
        }
                
        const StagingHandle& dereference() const
        {
            return static_cast<const StagingHandle&>(*this);
        }
        
        bool equal(const Iterator& other) const
        { 
            return _step == other._step && _chunkOrd == other._chunkOrd;
        }
        
        inline void initStepFront();
        inline void initStepBack();
        
        inline void increment();
        inline void decrement();
        
        friend class NodeEvent;
        friend class boost::iterator_core_access;
        
#ifdef ADDLE_TEST
        friend class ::DataTree_UTest;
#endif
    };
    
    Iterator begin() const
    {
        if (!_data) return Iterator();
        return Iterator(
                _data->steps.cbegin(), 
                _data->steps.cbegin(), 
                _data->steps.cend()
            );
    }
    
    Iterator end() const 
    {
        if (!_data) return Iterator();
        return Iterator(
                _data->steps.cbegin(), 
                _data->steps.cend(), 
                _data->steps.cend()
            );
    }
    
private:
    class Step
    {
    public:
        explicit Step(NodeOperation operation)
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
        
        std::optional<NodeAddress> mapForward(
                const NodeAddress& from, 
                std::size_t minChunkOrd = 0,
                std::size_t maxChunkOrd = SIZE_MAX) const
        {
            if (from.isRoot()) return from;

            switch(_operation)
            {
                case NodeOperation::Add:
                {
                    PrimaryChunkMapper<true, true> mapper(*this, minChunkOrd, maxChunkOrd);
                    return mapper.mapAddress(from);
                }
                case NodeOperation::Remove:
                {
                    PrimaryChunkMapper<false, false> mapper(*this, minChunkOrd, maxChunkOrd);
                    return mapper.mapAddress(from);
                }
                
        //                 case NodeOperation::Move: // TODO
                    
                default:
                    Q_UNREACHABLE();
            }
        }
        
        template<typename InRange, class OutContainer>
        void mapChunksForward(
                const InRange& in, 
                OutContainer& out,
                std::size_t minChunkOrd = 0, 
                std::size_t maxChunkOrd = SIZE_MAX) const;
        
        std::optional<NodeAddress> mapBackward(
                const NodeAddress& from,
                std::size_t minChunkOrd = 0, 
                std::size_t maxChunkOrd = SIZE_MAX) const
        {
            if (_primaryChunks.empty())
                return from;
            
            switch(_operation)
            {
                case NodeOperation::Add:
                {
                    PrimaryChunkMapper<false, true> mapper(*this, minChunkOrd, maxChunkOrd);
                    return mapper.mapAddress(from);
                }
                case NodeOperation::Remove:
                {
                    PrimaryChunkMapper<true, false> mapper(*this, minChunkOrd, maxChunkOrd);
                    return mapper.mapAddress(from);
                }
                
        //                 case NodeOperation::Move: // TODO
                    
                default:
                    Q_UNREACHABLE();
            }
        }
        
        template<typename InRange, class OutContainer>
        void mapChunksBackward(
                const InRange& in, 
                OutContainer& out,
                std::size_t minChunkOrd = 0, 
                std::size_t maxChunkOrd = SIZE_MAX) const;
        
        // A convenience to avoid having to make a container to hold the result
        // of a subtractive mapping (i.e., when it can be assumed there is no
        // more than one chunk). Maps forward on Remove steps and backward on 
        // Add steps.
        std::optional<NodeChunk> mapChunkSubtractive(
                NodeChunk from,
                std::size_t minChunkOrd = 0, 
                std::size_t maxChunkOrd = SIZE_MAX) const;
                
//         std::ptrdiff_t childCountOffset(
//                 NodeAddress parent, 
//                 std::size_t progress = SIZE_MAX) const;
        
        // Changes the primary chunks of this Add step as if `rel` had been 
        // inserted before them. (Has no effect and returns quickly if `rel` is
        // above all chunks in the step). Returns the change in the number of 
        // chunks in the step.
        signed rebase(const NodeChunk& rel);
        
        // Adds a chunk to this step, eliding or merging it with existing chunks
        // as necessary. Returns the change in the number of chunks in the step.
        signed addPrimaryChunk(NodeChunk&& chunk);
        
        void moveRoot(NodeAddress rel);
        
    private:
        template<bool Additive, bool ChunkIndciesAscending = true>
        struct PrimaryChunkMapper
        {
            PrimaryChunkMapper(const Step& step_,
                    std::size_t minChunkOrd_ = 0,
                    std::size_t maxChunkOrd_ = SIZE_MAX)
                : step(step_), 
                minChunkOrd(minChunkOrd_),
                maxChunkOrd(std::min(maxChunkOrd_, step._chunkCount))
            {
                assert(minChunkOrd <= maxChunkOrd);
            }
            
            PrimaryChunkMapper(const Step& step_, 
                    NodeChunk from_, 
                    std::size_t minProgress_ = 0,
                    std::size_t maxProgress_ = SIZE_MAX);
            
            std::optional<NodeAddress> mapAddress(NodeAddress from);
                
            std::optional<NodeChunk> yieldMapChunk();
            
            const Step& step;
            const std::size_t minChunkOrd;
            const std::size_t maxChunkOrd;
            
            std::optional<NodeChunk> remainderChunk;
            
            std::size_t chunkOrd = 0;
            
            std::optional<NodeAddress> mappedParentAddress;
            
            chunk_map_t::const_iterator cChunk = {};
            
//             using f_chunk_iter_t = boost::mp11::mp_if_c<
//                     ProgressReversed,
//                     std::reverse_iterator<chunk_list_t::const_iterator>,
//                     chunk_list_t::const_iterator
//                 >;
            
            chunk_list_t::const_iterator fChunk     = {};
            chunk_list_t::const_iterator fChunkEnd  = {};
        };
                    
        NodeOperation _operation;
        
        std::size_t _chunkCount = 0;
        
        chunk_map_t _primaryChunks;
        chunk_list_t _secondaryChunks;
        
        initial_child_counts_t _initialChildCounts;
        
        NodeChunk _moveSource;
        NodeAddress _moveDest;
        
#ifdef ADDLE_TEST
        friend class ::DataTree_UTest;
#endif
    };
    
    struct Data : QSharedData
    {
        step_list_t steps;
        std::size_t totalChunkCount = 0;
    };
    
    void initData()
    {
        if (!_data)
            _data = new Data;
        else
            _data.detach();
    }
    
    void removeChunks_impl(NodeChunk* begin, NodeChunk* end);
        
    QList<NodeChunk> mapChunksForward_impl(QList<NodeChunk> from) const
    {
        if (!_data || from.isEmpty())
            return from;
        
//         if (from.size() == 1 && (!from.first().length || from.first().length == 1))
//         {
//             auto mapped = mapForward(from.first().address);
//             if (mapped)
//                 return {{ *mapped, from.first().length }};
//             else
//                 return {};
//         }
        
        QList<NodeChunk> result;
        
        auto i  = _data->steps.cbegin();
        auto end = _data->steps.cend();
        
        if (i == end) return {}; // impossible?
        
        while (true)
        {
            const Step& step = *i;
            
            step.mapChunksForward(from, result);
            if (result.isEmpty()) return {};
            
            ++i;
            if (i == end) break;
            
            result.swap(from);
            result.clear();
        }
        
        cleanupChunkSet(result);
        
        return result;
    }
    
    QList<NodeChunk> mapChunksBackward_impl(QList<NodeChunk> from) const
    {
        if (!_data || from.isEmpty())
            return from;
        
//         if (from.size() == 1 && (!from.first().length || from.first().length == 1))
//         {
//             auto mapped = mapBackward(from.first().address);
//             if (mapped)
//                 return {{ *mapped, from.first().length }};
//             else
//                 return {};
//         }
        
        QList<NodeChunk> result;
        
        auto i  = _data->steps.crbegin();
        auto end = _data->steps.crend();
        
        if (i == end) return {}; // impossible?
        
        while (true)
        {
            const Step& step = *i;
            
            step.mapChunksBackward(from, result);
            if (result.isEmpty()) return {};
            
            ++i;
            if (i == end) break;
            
            result.swap(from);
            result.clear();
        }
        
        cleanupChunkSet(result);
        
        return result;
    }
    
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
    template<class> friend class EnactNodeEventHelper;
    friend class TreeObserverData;
    friend class ObservedTreeState;
    friend class NodeEventBuilder;
    
#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
};

inline void NodeEvent::Iterator::initStepFront()
{
    switch(operation())
    {
        case Add:
            _cChunk = (*_step).primaryChunks().begin();
            _fChunk = (*_cChunk).second.begin();
            break;
            
        case Remove:
            _rCChunk = std::make_reverse_iterator((*_step).primaryChunks().end());
            _rFChunk = std::make_reverse_iterator((*_rCChunk).second.end());
            break;
            
        default:
            Q_UNREACHABLE();
    }
}

inline void NodeEvent::Iterator::initStepBack()
{
    _chunkOrd = (*_step).chunkCount();
    switch(operation())
    {
        case Add:
            _cChunk = --((*_step).primaryChunks().end());
            _fChunk = --((*_cChunk).second.end());
            break;
            
        case Remove:
            _rCChunk = --std::make_reverse_iterator((*_step).primaryChunks().begin());
            _rFChunk = --std::make_reverse_iterator((*_rCChunk).second.begin());
            break;
            
        default:
            Q_UNREACHABLE();
    }
}
        
inline void NodeEvent::Iterator::increment()
{
    assert (_step != _stepEnd);
    
    ++_chunkOrd;
    switch(operation())
    {
        case Add:
            ++_fChunk;
            if (_fChunk == (*_cChunk).second.end())
            {
                ++_cChunk;
                if (_cChunk == (*_step).primaryChunks().end())
                {
                    ++_step;
                    if (_step != _stepEnd) 
                        initStepFront();
                    
                    _chunkOrd = 0;
                    return;
                }
                else
                {
                    _fChunk = (*_cChunk).second.begin();
                }
            }
            return;
        
        case Remove:
            ++_rFChunk;
            if (_rFChunk.base() == (*_rCChunk).second.begin())
            {
                ++_rCChunk;
                if (_rCChunk.base() == (*_step).primaryChunks().begin()) 
                {
                    ++_step;
                    if (_step != _stepEnd) 
                        initStepFront();
                    
                    _chunkOrd = 0;
                    return;
                }
                else
                {
                    _rFChunk = std::make_reverse_iterator((*_rCChunk).second.end());
                }
            }
            return;
            
        default:
            Q_UNREACHABLE();
    }
}

inline void NodeEvent::Iterator::decrement()
{
    --_chunkOrd;
    if (_step == _stepEnd) 
    {
        --_step;
        initStepBack();
        return;
    }
    
    switch(operation())
    {
        case Add:
            if (_fChunk == (*_cChunk).second.begin())
            {
                if (_cChunk == (*_step).primaryChunks().begin()) 
                {
                    --_step;
                    initStepBack();
                    
                    return;
                }
                else
                {
                    --_cChunk;
                    _fChunk = --((*_cChunk).second.end());
                }
            }
            else
            {
                --_fChunk;
            }
            return;
        
        case Remove:
            if (_rFChunk.base() == (*_rCChunk).second.end())
            {
                if (_rCChunk.base() == (*_step).primaryChunks().end()) 
                {
                    --_step;
                    initStepBack();
                    
                    return;
                }
                else
                {
                    --_rCChunk;
                    _rFChunk = --std::make_reverse_iterator((*_rCChunk).second.begin());
                }
            }
            {
                --_rFChunk;
            }
            return;
            
        default:
            Q_UNREACHABLE();
    }
}

extern template void NodeEvent::Step::mapChunksForward(const QList<NodeChunk>&, QList<NodeChunk>&, std::size_t, std::size_t) const;
extern template void NodeEvent::Step::mapChunksBackward(const QList<NodeChunk>&, QList<NodeChunk>&, std::size_t, std::size_t) const;

// TODO: rename
// Modifies a tree and records those modifications in an event.
template<class Tree>
class EnactNodeEventHelper
{
public:
    using handle_t = aux_datatree::node_handle_t<Tree>;
    using child_handle_t = aux_datatree::child_node_handle_t<handle_t>;
    
    EnactNodeEventHelper(handle_t root, NodeEvent& event)
        : _root(root),
        _event(event)
    {
        assert(_root);
    }
    
    EnactNodeEventHelper(const EnactNodeEventHelper&) = delete;
    EnactNodeEventHelper(EnactNodeEventHelper&&) = delete;
    EnactNodeEventHelper& operator=(const EnactNodeEventHelper&) = delete;
    EnactNodeEventHelper& operator=(EnactNodeEventHelper&&) = delete;
    
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
auto EnactNodeEventHelper<Tree>::insertNodes(
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
    _event.addChunk( 
            NodeAddressBuilder(parentAddress) << startIndex,
            valuesCount 
        );  
    
    return result;
}

template<class Tree>
void EnactNodeEventHelper<Tree>::removeNodes_impl(
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
    _event.removeChunk(
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

    void startRecording()
    {
        const QWriteLocker locker(&_lock);
        
        assert(!_isRecording);
        
        _isRecording = true;
        _recording = NodeEvent();
    }
    
    NodeEvent finishRecording()
    {
        const QWriteLocker locker(&_lock);
        
        assert(_isRecording);
        
        _isRecording = false;
        
        pushEvent(_recording);
        return std::exchange(_recording, NodeEvent());
    }
    
    void pushEvent(NodeEvent event);
    
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
    inline TreeObserverData();
    void releaseNodeEvent(node_event_iterator id);
    void removeNodeRefData(const GenericNodeRefData* data);
    
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

inline TreeObserverData::TreeObserverData()
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
    
    TreeObserver() = default;
    
    TreeObserver(Tree& tree)
        : _tree(std::addressof(tree)),
        _root(::Addle::aux_datatree::tree_root(tree)),
        _data(new TreeObserverData)
    {
    }
    
    TreeObserver(const TreeObserver&) = delete;
    TreeObserver(TreeObserver&&) = delete;
    TreeObserver& operator=(const TreeObserver&) = delete;
    TreeObserver& operator=(TreeObserver&&) = delete;
    
    ~TreeObserver()
    {
        if (_data) _data->onTreeDeleted(); 
    }
    
    void setTree(Tree& tree)
    {
        _data = QSharedPointer<TreeObserverData>(new TreeObserverData);
        
        _tree = std::addressof(tree);
        _root = ::Addle::aux_datatree::tree_root(tree);
    }
    
    const Tree* tree() const { return _tree; }
    Tree* tree() { return _tree; }
    
    void startRecording()
    {
        assert(_data);
        _data->startRecording();
    }
    
    NodeEvent finishRecording()
    {
        assert(_data);
        return _data->finishRecording();
    }
    
    NodeEvent pendingEvent() const
    {
        if (_data)
            return _data->_recording; 
        else
            return NodeEvent();
    }
    
    bool isRecording() const { return _data && _data->_isRecording; }
    
    template<typename Range>
    auto insertNodes(
            handle_t parent, 
            child_handle_t before, 
            Range&& nodeValues
         )
    {
        assert(_data && _data->_isRecording && _root);
        
        return EnactNodeEventHelper<Tree>(_root, _data->_recording)
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
        assert(_data && _data->_isRecording);
        _data->_recording.addChunk(
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
        assert(_data && _data->_isRecording && _root);
        EnactNodeEventHelper<Tree>(_root, _data->_recording)
            .removeNodes(
                parent, 
                startIndex, 
                count
            );
    }
    
    template<typename Range>
    void removeNodes(Range&& range)
    {
        assert(_data && _data->_isRecording && _root);
        EnactNodeEventHelper<Tree>(_root, _data->_recording)
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
        assert(_data && _data->_isRecording);
        _data->_recording.removeChunk(
                NodeAddressBuilder(::Addle::aux_datatree::node_address(parent)) << startIndex,
                count
            );
    }
    
    void passivePushEvent(NodeEvent event)
    {
        assert(_data && !_data->_isRecording);
        _data->pushEvent(std::move(event));
    }
    
    std::unique_ptr<QReadLocker> lockForRead() const
    { 
        if (_data)
            return _data->lockForRead(); 
        else
            return nullptr;
    }
    
    std::unique_ptr<QWriteLocker> lockForWrite() const 
    { 
        if (_data)
            return _data->lockForWrite(); 
        else
            return nullptr;
    }
    
private:
    Tree* _tree = nullptr;
    handle_t _root = {};
    
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
    
    std::optional<NodeAddress> mapToCurrent(NodeAddress address) const;
    std::optional<NodeAddress> mapFromCurrent(NodeAddress address) const;
    
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
    
protected:
    QSharedPointer<TreeObserverData> _treeObserverData;
    NodeAddress _address;
    bool _isDeleted = false;
    const std::type_info* _type;
    
    QAtomicInteger<unsigned> _refCount;
    
    friend class TreeObserverData;
    template<class, bool> friend class NodeRef;
};

template<typename Handle>
class GenericNodeRefData_withHandle : public GenericNodeRefData
{
public:
    virtual ~GenericNodeRefData_withHandle() noexcept = default;
    
private:
    Handle _handle;
    
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
    using data_t = GenericNodeRefData_withHandle<node_handle_t<Tree>>;
    
    std::size_t hash() const
    {
        return std::hash<data_t*>() (_data);
    }
    
    friend uint qHash(const NodeRef& ref, uint seed = 0)
    {
        return ref.hash() ^ seed;
    }
    
    data_t* _data;
    friend struct std::hash<NodeRef<Tree, IsConst>>;
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
        if ((*(*i).second->_type) != typeid(handle_t)) continue;
        
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
    
} // namespace aux_datatree

template<class Tree>
using DataTreeObserver = aux_datatree::TreeObserver<Tree>;

using DataTreeNodeEvent = aux_datatree::NodeEvent;
//using DataTreeNodeEventBuilder = aux_datatree::NodeEventBuilder;

}

Q_DECLARE_METATYPE(Addle::DataTreeNodeEvent)

namespace std {
template<typename Tree, bool IsConst>
struct hash<Addle::aux_datatree::NodeRef<Tree, IsConst>>
{
    std::size_t operator()(const Addle::aux_datatree::NodeRef<Tree, IsConst>& ref) const
    { 
        return ref.hash(); 
    }
};

}

#ifdef ADDLE_TEST
QTEST_TOSTRING_IMPL_BY_QDEBUG((::std::optional<::Addle::aux_datatree::NodeAddress>))
#endif // ADDLE_TEST
