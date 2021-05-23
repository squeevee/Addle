#pragma once

#include <algorithm>
#include <any>
#include <deque>
#include <optional>
#include <memory>

#include <stdint.h> // SIZE_MAX

#include <QSharedData>
#include <QReadWriteLock>

#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include "./aux.hpp"

namespace Addle {
namespace aux_datatree2 {
    
using namespace aux_datatree;

// TODO: move to aux.hpp, maybe share code with NodeAddress
struct NodeChunk
{
    NodeAddress address;
    std::size_t length;
    
    inline bool coversAddress(const NodeAddress& otherAddress) const
    {
        if (address.isRoot())
            return true;
        
        if (address.size() > otherAddress.size())
            return false;
        
        auto i = address.begin();
        const auto end = address.end();
        
        auto j = otherAddress.begin();
        
        while (true)
        {
            auto index = *i;
            auto otherIndex = *j;
            
            ++i;
            ++j;
            
            if (i == end)
            {
                return otherIndex >= index && otherIndex < index + length;
            }
            else
            {
                if (index != otherIndex)
                    return false;
            }
        }
    }
    
    friend bool operator<(const NodeChunk& lhs, const NodeChunk& rhs)
    {
        return lhs.address < rhs.address; 
    }
    
    friend bool operator<(const NodeAddress& lhs, const NodeChunk& rhs)
    {
        return lhs < rhs.address; 
    }
    
    friend bool operator<(const NodeChunk& lhs, const NodeAddress& rhs)
    {
        return lhs.address < rhs; 
    }
    
#ifdef ADDLE_DEBUG
    friend QDebug operator<<(QDebug debug, const NodeChunk& chunk)
    {
        return debug << chunk.address
            << chunk.length;
    }
#endif
};

template<typename Tree>
class TreeObserver;

class TreeObserverData;
class NostalgicNodeEventHelper;

/**
 * Describes a change in the node structure of a data tree. Nodes may be added,
 * removed, or moved; and the event may describe multiple such operations
 * performed in sequence.
 * 
 * Build a NodeEvent using NodeEventBuilder.
 */
class NodeEvent
{
    class Step;

    using chunk_list_t = QVector<NodeChunk>;
    // sorted vectors of sibling chunks, arranged arranged by parent address
    using chunk_map_t = QMap<NodeAddress, chunk_list_t>;
    
public:    
    enum NodeOperation 
    {
        Add,
        Remove,
        Move
    };
    
    NodeEvent() = default;
    
    template<typename NodeHandle>
    NodeEvent(NodeHandle root)
        : _data(new Data (root))
    {
    }
    
    NodeEvent(const NodeEvent&) = default;
    NodeEvent(NodeEvent&&) = default;
    
    NodeEvent& operator=(const NodeEvent&) = default;
    NodeEvent& operator=(NodeEvent&&) = default;
    
    inline NodeAddress mapForward(NodeAddress from) const
    {
        return mapForward_impl(std::move(from), SIZE_MAX);
    }
    
    inline NodeAddress mapBackward(NodeAddress from) const
    {
        return mapBackward_impl(std::move(from), SIZE_MAX);
    }
    
    std::any root() const { return _data ? _data->root : std::any(); }
    
private:
    class Step
    {
    public:
        Step(NodeOperation operation)
            : _operation(operation)
        {
        }
        
        inline NodeOperation operation() const { return _operation; }
        
        inline std::size_t chunkCount() const { return _chunkCount; }
        
        const chunk_map_t& primaryChunks() const { return _primaryChunks; }
        const chunk_map_t& secondaryChunks() const { return _secondaryChunks; }
        
        inline NodeAddress mapForward(const NodeAddress& from, std::size_t progress = SIZE_MAX) const
        {
            if (from.isRoot()) return NodeAddress();
            
            switch(_operation)
            {
                case NodeOperation::Add:
                    return map_impl<true>(from, progress);
                case NodeOperation::Remove:
                    return map_impl<false, true>(from, progress);
                
//                 case StepType::Move: // TODO
                    
                default:
                    Q_UNREACHABLE();
            }
        }
        
        inline NodeAddress mapBackward(const NodeAddress& from, std::size_t progress = SIZE_MAX) const
        {
            if (_primaryChunks.isEmpty())
                return from;
            
            switch(_operation)
            {
                case NodeOperation::Add:
                    return map_impl<false>(from, progress);
                case NodeOperation::Remove:
                    return map_impl<true, true>(from, progress);
                
//                 case StepType::Move: // TODO
                    
                default:
                    Q_UNREACHABLE();
            }
        }
        
        // Adds a chunk to this step, eliding or merging it with existing chunks
        // as necessary.
        inline void addPrimaryChunk(NodeChunk&& chunk);
        
    private:
                
        template<bool additive, bool progressReversed = false>
        inline NodeAddress map_impl(
                const NodeAddress& from, 
                std::size_t progress = SIZE_MAX, 
                bool* wasDeleted = nullptr
            ) const;
    
        NodeOperation _operation;
        
        std::size_t _chunkCount = 0;
        
        chunk_map_t _primaryChunks;
        chunk_map_t _secondaryChunks;
        
        NodeChunk _moveSource;
        NodeAddress _moveDest;
    };
    
    class StepBuilder
    {
    public:
        StepBuilder(Step& step, bool isNew = false)
            : _step(step), _isNew(isNew)
        {
        }
        
        inline void addPrimaryChunk(NodeChunk chunk)
        {
            if (!_isNew)
            {
                auto f = _step.mapForward(chunk.address);
                auto b = _step.mapBackward(chunk.address);
                
                switch(_step.operation())
                {
                    case NodeOperation::Add:
                        chunk.address = f;
                        break;
                    case NodeOperation::Remove:
                        chunk.address = b;
                        break;
                    default:
                        break;
                }
            }
            
            _step.addPrimaryChunk(std::move(chunk));
        }
        
    private:
        Step& _step;
        bool _isNew;
    };
    
    struct Data : QSharedData
    {
        Data()
            : id(reinterpret_cast<quintptr>(this))
        {
        }
        
        
        Data(std::any root_) 
            : id(reinterpret_cast<quintptr>(this)),
             root(root_)
        {
        }
        
        Data(const Data& other) = default;
        Data(Data&&) = delete;
        
        inline ~Data();
        
        quintptr id;
        std::any root;
        QList<Step> steps;
        QSharedPointer<TreeObserverData> observerData;
    };
    
    void initData()
    {
        if (!_data)
            _data = new Data;
        else
            _data.detach();
    }
    
    StepBuilder addStep(NodeOperation type)
    {
        initData();
        
        bool isNew = false;
        
        if (_data->steps.isEmpty() || _data->steps.last().operation() != type)
        {
            _data->steps.append( Step(type) );
            isNew = true;
        }
        
        Step& step = _data->steps.last();
        return StepBuilder(step, isNew);
    }

    inline NodeAddress mapForward_impl(NodeAddress from, std::size_t progress = SIZE_MAX) const
    {
        if (!_data || from.isRoot()) 
            return from;
    
        NodeAddress result(from);
        
        for (const Step& step : _data->steps)
        {
            result = step.mapForward(result, progress);
            
            if (progress <= step.chunkCount())
                break;
            else
                progress -= step.chunkCount();
        }
        
        return result;
    }

    inline NodeAddress mapBackward_impl(NodeAddress from, std::size_t progress = SIZE_MAX) const
    {
        if (!_data || from.isRoot()) 
            return from;
        
        NodeAddress result(from);
        
        for (const Step& step : boost::adaptors::reverse(_data->steps))
        {
            result = step.mapBackward(result, progress);
            
            if (progress <= step.chunkCount())
                break;
            else
                progress -= step.chunkCount();
        }
        
        return result;
    }
    
    inline NodeEvent copyWithData(QSharedPointer<TreeObserverData> observerData) const
    {
        NodeEvent result(*this);
        result._data.detach();
        result._data->observerData = observerData;
        
        return result;
    }
    
    QSharedDataPointer<Data> _data;
    
    template<typename> friend class TreeObserver;
    friend class TreeObserverData;
    friend class NodeEventBuilder_impl;
    friend class NostalgicNodeEventHelper;
};

inline void NodeEvent::Step::addPrimaryChunk(NodeChunk&& chunk)
{
    if (chunk.length == 0 || chunk.address.isRoot())
        return;
    
    if (_operation == NodeOperation::Remove)
    {
        auto i = _primaryChunks.begin();
        auto mid = _primaryChunks.lowerBound(chunk.address); 
        auto end = _primaryChunks.end();
        
        // if the step contains an ancestor of `chunk`, do nothing.
        for (; i != mid; ++i)
        {
            if (i.key().isAncestorOf(chunk.address))
            {
                auto chunkListEnd = (*i).end();
                auto j = std::lower_bound(
                        (*i).begin(), 
                        chunkListEnd, 
                        chunk.address
                    );
                for (; j != chunkListEnd; ++j)
                {
                    if ((*j).coversAddress(chunk.address))
                        return;
                    
                    if ((*j).address > chunk.address)
                        break;
                }
            }
        }
        
        while (i != end)
        {
            if (chunk.coversAddress(i.key()))
            {
                _chunkCount -= (*i).size();
                i = _primaryChunks.erase(i);
            }
            else
            {
                ++i;
            }
            
            if (i.key().exceedsDescendantsOf(chunk.address))
                break;
        }
    }
    
    auto parentAddress = chunk.address.parent();
    
    if (!_primaryChunks.contains(parentAddress))
    {
        _primaryChunks[parentAddress] = { std::move(chunk) };
        ++_chunkCount;
        return;
    }
    
    chunk_list_t& siblings = _primaryChunks[parentAddress];
    bool mergeWithUpper = false;
    
    auto upperBound = std::upper_bound(
            siblings.begin(), 
            siblings.end(), 
            chunk
        );
    
    if (upperBound != siblings.end())
    {
        // `chunk` is an abutting or overlapping lower-sibling
        // of another chunk in this step. We note this for later
        mergeWithUpper = chunk.address.lastIndex() + chunk.length 
            >= (*upperBound).address.lastIndex();
    }
    
    if (upperBound != siblings.begin())
    {
        auto lowerBound = upperBound - 1;
        
        if (
            Q_UNLIKELY(
                (*lowerBound).address.lastIndex() + (*lowerBound).length
                >= chunk.address.lastIndex()
            )
        )
        {
            // `chunk` is an abutting or overlapping upper-sibling
            // of another chunk in this step.
            
            std::size_t overlap = (*lowerBound).address.lastIndex() 
                + (*lowerBound).length 
                - chunk.address.lastIndex();
            
            if (overlap >= chunk.length)
                // `chunk` is fully overlapped by a chunk already in
                // this step. This includes `chunk` being identical
                // to a chunk already in this step.
                return;
            
            if (Q_UNLIKELY(mergeWithUpper))
            {
                // `chunk` abutts and/or overlaps both its
                // neighboring siblings. Set the length of the lower
                // sibling to encompass all three, then remove the
                // upper sibling.
                
                (*lowerBound).length = (*upperBound).address.lastIndex()
                    + (*upperBound).length
                    - (*lowerBound).address.lastIndex();
                    
                siblings.erase(upperBound);
                --_chunkCount;
            }
            else
            {
                // Set the length of the lower sibling to encompass
                // itself and `chunk`
                (*lowerBound).length += chunk.length - overlap;
            }
            
            return;
        }
    }
    
    if (Q_UNLIKELY(mergeWithUpper))
    {
        // `chunk` abutts and/or overlaps its upper sibling, but not its 
        // lower sibling. Set the address of the upper sibling to that
        // of `chunk`, and set its length to encompass them both.
        
        (*upperBound).length = (*upperBound).address.lastIndex()
            + (*upperBound).length
            - chunk.address.lastIndex();
        
        (*upperBound).address = std::move(chunk.address);
        return;
    }
    
    // No merge
    siblings.insert(upperBound, std::move(chunk));
    ++_chunkCount;
}

template<
    // Determines the direction that the mapping adjusts an address,
    // i.e., up (true) or down (false). Additive mapping implements 
    // forward map for Add steps and backward map for Remove steps, 
    // subtractive mapping implements the reverse.
    bool additive,
        
    // Determines the order in which chunks are counted when mapping
    // against a step "in progress", i.e., from low to high (false, 
    // the default), or high to low (true). Progressing through the
    // chunks of add steps from low to high, and of remove steps 
    // from high to low can reduce the need for reindexing for some 
    // tasks.
    //
    // (The calculation always considers chunks in order from low to
    // high, this simply determines whether chunks are skipped at
    // the beginning or end of that process)
    bool progressReversed
>
inline NodeAddress NodeEvent::Step::map_impl(
        // The input address
        const NodeAddress& from,
        
        // Used to map against a step "in progress", i.e., only a
        // portion (the given number) of the chunks in the step are
        // considered.
        std::size_t progress,
        
        // If not null, the value pointed-to by wasDeleted is set to 
        // `true` if the node at `from` was deleted by this Remove step
        // (or this is a backward mapping of an Add step that added the
        // node)
        bool* wasDeleted
    ) const
{
    using namespace boost::adaptors;
    
    if (Q_UNLIKELY(!progress)) return from;
    // other fixed-point cases should be handled upstream
    
    NodeAddress result;
    
    progress = std::min(progress, _chunkCount);
    
    // Counts chunks as they are encountered from low to high. If
    // `!progressReversed`, then the mapping algorithm stops when this
    // reaches `progress`. If `progressReversed`, then the mapping
    // algorithm starts after it reaches `_size - progress`.
    std::size_t progressCount = 0;
    
    auto lowerBound = _primaryChunks.lowerBound(from);
    for (auto i = _primaryChunks.begin(); i != lowerBound; ++i)
    {
        if constexpr (progressReversed)
        {
            if (progressCount + (*i).size() < _chunkCount - progress)
            {
                progressCount += (*i).size();
                continue;
            }
        }
        
        const NodeAddress& ancestor = i.key();
        
        if constexpr (additive)
        {
            if (!ancestor.isAncestorOf(from))
            {
                progressCount += (*i).size();
                continue;
            }
        }
        else // !additive
        {
            if (ancestor > result) break;
            if (!ancestor.isAncestorOf(result) && ancestor != result) 
            {
                progressCount += (*i).size();
                continue;
            }
        }
        
        if (result.size() < ancestor.size())
        {
            // Copy any missing indices from `ancestor` into `result`
            auto ancestorEnd = ancestor.end();
            for (auto j = ancestor.begin() + result.size(); j != ancestorEnd; ++j)
                result = std::move(result) << *j;
        }
        
        // Find the "active" index that would be affected by the chunks
        // under `ancestor`
        
        const std::size_t sourceIndex = from[ancestor.size()];
        std::size_t index = sourceIndex;
        auto chunkListEnd = (*i).end();
        auto j = (*i).begin();
        
        if constexpr (progressReversed)
        {
            j += _chunkCount - progress - progressCount;
            progressCount = _chunkCount - progress;
        }
        
        for (; j != chunkListEnd; ++j)
        {
            if constexpr (!progressReversed)
            {
                if (progressCount >= progress)
                    goto limitReached;
            }
            
            bool relevant = false;
            if constexpr (additive)
            {
                if ((*j).address.lastIndex() <= index)
                {
                    index += (*j).length;
                    relevant = true;
                }
            }
            else // !additive
            {
                if ((*j).address.lastIndex() + (*j).length <= sourceIndex)
                {
                    if (index < (*j).length)
                    {
                        if (wasDeleted) *wasDeleted = true;
                        return NodeAddress();
                    }
                    else
                    {
                        index -= (*j).length;
                        relevant = true;
                    }
                }
            }
            
            if (!relevant)
            {
                // `*j` will have no bearing on `from`, nor will any
                // of its subsequent siblings.
                
                progressCount += std::distance(j, (*i).end());
                break; //inner loop
            }
            
            ++progressCount;
        }
        
        result = std::move(result) << index;
    }
    
limitReached:

    // Copy any missing indices from `from` into `result`
    if (result.size() < from.size())
    {
        auto fromEnd = from.end();
        for (auto k = from.begin() + result.size(); k != fromEnd; ++k)
            result = std::move(result) << *k;
    }
    
    return result;
}

/**
 * Utility for synchronizing the structures of multiple data trees across a node 
 * event.
 *
 * NostalgicNodeEventHelper emulates a sequence of simple incremental
 * transitions in the data tree structure, e.g., so the same structural changes
 * can be echoed in another tree. At any point, NostalgicNodeEventHelper may be 
 * queried for address mappings, node children counts, and safe access to 
 * underlying data.
 * 
 * The helper will also indicate an operation and an operating chunk. Calling 
 * `next()` on the helper emulates applying that operation to that chunk on the 
 * underlying tree.
 * 
 * A newly-initialized NostalgicNodeEventHelper is "fully nostalgic", and will
 * emulate the tree as if the event had never occurred. As the helper is 
 * progressed, it gradually "applies" changes until it reaches the end state of 
 * the event and is no longer nostalgic.
 * 
 * Note that the transitional states given by NostalgicNodeEventHelper may never
 * have existed on the underlying tree, only the end-state of the helper and the 
 * event are strictly equivalent. Chunks given by the helper are in an optimized
 * order and simplified, with redundancies omitted.
 * 
 * NostalgicNodeEventHelper is not thread-safe.
 */
class NostalgicNodeEventHelper
{
public:
    using NodeOperation = NodeEvent::NodeOperation;
    
    NostalgicNodeEventHelper() = default;
    NostalgicNodeEventHelper(NostalgicNodeEventHelper&&) = default;
    NostalgicNodeEventHelper& operator=(NostalgicNodeEventHelper&&) = default;
    
    NostalgicNodeEventHelper(const NostalgicNodeEventHelper& other)
        : _event(other._event),
        _progress(other._progress),
        _step(other._step),
        _stepEnd(other._stepEnd),
        _cChunk(other._cChunk),
        _fChunk(other._fChunk),
        _rCChunk(other._rCChunk),
        _rFChunk(other._rFChunk),
        _maxCacheSize(other._maxCacheSize)
    {
    }
    
    NostalgicNodeEventHelper& operator=(const NostalgicNodeEventHelper& other)
    {
        _event = other._event;
        _progress = other._progress;
        _step = other._step;
        _stepEnd = other._stepEnd;
        _cChunk = other._cChunk;
        _fChunk = other._fChunk;
        _rCChunk = other._rCChunk;
        _rFChunk = other._rFChunk;
        _maxCacheSize = other._maxCacheSize;
        
        invalidateCaches();
        
        return *this;
    }
    
    NostalgicNodeEventHelper(const NodeEvent& event)
        : _event(event)
    {
        if (!_event._data) return;
        
        _step       = _event._data->steps.begin();
        _stepEnd    = _event._data->steps.end();
        
        initChunk();
    }
    
    NostalgicNodeEventHelper(NodeEvent&& event)
        : _event(std::move(event))
    {
        if (!_event._data) return;
        
        _step       = _event._data->steps.begin();
        _stepEnd    = _event._data->steps.end();
        
        initChunk();
    }
    
    NostalgicNodeEventHelper& operator=(const NodeEvent& event)
    {
        _event = event;
        _progress = 0;
        
        if (_event._data)
        {
            _step       = _event._data->steps.begin();
            _stepEnd    = _event._data->steps.end();
        }
        else
        {
            _step       = {};
            _stepEnd    = {};
        }
        
        initChunk();
        invalidateCaches();
        
        return *this;
    }
    
    NostalgicNodeEventHelper& operator=(NodeEvent&& event)
    {
        _event = std::move(event);
        _progress = 0;
        
        if (_event._data)
        {
            _step       = _event._data->steps.begin();
            _stepEnd    = _event._data->steps.end();
        }
        else
        {
            _step       = {};
            _stepEnd    = {};
        }
        
        initChunk();
        invalidateCaches();
        
        return *this;
    }
    
    void clear() 
    {
        if (!_event._data) return;
        
        _event._data = nullptr;
        _progress = 0;
        _step = {};
        _stepEnd = {};
        
        initChunk();
        invalidateCaches();
    }
    
    NodeOperation operation() const 
    { 
        return _step != _stepEnd ?
            (*_step).operation() :
            (NodeOperation)(NULL); 
    }
    
    NodeChunk operatingChunk() const
    {
        if (_step != _stepEnd)
        {
            switch ((*_step).operation())
            {
                case NodeOperation::Add:
                    if (Q_LIKELY(_fChunk != (*_cChunk).end()))
                        return *_fChunk;
                    break;
                    
                case NodeOperation::Remove:
                    if (Q_LIKELY( _rFChunk != std::make_reverse_iterator(
                            (*_rCChunk).begin()
                        ) ))
                        return *_rFChunk;
                    break;
                    
                default:
                    Q_UNREACHABLE();
            }
        }
        
        return NodeChunk { NodeAddress(), 0 };
    }
    
    
    bool hasNext() const { return _event._data && _step != _stepEnd; }
    
    bool next()
    {
        if (!_event._data || _step == _stepEnd) return false;
        
        switch ((*_step).operation())
        {
            case NodeOperation::Add:
                ++_fChunk;
                
                if (_fChunk != (*_cChunk).end())
                    break;
                
                ++_cChunk;
                
                if (_cChunk != (*_step).primaryChunks().end())
                {
                    _fChunk = (*_cChunk).begin();
                    break;
                }
                
                ++_step;
                initChunk();
                break;
                
            case NodeOperation::Remove:
                ++_rFChunk;
                
                if (_rFChunk != std::make_reverse_iterator(
                        (*_rCChunk).begin()
                    ))
                    break;
                
                ++_rCChunk;
                
                if (_rCChunk != std::make_reverse_iterator(
                        (*_step).primaryChunks().begin()
                    ))
                {
                    _rFChunk = std::make_reverse_iterator((*_rCChunk).end());
                    break;
                }
                
                ++_step;
                initChunk();
                break;
                
            default:
                Q_UNREACHABLE();
        }
        
        ++_progress;
        
        if (_step == _stepEnd)
        {
            clear();
            return false;
        }
        else
        {
            return true;
        }
    }
    
    NodeAddress mapForward(NodeAddress from) const
    {
        if (!_event._data || _step == _stepEnd) 
            return from;
        
        auto cacheIndex = qMakePair(from, _progress);
        
        if (_forwardMapCache.contains(cacheIndex))
        {
            auto i = noDetach(_forwardMapCache)[cacheIndex];
            
            // Prioritize this entry against being pruned in the future.
            _mapCacheQueue.splice(_mapCacheQueue.begin(), _mapCacheQueue, i);
            
            return (*i).toAddress;
        }
        
        NodeAddress result = _event.mapForward_impl(from, _progress);
        
        {
            auto i = _mapCacheQueue.insert(
                    _mapCacheQueue.end(), 
                    { result, from, _progress }
                );
    
            _forwardMapCache[cacheIndex] = i;
            _backwardMapCache[qMakePair(result, _progress)] = i;
            
            pruneCaches();
        }
        
        return result;
    }
    
    NodeAddress mapBackward(NodeAddress from) const
    {
        if (!_event._data || _step == _stepEnd) 
            return from;
        
        auto cacheIndex = qMakePair(from, _progress);
        
        if (_backwardMapCache.contains(cacheIndex))
        {
            auto i = noDetach(_backwardMapCache)[cacheIndex];
            
            // Prioritize this entry against being pruned in the future.
            _mapCacheQueue.splice(_mapCacheQueue.begin(), _mapCacheQueue, i);
            
            return (*i).toAddress;
        }
        
        NodeAddress result = _event.mapBackward_impl(from, _progress);
        
        {
            auto i = _mapCacheQueue.insert(
                    _mapCacheQueue.end(), 
                    { result, from, _progress }
                );
    
            _backwardMapCache[cacheIndex] = i;
            _forwardMapCache[qMakePair(result, _progress)] = i;
            
            pruneCaches();
        }
        
        return result;
    }
    
    std::size_t maxCacheSize() const { return _maxCacheSize; }
    void setMaxCacheSize(std::size_t size)
    {
        _maxCacheSize = size;
        
        if (size == 0)
        {
            _mapCacheQueue.clear();
            _forwardMapCache.clear();
            _backwardMapCache.clear();
        }
        else
        {
            pruneCaches();
        }
    }
    
private:
    using step_iterator = QList<NodeEvent::Step>::const_iterator;
    
    using coarse_chunk_iterator = NodeEvent::chunk_map_t::const_iterator;
    using fine_chunk_iterator = NodeEvent::chunk_list_t::const_iterator;
    
    using r_coarse_chunk_iterator = std::reverse_iterator<coarse_chunk_iterator>;
    using r_fine_chunk_iterator = std::reverse_iterator<fine_chunk_iterator>;
    
    struct CacheQueueEntry
    {
        NodeAddress fromAddress;
        NodeAddress toAddress;
        std::size_t progress;
    };
    
    void initChunk()
    {
        if (!_event._data || _step == _stepEnd)
        {
            _cChunk     = {};
            _fChunk     = {};
            _rCChunk    = {};
            _rFChunk    = {};
            
            return;
        }
        
        switch((*_step).operation())
        {
            case NodeOperation::Add:
                _rCChunk    = {};
                _rFChunk    = {};
                
                _cChunk     = (*_step).primaryChunks().begin();
                _fChunk     = (*_cChunk).begin();
                
                break;
                
            case NodeOperation::Remove:
                _cChunk     = {};
                _fChunk     = {};
                
                _rCChunk    = std::make_reverse_iterator((*_step).primaryChunks().end());
                _rFChunk    = std::make_reverse_iterator((*_rCChunk).end());
                
                break;
                
            default:
                Q_UNREACHABLE();
        }
    }
    
//     std::size_t totalChunkCount() const
//     {
//         if (!_event || !_event->_data) return 0;
//         
//         if (_totalChunkCountCache == SIZE_MAX)
//         {
//             _totalChunkCountCache = 0;
//             for (const auto& step : _event->_data->steps)
//                 _totalChunkCountCache += step.chunkCount();
//         }
//         
//         return _totalChunkCountCache;
//     }
    
    void invalidateCaches()
    {
        _mapCacheQueue.clear();
        _forwardMapCache.clear();
        _backwardMapCache.clear();
        
//         _totalChunkCountCache = SIZE_MAX;
    }
    
    void pruneCaches() const
    {
        while (_mapCacheQueue.size() > _maxCacheSize)
        {
            const auto& last = _mapCacheQueue.back();
            
            _forwardMapCache.remove(qMakePair(last.fromAddress, last.progress));
            _backwardMapCache.remove(qMakePair(last.toAddress, last.progress));
            
            _mapCacheQueue.pop_back();
        }
    }
        
    NodeEvent _event;
    std::size_t _progress = 0;
    
    step_iterator _step = {};
    step_iterator _stepEnd = {};
    
    coarse_chunk_iterator   _cChunk = {};
    fine_chunk_iterator     _fChunk = {};
    r_coarse_chunk_iterator _rCChunk = {};
    r_fine_chunk_iterator   _rFChunk = {};
    
    std::size_t _maxCacheSize = 1024;
            
    using map_cache_queue_t = std::list<CacheQueueEntry>;
            
//     mutable std::size_t _totalChunkCountCache = SIZE_MAX;
    
    // A collection of cached node address mappings, in order of recent access.
    // If the size exceeds _maxCacheSize, then the oldest entries are deleted
    // first.
    mutable map_cache_queue_t _mapCacheQueue;
    
    // Fast lookup indices for node address mappings. 
    // TODO std::unordered_map is probably better (we don't need or want
    // implicit sharing)
    mutable QHash<QPair<NodeAddress, std::size_t>, map_cache_queue_t::const_iterator> _forwardMapCache;
    mutable QHash<QPair<NodeAddress, std::size_t>, map_cache_queue_t::const_iterator> _backwardMapCache;
};

class NodeEventBuilder_impl
{
    using StepType = NodeEvent::NodeOperation;
    
public:
    NodeEventBuilder_impl(NodeEvent& event)
        : _event(event)
    {
    }
  
    const NodeEvent& event() const { return _event; }
    NodeEvent& event() { return _event; }
    
    inline void addChunk(NodeAddress address, std::size_t length)
    {
        auto handle = _event.addStep(StepType::Add);
        handle.addPrimaryChunk({ std::move(address), length });
    }
    
    template<typename Range>
    inline void addChunks(Range&& range)
    {
        static_assert(
            std::is_convertible_v<
                typename boost::range_reference<boost::remove_cv_ref_t<Range>>::type,
                NodeChunk
            >,
            "This method may only be used for ranges of NodeChunk"
        );
        
        if (!boost::empty(range))
        {
            auto handle = _event.addStep(StepType::Add);
            
            QVarLengthArray<NodeChunk, 64> buffer(boost::begin(range), boost::end(range));
            std::sort(buffer.begin(), buffer.end());
            
            for (NodeChunk& entry : buffer)
                handle.addPrimaryChunk(std::move(entry));
        }
    }
    
    inline void removeChunk(NodeAddress address, std::size_t length)
    {
        auto handle = _event.addStep(StepType::Remove);
        handle.addPrimaryChunk({ std::move(address), length });
    }
    
    template<typename Range>
    inline void removeChunks(Range&& range)
    {
        static_assert(
            std::is_convertible_v<
                typename boost::range_reference<boost::remove_cv_ref_t<Range>>::type,
                NodeChunk
            >,
            "This method may only be used for ranges of NodeChunk"
        );
        
        if (!boost::empty(range))
        {
            auto handle = _event.addStep(StepType::Remove);
            
            QVarLengthArray<NodeChunk, 64> buffer(boost::begin(range), boost::end(range));
            std::sort(buffer.begin(), buffer.end());
            
            for (NodeChunk& entry : buffer)
                handle.addPrimaryChunk(std::move(entry));
        }
    }
    
private:
    NodeEvent& _event;
};

class TreeObserverData
{
public:    
    TreeObserverData(const TreeObserverData&) = delete;
    TreeObserverData(TreeObserverData&&) = delete;
    
    ~TreeObserverData()
    { 
        const QWriteLocker locker(&_lock);
        assert(_events.empty() && _isDeleted);
    }
    
    void createNodeEvent()
    {
        const QWriteLocker locker(&_lock);
        
        NodeEvent event;
        event.initData();
        auto id = event._data->id;
        
        auto i = _events.insert(
                _events.end(),
                { std::move(event), id, true }
            );
        _eventIndex[id] = i;
    }
    
    void releaseNodeEvent(quintptr id)
    {
        const QWriteLocker locker(&_lock);
        
        if(Q_UNLIKELY(!id || !_eventIndex.contains(id))) return;
        
        auto i = noDetach(_eventIndex)[id];
        if (i == _events.begin())
        {
            // The event being released is the oldest active event on record.
            // There may be younger inactive events that are now ready to be
            // deleted.
            
            // However, we keep the most recent 
            
            auto end = --(_events.end());
            
            if (_isRecording)
                --end;
            
            for (; i != end; ++i)
            {
                if ((*i).isActive) 
                    break;
                else
                    _eventIndex.remove((*i).id);
            }
            
            _events.erase(_events.begin(), i);
        }
        else
        {
            (*i).isActive = false;
        }
    }
    
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
    
    std::unique_ptr<QWriteLocker> lockForWrite()
    {
        return std::make_unique<QWriteLocker>(&_lock);
    }
    
private:
    TreeObserverData(std::any root)
        : _root(root)
    {
    }
    
    std::any _root;
    
    struct EventEntry
    {
        NodeEvent event;
        quintptr id;
        bool isActive;
    };
    using node_event_list = std::list<EventEntry>;
    
    node_event_list _events;
    QHash<quintptr, node_event_list::iterator> _eventIndex;
    
    bool _isDeleted = false;
    bool _isRecording = false;
    
    mutable QReadWriteLock _lock = QReadWriteLock(QReadWriteLock::Recursive);
    
    template<typename> friend class TreeObserver;
};

inline NodeEvent::Data::~Data()
{
    if (observerData) 
        observerData->releaseNodeEvent(id);
}

template<typename Tree>
class TreeObserver
{
public:
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
        const auto lock = _data->lockForWrite();
        assert(!_data->_isRecording);
        
        _data->_isRecording = true;
        _data->createNodeEvent();
    }
    
    NodeEvent finishRecording()
    {
        const auto lock = _data->lockForWrite();
        assert(_data->_isRecording);
        
        _data->_isRecording = false;
        return _data->_events.back().event.copyWithData(_data);
    }
    
    NodeEvent lastNodeEvent() const
    {
        const auto lock = _data->lockForRead();
        
        auto i = _data->_events.crbegin();
        auto rend = _data->_events.crend();
        
        if (_data->_isRecording && i != rend)
            ++i;
        
        if (i != rend)
        {
            NodeEvent event(*i);
            return event.copyWithData(_data);
        }
        else
        {
            return NodeEvent();
        }
    }
    
private:
    ::Addle::aux_datatree::node_handle_t<Tree> _root = {};
    QSharedPointer<TreeObserverData> _data;
};

}

using DataTreeNodeEvent = aux_datatree2::NodeEvent;

class DataTreeNodeEventBuilder
{
    using _impl = aux_datatree2::NodeEventBuilder_impl;
public:
    DataTreeNodeEventBuilder() = default;
    
    template< typename Tree_>
    DataTreeNodeEventBuilder(Tree_& tree)
        : _event(::Addle::aux_datatree::tree_root(tree))
    {
    }
    
    DataTreeNodeEventBuilder(DataTreeNodeEventBuilder&&) = default;
    DataTreeNodeEventBuilder& operator=(DataTreeNodeEventBuilder&&) = default;
    
    DataTreeNodeEvent event() const { return _event; }
    
    operator const DataTreeNodeEvent& () const & { return _event; }
    operator DataTreeNodeEvent () && { return std::move(_event); }
    
    inline DataTreeNodeEventBuilder& addAddress(DataTreeNodeAddress address)
    {
        _impl(_event).addChunk(std::move(address), 1);
        return *this;
    }
    
    template<typename Range>
    inline DataTreeNodeEventBuilder& addAddresses(Range&& range)
    {
        static_assert(
            std::is_convertible_v<
                typename boost::range_reference<boost::remove_cv_ref_t<Range>>::type,
                DataTreeNodeAddress
            >,
            "This method may only be used for ranges of NodeAddress"
        );
        
        if (!boost::empty(range))
        {
            using namespace boost::adaptors;
            _impl(_event).addChunks(
                    range 
                    | transformed([] (auto&& address) { 
                        return aux_datatree2::NodeChunk { std::forward<decltype(address)>(address), 1 };
                    })
                );
        }
        
        return *this;
    }
    
    inline DataTreeNodeEventBuilder& addAddresses(const std::initializer_list<DataTreeNodeAddress>& addresses)
    {
        return addAddresses<const std::initializer_list<DataTreeNodeAddress>&>(addresses);
    }
    
    inline DataTreeNodeEventBuilder& addChunk(DataTreeNodeAddress address, std::size_t length)
    {
        _impl(_event).addChunk(std::move(address), length);
        return *this;
    }
    
    template<typename Range>
    inline DataTreeNodeEventBuilder& addChunks(Range&& range)
    {
        _impl(_event).addChunks(std::forward<Range>(range));
        return *this;
    }
    
    inline DataTreeNodeEventBuilder& addChunks(const std::initializer_list<aux_datatree2::NodeChunk>& chunks)
    {
        return addChunks<const std::initializer_list<aux_datatree2::NodeChunk>&>(chunks);
    }
    
    inline DataTreeNodeEventBuilder& removeAddress(DataTreeNodeAddress address)
    {
        _impl(_event).removeChunk(std::move(address), 1);
        return *this;
    }
    
    template<typename Range>
    inline DataTreeNodeEventBuilder& removeAddresses(Range&& range)
    {
        static_assert(
            std::is_convertible_v<
                typename boost::range_reference<boost::remove_cv_ref_t<Range>>::type,
                DataTreeNodeAddress
            >,
            "This method may only be used for ranges of NodeAddress"
        );
        
        if (!boost::empty(range))
        {
            using namespace boost::adaptors;
            _impl(_event).removeChunks(
                    range 
                    | transformed([] (auto&& address) { 
                        return aux_datatree2::NodeChunk { std::forward<decltype(address)>(address), 1 };
                    })
                );
        }
        
        return *this;
    }
    
    inline DataTreeNodeEventBuilder& removeAddresses(const std::initializer_list<DataTreeNodeAddress>& addresses)
    {
        return removeAddresses<const std::initializer_list<DataTreeNodeAddress>&>(addresses);
    }
    
    inline DataTreeNodeEventBuilder& removeChunk(DataTreeNodeAddress address, std::size_t length)
    {
        _impl(_event).removeChunk(std::move(address), length);
        return *this;
    }
    
    template<typename Range>
    inline DataTreeNodeEventBuilder& removeChunks(Range&& range)
    {
        _impl(_event).removeChunks(std::forward<Range>(range));
        return *this;
    }
    
    inline DataTreeNodeEventBuilder& removeChunks(const std::initializer_list<aux_datatree2::NodeChunk>& chunks)
    {
        return removeChunks<const std::initializer_list<aux_datatree2::NodeChunk>&>(chunks);
    }
    
private:
    DataTreeNodeEvent _event;
};
}
