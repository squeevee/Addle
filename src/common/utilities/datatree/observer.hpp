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
class NostalgicNodeEventHelper;

template<bool additive>
inline NodeAddress mapSingle_impl(
        const NodeAddress& from,
        const NodeChunk& rel,
        bool* terminal = nullptr)
{
    if (from.isRoot()
            || rel.address.isRoot()
            || rel.address > from 
            || rel.address.size() > from.size()
            || !rel.length
        )
    {
        return from;
    }
    
    if (terminal) 
        *terminal = false;
    
    NodeAddressBuilder result;
    
    auto i = from.begin();
    
    auto j = rel.address.begin();
    auto relEnd = rel.address.end();
    
    while (true)
    {
        std::size_t index = *i;
        std::size_t relIndex = *j;
        
        ++i;
        ++j;
        
        if (j == relEnd)
        {
            if (index >= relIndex)
            {
                if constexpr (additive)
                {
                    result.append(index + rel.length);
                }
                else // !additive
                {
                    if (index > rel.length)
                    {
                        result.append(index - rel.length);
                    }
                    else
                    {
                        if (terminal) 
                            *terminal = true;
                        
                        return NodeAddress();
                    }
                }
            }
            
            break;
        }
        else
        {
            result.append(index);
        }
    }
    
    if (result.size() < from.size())
    {
        auto fromEnd = from.end();
        for (auto k = from.begin() + result.size(); k != fromEnd; ++k)
            result.append(*k);
    }
    
    return std::move(result);
}

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
    
    inline NodeAddress mapForward(
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

    inline NodeAddress mapBackward(
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
    
    inline std::size_t childCount(
            NodeAddress parent, 
            std::size_t progress = SIZE_MAX) const
    {
        if (!_data) return 0;
        
        std::ptrdiff_t result = _data->initialChildCounts[parent];
        
        for (const Step& step : _data->steps)
        {
            std::ptrdiff_t offset = step.childCountOffset(parent, progress);
            
            result += offset;
            
            if (progress > step.chunkCount())
                progress -= step.chunkCount();
            else
                break;
        }
        
        return std::max(result, (std::ptrdiff_t)0);
    }
    
    std::any root() const { return _data ? _data->root : std::any(); }
    
    bool isCoordinated() const { return _data && _data->observerData; }
    
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
        const chunk_list_t& secondaryChunks() const { return _secondaryChunks; }
        
        inline NodeAddress mapForward(
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
        
        inline NodeAddress mapBackward(
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
        
        inline std::ptrdiff_t childCountOffset(
                NodeAddress parent, 
                std::size_t progress = SIZE_MAX) const
        {
            using namespace boost::adaptors;
            
            if (_primaryChunks.empty() || !progress)
                return 0;
            
            progress = std::min(progress, _chunkCount);
            
            switch(_operation)
            {
                case NodeOperation::Add:
                {
                    if (progress < _chunkCount)
                    {                        
                        auto lowerBound = _primaryChunks.lower_bound(parent);
                        auto lowerIter = _primaryChunks.begin();
                        
                        for (; lowerIter != lowerBound; ++lowerIter)
                        {
                            if (progress > (*lowerIter).second.size())
                                progress -= (*lowerIter).second.size();
                            else
                                return 0;
                        }
                    }
                    
                    std::ptrdiff_t result = 0;
                    
                    auto childrenFind = _primaryChunks.find(parent);
                    if (childrenFind != _primaryChunks.end())
                    {
                        auto childIter = (*childrenFind).second.begin();
                        auto childrenEnd = (*childrenFind).second.end();
                        
                        for (; childIter != childrenEnd; ++childIter)
                        {
                            if (!progress) break;
                            
                            result += (*childIter).length;
                            --progress;
                        }
                    }
                    
                    return result;
                }
                    
                case NodeOperation::Remove:
                {
                    if (progress < _chunkCount)
                    {
                        auto bound = std::make_reverse_iterator(
                                _primaryChunks.upper_bound(parent)
                            );
                        auto upperIter = std::make_reverse_iterator(
                                _primaryChunks.end()
                            );
                        
                        for (; upperIter != bound; ++upperIter)
                        {
                            if (progress > (*upperIter).second.size())
                                progress -= (*upperIter).second.size();
                            else
                                return 0;
                        }
                    }
                    
                    std::ptrdiff_t result = 0;
                    
                    auto childrenFind = _primaryChunks.find(parent);
                    if (childrenFind != _primaryChunks.end())
                    {
                        auto childIter = (*childrenFind).second.rbegin();
                        auto childrenEnd = (*childrenFind).second.rend();
                        
                        for (; childIter != childrenEnd; ++childIter)
                        {
                            if (!progress) break;
                            
                            result -= (*childIter).length;
                            --progress;
                        }
                    }
                    
                    return result;
                }
                
//                 case NodeOperation::Move: // TODO
                    
                default:
                    Q_UNREACHABLE();
            }
        }
        
        // Adjusts relevant addresses in this Add step as if `rel` had been 
        // inserted first. (this is occasionally necessary if merging `rel` 
        // would require an address with a negative index.)
        inline void rebase(const NodeChunk& rel)
        {
            assert(_operation == NodeOperation::Add);
            
            {
                auto i = _primaryChunks.lower_bound(rel.address);
                while (i != _primaryChunks.end())
                {
                    for (NodeChunk& chunk : (*i).second)
                        chunk.address = mapSingle_impl<true>(chunk.address, rel);
                    
                    NodeAddress parentAddress = (*i).first;
                    NodeAddress mappedParentAddress = 
                        mapSingle_impl<true>(parentAddress, rel);
                        
                    if (parentAddress != mappedParentAddress)
                    {
                        auto nh = _primaryChunks.extract(i);
                        nh.key() = mappedParentAddress;
                        auto ins = _primaryChunks.insert(std::move(nh));
                        
                        assert(ins.inserted);
                        i = ins.position;
                    }
                }
            }
            
            {
                auto j = _primaryChunks.find(rel.address.parent());
                if (j == _primaryChunks.end())
                    return;
                    
                auto chunksEnd = (*j).second.end();
                auto k = std::lower_bound(
                        (*j).second.begin(),
                        chunksEnd,
                        rel.address
                    );
                
                for (; k != chunksEnd; ++k)
                    (*k).address = mapSingle_impl<true>((*k).address, rel);
            }
        }
        
        // Adds a chunk to this step, eliding or merging it with existing chunks
        // as necessary.
        inline void addPrimaryChunk(NodeChunk&& chunk);
        
    private:
                
        template<bool additive, bool progressReversed = false>
        inline NodeAddress primaryMap_impl(
                const NodeAddress& from, 
                std::size_t progress = SIZE_MAX, 
                bool* terminal = nullptr
            ) const;
        NodeOperation _operation;
        
        std::size_t _chunkCount = 0;
        
        chunk_map_t _primaryChunks;
        chunk_list_t _secondaryChunks;
        
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
        
        inline void addPrimaryChunk(NodeChunk chunk)
        {
            if (!_isNew)
            {
                bool terminal = false;
                NodeAddress newAddress;
                
                switch(_step.operation())
                {
                    case NodeOperation::Add:
                        newAddress = _step.mapBackward(
                                chunk.address, 
                                SIZE_MAX, 
                                &terminal
                            );
                        break;
                        
                    case NodeOperation::Remove:
                        newAddress = _step.mapForward(
                                chunk.address, 
                                SIZE_MAX, 
                                &terminal
                            );
                        break;
                        
                    default:
                        break;
                }
                
                if (terminal)
                {
                    _step.rebase(chunk);
                }
                else
                {
                    chunk.address = std::move(newAddress);
                }
            }
            
            std::size_t countBefore = _step.chunkCount();
            _step.addPrimaryChunk(std::move(chunk));
            std::size_t countAfter = _step.chunkCount();
            
            if (countAfter > countBefore)
                _totalChunkCount += (countAfter - countBefore);
            else if (countAfter < countBefore)
                _totalChunkCount -= (countBefore - countAfter);
        }
        
    private:
        Step& _step;
        std::size_t& _totalChunkCount;
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
        
        Data(Data&&) = delete;
        
        inline Data(const Data& other);     // defined after TreeObserverData
        inline ~Data();                     // defined after TreeObserverData
        
        quintptr id;
        
        std::any root;
        QList<Step> steps;
        std::size_t totalChunkCount = 0;
        QHash<NodeAddress, std::size_t> initialChildCounts;
        
        QSharedPointer<TreeObserverData> observerData;
        unsigned* observerRefCount = nullptr;
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
        
        // Not allowed to add steps to a detached copy of an observer event
        assert(
            !_data->observerData
            || _data->id == reinterpret_cast<quintptr>(qAsConst(_data).data())
        );
        
        bool isNew = false;
        
        if (_data->steps.isEmpty() || _data->steps.last().operation() != type)
        {
            _data->steps.append( Step(type) );
            isNew = true;
        }
        
        Step& step = _data->steps.last();
        return StepBuilder(step, _data->totalChunkCount, isNew);
    }
    
    
    inline void addChunk_impl(NodeAddress address, std::size_t length)
    {
        auto handle = addStep(NodeOperation::Add);
        handle.addPrimaryChunk({ std::move(address), length });
    }
    
    template<typename Range>
    inline void addChunks_impl(Range&& range)
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
                    boost::begin(range), 
                    boost::end(range)
                );
            std::sort(buffer.begin(), buffer.end());
            // sorting beforehand is by no means required, but it guarantees a
            // best-case performance for addPrimaryChunk
            
            for (NodeChunk& entry : buffer)
                handle.addPrimaryChunk(std::move(entry));
        }
    }
    
    inline void removeChunk_impl(NodeAddress address, std::size_t length)
    {
        auto handle = addStep(NodeOperation::Remove);
        handle.addPrimaryChunk({ std::move(address), length });
    }
    
    template<typename Range>
    inline void removeChunks_impl(Range&& range)
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
                    boost::begin(range), 
                    boost::end(range)
                );
            std::sort(buffer.begin(), buffer.end());
            
            for (NodeChunk& entry : buffer)
                handle.addPrimaryChunk(std::move(entry));
        }
    }
    
    void setInitialChildCount_impl(
            NodeAddress address, 
            std::size_t size, 
            bool ignoreIfSet = true)
    {
        initData();
        
        if (!ignoreIfSet || !_data->initialChildCounts.contains(address))
            _data->initialChildCounts[address] = size;
    }
    
    QSharedDataPointer<Data> _data;
    
    template<typename> friend class TreeObserver;
    template<class> friend class NodeEventTreeHelper;
    friend class TreeObserverData;
    friend class NostalgicNodeEventHelper;
    friend class NodeEventBuilder;
    
#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
};

inline void NodeEvent::Step::addPrimaryChunk(NodeChunk&& chunk)
{
    if (chunk.length == 0 || chunk.address.isRoot())
        return;
    
    if (_operation == NodeOperation::Remove)
    {
        auto i = _primaryChunks.begin();
        auto mid = _primaryChunks.lower_bound(chunk.address); 
        
        // if the step contains an ancestor of `chunk`, do nothing.
        for (; i != mid; ++i)
        {
            const NodeAddress& key = (*i).first;
            const chunk_list_t& chunks = (*i).second;
            
            if (key.isAncestorOf(chunk.address))
            {
                auto chunkListEnd = chunks.end();
                auto j = std::lower_bound(
                        chunks.begin(), 
                        chunkListEnd, 
                        chunk.address
                    );
                for (; j != chunkListEnd; ++j)
                {
                    if ((*j).coversAddress(chunk.address))
                        return;
                    
                    if ((*j).address > chunk.address)
                        break; // chunk list loop
                }
            }
        }
        
        // if the step contains descendants of `chunk`, remove them.
        while (i != _primaryChunks.end())
        {
            const NodeAddress& key = (*i).first;
            
            if (chunk.coversAddress(key))
            {
                _chunkCount -= (*i).second.size();
                i = _primaryChunks.erase(i);
            }
            else
            {
                ++i;
            }
            
//             auto commonAncestor = key.lastCommonAncestor(chunk.address);
//             if (commonAncestor != key.end() 
//                 && *commonAncestor)
//                 break;
        }
    }
    
    auto parentAddress = chunk.address.parent();
    
    auto k = _primaryChunks.find(parentAddress);
    if (k == _primaryChunks.end())
    {
        _primaryChunks[parentAddress] = { std::move(chunk) };
        ++_chunkCount;
        return;
    }
    
    chunk_list_t& siblings = (*k).second;
    bool mergeWithUpper = false;
    
    auto upperBound = std::upper_bound(
            siblings.begin(), 
            siblings.end(), 
            chunk
        );
    
    if (upperBound != siblings.end())
    {
        // Test if `chunk` is an abutting or overlapping lower-sibling of 
        // another chunk in this step, and note this for later.
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
            
            std::size_t overhang = (*lowerBound).address.lastIndex() 
                + (*lowerBound).length 
                - chunk.address.lastIndex();
            
            if (overhang >= chunk.length)
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
                (*lowerBound).length += chunk.length - overhang;
            }
            
            return;
        }
    }
    
    if (Q_UNLIKELY(mergeWithUpper))
    {
        // `chunk` abutts and/or overlaps its upper sibling, but not its lower 
        // sibling. Set the address of the upper sibling to that of `chunk`, and 
        // set its length to encompass them both.
        
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
    // Determines the direction that the mapping adjusts an address, i.e., up 
    // (true) or down (false). Additive mapping implements forward map for Add 
    // steps and backward map for Remove steps, subtractive mapping implements 
    // the reverse.
    bool additive,
        
    // Determines the order in which chunks are counted when mapping against a 
    // step "in progress", i.e., from low to high (false, the default), or high 
    // to low (true).
    //
    // (The calculation itself always iterates chunks in order from low to high.
    // This simply determines, if progress is less than the size of the step,
    // whether to skip the chunks at the beginning or the end.)
    bool progressReversed
>
inline NodeAddress NodeEvent::Step::primaryMap_impl(
        // The input address
        const NodeAddress& from,
        
        // Used to map against a step "in progress", i.e., only a portion (the
        // given number) of the chunks in the step are considered.
        std::size_t progress,
        
        // If not null, the value pointed-to by `terminal` is set to true if:
        // - This call implements the forward mapping of a Remove step that 
        // removed the given node.
        // - This call implements the backward mapping of an Add step that added 
        // the given node.
        // 
        // The value is set to false otherwise. `progress` is taken into account
        // where applicable.
        bool* terminal) const
{
    using namespace boost::adaptors;
    
    if (Q_UNLIKELY(!progress)) return from;
    // other fixed-point cases should be handled upstream
    
    NodeAddressBuilder result;
    
    progress = std::min(progress, _chunkCount);
    
    // Counts chunks as they are encountered from low to high. If
    // `!progressReversed`, then the mapping algorithm stops when this reaches
    // `progress`. If `progressReversed`, then the mapping algorithm starts 
    // after it reaches `_size - progress`.
    std::size_t progressCount = 0;
    
    auto lowerBound = _primaryChunks.lower_bound(from);
    for (auto i = _primaryChunks.begin(); i != lowerBound; ++i)
    {
        const NodeAddress& key = (*i).first;
        const chunk_list_t& chunks = (*i).second;
        
        if constexpr (progressReversed)
        {
            if (progressCount + chunks.size() < _chunkCount - progress)
            {
                progressCount += chunks.size();
                continue;
            }
        }
        
        if constexpr (additive)
        {
            if (!key.isAncestorOf(from))
            {
                progressCount += chunks.size();
                continue;
            }
        }
        else // !additive
        {
            if (key > result) break;
            if (!key.isAncestorOf(result) && key != result) 
            {
                progressCount += chunks.size();
                continue;
            }
        }
        
        if (result.size() < key.size())
        {
            // This address is deeper in the tree than previous ones. Copy the 
            // missing indices from `key` into `result`
            auto keyEnd = key.end();
            for (auto j = key.begin() + result.size(); j != keyEnd; ++j)
                result.append(*j);
        }
        
        // Find the "active" index that would be affected by the chunks
        // under `ancestor`
        
        const std::size_t sourceIndex = from[key.size()];
        std::size_t index = sourceIndex;
        auto chunksEnd = chunks.end();
        auto j = chunks.begin();
        
        if constexpr (progressReversed)
        {
            j += _chunkCount - progress - progressCount;
            progressCount = _chunkCount - progress;
        }
        
        for (; j != chunksEnd; ++j)
        {
            if constexpr (!progressReversed)
            {
                if (progressCount >= progress)
                    goto limitReached; // break from _primaryChunks loop
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
                if ((*j).address.lastIndex() <= sourceIndex)
                {
                    if (index < (*j).length)
                    {
                        if (terminal) *terminal = true;
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
                
                progressCount += std::distance(j, chunksEnd);
                break; // chunk list loop
            }
            
            ++progressCount;
        }
        
        result.append(index);
    }
    
limitReached:
    
    if (result.size() < from.size())
    {
        // `from` is deeper into the tree than any chunks in this step. Copy the 
        // missing indices from `from` into `result`
        
        auto fromEnd = from.end();
        for (auto k = from.begin() + result.size(); k != fromEnd; ++k)
            result.append(*k);
    }
    
    return std::move(result);
}


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
    auto insertNodes(
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
        
        _event.setInitialChildCount_impl(
                _event.mapBackward(parentAddress), initialChildCount
            );
        _event.addChunk_impl( 
                NodeAddressBuilder(parentAddress) << startIndex,
                valuesCount 
            );  
        
        return result;
    }
    
    template<typename Range, 
        typename std::enable_if_t<!std::is_convertible_v<
            typename boost::range_category<boost::remove_cv_ref_t<Range>>::type,
            std::forward_iterator_tag
        >,
        void*> = nullptr>
    auto insertNodes(
            handle_t parent, 
            child_handle_t before, 
            Range&& nodeValues
        )
    {
        using buffer_t = QVarLengthArray<
                typename boost::range_value<boost::remove_cv_ref_t<Range>>::type, 
                64
            >;
        using forward_range_t = boost::iterator_range<
                std::move_iterator<typename buffer_t::iterator>
            >;
        
        buffer_t buffer(boost::begin(nodeValues), boost::end(nodeValues));
        
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
        
        _event.setInitialChildCount_impl(
                _event.mapBackward(parentAddress), initialChildCount
            );
        _event.removeChunk_impl(
                NodeAddressBuilder(parentAddress) << startIndex, 
                count
            );
    }
    
    handle_t _root;
    NodeEvent& _event;
};

class TreeObserverData
{
    
    struct EventEntry
    {
        NodeEvent event;
        quintptr id;
        
        // TODO: serialize ref counting with atomic int instead of mutex. The 
        // main challenge of this is that this is not a true ref count -- the 
        // observer data always keeps a single implicit semiweak reference.
        // Therefore a refCount of zero does not guarantee there will be no 
        // subsequent access from another thread as it might with other shared 
        // data.
        // 
        // This can likely be worked around by using a reference-counted object
        // to store the reference count itself (gives observer data maximum 
        // freedom to manage itself while avoiding memory access races) as well 
        // as some (atomic?) flags to allow event data and observer data to 
        // coordinate to resolve ordering issues.
        unsigned refCount;
    };
    using node_event_list = std::list<EventEntry>;
    
public:
    class EventCollectionIterator 
        : public boost::iterator_adaptor<
            EventCollectionIterator,
            node_event_list::const_iterator,
            NodeEvent,
            boost::use_default, // Category 
            const NodeEvent&
        >
    {
        using base_t = node_event_list::const_iterator;
        using adaptor_t = boost::iterator_adaptor<
                EventCollectionIterator,
                node_event_list::const_iterator,
                NodeEvent,
                boost::use_default,
                const NodeEvent&
            >;
        
    public:
        EventCollectionIterator() = default;
        EventCollectionIterator(const EventCollectionIterator&) = default;
        
    private:
        EventCollectionIterator(base_t base)
            : adaptor_t(base)
        {
        }
        
        const NodeEvent& dereference() { return (*base()).event; }
        
        friend class boost::iterator_core_access;
        friend class TreeObserverData;
    };
    
    using EventCollection = boost::iterator_range<EventCollectionIterator>;
    
    TreeObserverData(const TreeObserverData&) = delete;
    TreeObserverData(TreeObserverData&&) = delete;
    
    ~TreeObserverData()
    { 
        const QMutexLocker eventRefLocker(&_eventRefMutex);
        const QWriteLocker locker(&_lock);
        
        assert(
                _isDeleted 
                && (
                    _events.empty()
                )
            );
    }
    
    void startRecording()
    {
        const QWriteLocker locker(&_lock);
        
        assert(!_isRecording);
        
        _isRecording = true;
        _recording = NodeEvent();
    }
    
    const NodeEvent& finishRecording()
    {
        _eventRefMutex.lock();
        const QWriteLocker locker(&_lock);
        _eventRefMutex.unlock();
        
        assert(_isRecording);
        
        _isRecording = false;
        
        _recording.initData();
        auto id = _recording._data->id;
        
        assert(!_eventIndex.contains(id));
        
        auto i = _events.insert(
                _events.end(),
                { std::move(_recording), id, 0 }
            );
        _eventIndex[id] = i;
        
        return _events.back().event;
    }
    
    void releaseNodeEvent(quintptr id)
    {
        // Expected to be called from inside NodeEvent::Data::~Data where it is
        // wrapped in a lock of _eventRefMutex, i.e., no ref counts will change.
        
        assert(!_eventRefMutex.tryLock());
        
        // Ensure that no events are added
        const QWriteLocker locker(&_lock);
        
        assert(id && _eventIndex.contains(id));
        
        auto i = noDetach(_eventIndex)[id];
        if (i == _events.begin())
        {
            auto end = _events.end();
            for (; i != end; ++i)
            {
                if ((*i).refCount) 
                    break;
                else
                    _eventIndex.remove((*i).id);
            }
            _events.erase(_events.begin(), i);
        }
    }
    
    void onTreeDeleted()
    {
        const QWriteLocker locker(&_lock);
        
        assert(!_isDeleted);
        _isDeleted = true;
    }
    
    EventCollection events() const
    {
        return EventCollection(
                EventCollectionIterator(_events.begin()), 
                EventCollectionIterator(_events.end())
            );
    }
    
    EventCollectionIterator eventsEnd() const { return EventCollectionIterator(_events.end()); }
    
    EventCollectionIterator findEvent(const NodeEvent& event) const
    {
        if (!event._data || !_eventIndex.contains(event._data->id))
            return EventCollectionIterator(_events.end());
        else
            return EventCollectionIterator(_eventIndex[event._data->id]);
    }
    
    std::unique_ptr<QReadLocker> lockForRead() const
    {
        return std::make_unique<QReadLocker>(&_lock);
    }
    
    std::unique_ptr<QWriteLocker> lockForWrite()
    {
        return std::make_unique<QWriteLocker>(&_lock);
    }
    
    std::unique_ptr<QMutexLocker> lockEventRefCount() const
    {
        return std::make_unique<QMutexLocker>(&_eventRefMutex);
    }
    
private:
    TreeObserverData(std::any root)
        : _root(root)
    {
    }
    
    std::any _root;
    
    node_event_list _events;
    QHash<quintptr, node_event_list::iterator> _eventIndex;
    
    bool _isDeleted = false;
    bool _isRecording = false;
    
    NodeEvent _recording;
    
    mutable QMutex _eventRefMutex;
    mutable QReadWriteLock _lock = QReadWriteLock(QReadWriteLock::Recursive);
    
    template<typename> friend class TreeObserver;
    
#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
};

inline NodeEvent::Data::Data(const Data& other)
    : id(other.id),
    root(other.root),
    steps(other.steps),
    initialChildCounts(other.initialChildCounts),
    observerData(other.observerData),
    observerRefCount(other.observerRefCount)
{
    if (observerData)
    {
        const auto lock = observerData->lockEventRefCount();
        
        assert(observerRefCount);
        ++(*observerRefCount);
    }
}
        
inline NodeEvent::Data::~Data()
{
    if (observerData)
    {
        const auto lock = observerData->lockEventRefCount();
        
        assert(observerRefCount);
        if (!--(*observerRefCount))
            observerData->releaseNodeEvent(id);
    }
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
        return exposeEvent(_data->finishRecording());
    }
    
    NodeEvent lastNodeEvent() const
    {
        const auto lock = _data->lockForRead();
        
        if (_data->_events.empty())
        {
            return NodeEvent();
        }
        else
        {
            return exposeEvent(_data->_events.back().event);
        }
    }
    
    template<typename Range>
    auto insertNodes(
            handle_t parent, 
            child_handle_t before, 
            Range&& nodeValues
         )
    {
        assert(_data->_isRecording);
        
        return NodeEventTreeHelper<Tree>(
                _root, 
                _data->_recording
            ).template insertNodes<Range>(
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
        NodeEventTreeHelper<Tree>(
                _root, 
                _data->_recording
            ).removeNodes(
                parent, 
                startIndex, 
                count
            );
    }
    
    template<typename Range>
    void removeNodes(Range&& range)
    {
        assert(_data->_isRecording);
        NodeEventTreeHelper<Tree>(
                _root, 
                _data->_recording
            ).removeNodes(
                std::forward<Range>(range)
            );
    }
    
private:
    // Gives an altered copy of `event` that is suitable for use outside of the 
    // observer.
    NodeEvent exposeEvent(const NodeEvent& event) const
    {
        const auto locker = _data->lockEventRefCount();
        auto id = event._data->id;
        
        assert(!event._data->observerData && _data->_eventIndex.contains(id));
        unsigned& refCount = (*_data->_eventIndex[id]).refCount;
        
        NodeEvent result(event);
        result._data.detach(); 
        // This will not automatically increment the ref count because result's 
        // observerData pointer is not set.
        
        ++refCount;
        
        result._data->observerRefCount = &refCount;
        result._data->observerData = _data;
        
        return result;
    }
    
    ::Addle::aux_datatree::node_handle_t<Tree> _root = {};
    QSharedPointer<TreeObserverData> _data;
    
#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
};

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
    class UnexpectedEventException : public std::exception
    {
    public:
        UnexpectedEventException() = default;
        UnexpectedEventException(const UnexpectedEventException&) = default;
        virtual ~UnexpectedEventException() = default;
    };
    
    using NodeOperation = NodeEvent::NodeOperation;
    
    NostalgicNodeEventHelper() = default;
    NostalgicNodeEventHelper(NostalgicNodeEventHelper&&) = default;
    NostalgicNodeEventHelper& operator=(NostalgicNodeEventHelper&&) = default;
    
    NostalgicNodeEventHelper(const NostalgicNodeEventHelper& other)
        : _events(other._events),
        _progress(other._progress),
        _step(other._step),
        _stepEnd(other._stepEnd),
        _cChunk(other._cChunk),
        _fChunk(other._fChunk),
        _rCChunk(other._rCChunk),
        _rFChunk(other._rFChunk),
        _maxCacheSize(other._maxCacheSize),
        _observerData(other._observerData),
        _eventPosition(other._eventPosition)
    {
    }
    
    NostalgicNodeEventHelper& operator=(const NostalgicNodeEventHelper& other)
    {
        _events = other._events;
        _progress = other._progress;
        _step = other._step;
        _stepEnd = other._stepEnd;
        _cChunk = other._cChunk;
        _fChunk = other._fChunk;
        _rCChunk = other._rCChunk;
        _rFChunk = other._rFChunk;
        _maxCacheSize = other._maxCacheSize;
        _observerData = other._observerData;
        _eventPosition = other._eventPosition;
        
        invalidateCaches();
        
        return *this;
    }
    
    NostalgicNodeEventHelper(const NodeEvent& event)
    {
        if (!event._data || event._data->steps.empty()) return;
        
        _events = { event };
        
        _step       = _events.front()._data->steps.begin();
        _stepEnd    = _events.front()._data->steps.end();
        
        initChunkIterators();
        
        _observerData = _events.front()._data->observerData;
        if (_observerData)
        {
            auto lock = _observerData->lockForRead();
            _eventPosition = _observerData->findEvent(_events.front());
        }
    }
    
    NostalgicNodeEventHelper(NodeEvent&& event)
    {
        if (!event._data || event._data->steps.empty()) return;
        
        _events = { std::move(event) };
        
        _step       = _events.front()._data->steps.begin();
        _stepEnd    = _events.front()._data->steps.end();
        
        initChunkIterators();
        
        _observerData = _events.front()._data->observerData;
        if (_observerData)
        {
            auto lock = _observerData->lockForRead();
            _eventPosition = _observerData->findEvent(_events.front());
        }
    }
    
    NostalgicNodeEventHelper& operator=(const NodeEvent& event)
    {
        if (event._data && !event._data->steps.empty()) 
        {
            _events = { event };
            
            _progress = 0;
            _step       = _events.front()._data->steps.begin();
            _stepEnd    = _events.front()._data->steps.end();
            
            _observerData = _events.front()._data->observerData;
            if (_observerData)
            {
                auto lock = _observerData->lockForRead();
                _eventPosition = _observerData->findEvent(_events.front());
            }
        }
        else
        {
            clear();
        }

        initChunkIterators();        
        invalidateCaches();
        
        return *this;
    }
    
    NostalgicNodeEventHelper& operator=(NodeEvent&& event)
    {
        if (event._data && !event._data->steps.empty())
        {
            _events = { std::move(event) };
            
            _progress = 0;
            _step       = _events.front()._data->steps.begin();
            _stepEnd    = _events.front()._data->steps.end();
            
            _observerData = _events.front()._data->observerData;
            if (_observerData)
            {
                auto lock = _observerData->lockForRead();
                _eventPosition = _observerData->findEvent(_events.front());
            }
        }
        else
        {
            clear();
        }

        initChunkIterators();        
        invalidateCaches();
        
        return *this;
    }
    
    template<typename Range,
        typename std::enable_if_t<
            std::is_convertible_v<
                typename boost::range_reference<boost::remove_cv_ref_t<Range>>::type,
                NodeEvent
            >,
        void*> = nullptr>
    NostalgicNodeEventHelper& operator=(Range&& events)
    {
        if (boost::empty(events))
        {
            clear();
        }
        else
        {
            _events = std::deque<NodeEvent>(
                    boost::begin(events), 
                    boost::end(events)
                );
            
            _progress = 0;
            
            while (true)
            {
                _step       = _events.front()._data->steps.begin();
                _stepEnd    = _events.front()._data->steps.end();
                
                if (Q_LIKELY(_step != _stepEnd) || _events.empty()) break;
                
                _events.pop_front();
            }
        
            _observerData = _events.front()._data->observerData;
            if (_observerData)
            {
                auto lock = _observerData->lockForRead();
                _eventPosition = _observerData->findEvent(_events.front());
            }
        }

        initChunkIterators();        
        invalidateCaches();
        
        return *this;
    }
    
    NostalgicNodeEventHelper& operator=(const std::initializer_list<NodeEvent>& events)
    {
        return this->operator=<const std::initializer_list<NodeEvent>&>(events);
    }
    
    void clear() 
    {
        _events.clear();
        _progress = 0;
        _step = {};
        _stepEnd = {};
        
        initChunkIterators();
        invalidateCaches();
        
        _observerData = nullptr;
        _eventPosition = {};
    }
    
    NodeEvent event() const
    {
        return (!_events.empty()) ? _events.front() : NodeEvent();
    }
    
    template<typename Range,
        typename std::enable_if_t<
            std::is_convertible_v<
                typename boost::range_reference<boost::remove_cv_ref_t<Range>>::type,
                NodeEvent
            >,
        void*> = nullptr>
    void pushEvents(Range&& events)
    {
        if (boost::empty(events))
            return;
        
        bool wasEmpty = _events.empty();
        
        _events.insert(
                _events.end(), 
                boost::begin(events), 
                boost::end(events)
            );
        
        if (wasEmpty)
        {
            _progress = 0;
            
            _step       = _events.front()._data->steps.begin();
            _stepEnd    = _events.front()._data->steps.end();
            
            if (!_observerData && _events.front()._data->observerData)
                _observerData = _events.front()._data->observerData;
            
            if (_observerData)
            {
                auto lock = _observerData->lockForRead();
                _eventPosition = _observerData->findEvent(_events.front());
            }
            
            initChunkIterators();
            invalidateCaches();
        }
    }
    
    void pushEvents(const std::initializer_list<NodeEvent>& events)
    {
        pushEvents<const std::initializer_list<NodeEvent>&>(events);
    }
    
    void pushEvent(const NodeEvent& event)
    {
        if (!event._data) return;
        pushEvents(make_inline_ref_range(event));
    }
    
    void pushEvent(NodeEvent&& event)
    {
        if (!event._data) return;
        pushEvents(make_inline_ref_range(std::move(event)));
    }
    
    bool eventIsCompatible(const NodeEvent& event) const
    {
        return (!_observerData && _events.empty())
            || (event._data->observerData == _observerData);
    }
    
    bool isUpToDate() const
    {
        if (!_observerData) return !hasNext();
        
        auto locker = _observerData->lockForRead();
        return _eventPosition == _observerData->eventsEnd();
    }
        
    NodeOperation operation() const 
    { 
        return _step != _stepEnd ?
            (*_step).operation() :
            (NodeOperation)(NULL); 
    }
    
    NodeChunk operatingChunk() const
    {
        if (!_events.empty())
        {
            assert(_step != _stepEnd);
            switch ((*_step).operation())
            {
                case NodeOperation::Add:
                    if (Q_LIKELY(_fChunk != (*_cChunk).second.end()))
                        return *_fChunk;
                    break;
                    
                case NodeOperation::Remove:
                    if (Q_LIKELY( _rFChunk != std::make_reverse_iterator(
                            (*_rCChunk).second.begin()
                        ) ))
                        return *_rFChunk;
                    break;
                    
                default:
                    Q_UNREACHABLE();
            }
        }
        
        return NodeChunk { NodeAddress(), 0 };
    }
    
    bool hasNext() const { return !_events.empty(); }
    
    bool next()
    {
        if (_events.empty()) return false;
        
        assert(_step != _stepEnd);
        
        switch ((*_step).operation())
        {
            case NodeOperation::Add:
                ++_fChunk;
                
                if (_fChunk != (*_cChunk).second.end())
                    break;
                
                ++_cChunk;
                
                if (_cChunk != (*_step).primaryChunks().end())
                {
                    _fChunk = (*_cChunk).second.begin();
                    break;
                }
                
                ++_step;
                initChunkIterators();
                break;
                
            case NodeOperation::Remove:
                ++_rFChunk;
                
                if (_rFChunk != std::make_reverse_iterator(
                        (*_rCChunk).second.begin()
                    ))
                    break;
                
                ++_rCChunk;
                
                if (_rCChunk != std::make_reverse_iterator(
                        (*_step).primaryChunks().begin()
                    ))
                {
                    _rFChunk = std::make_reverse_iterator((*_rCChunk).second.end());
                    break;
                }
                
                ++_step;
                initChunkIterators();
                break;
                
            default:
                Q_UNREACHABLE();
        }
        
        ++_progress;
        
        if (_step != _stepEnd)
            return true;
        
        invalidateCaches();
        
        if (_observerData)
        {
            auto lock = _observerData->lockForRead();
            ++_eventPosition;
        }
        
        _events.pop_front();
                
        while(!_events.empty())
        {
            _progress = 0;
            if(_events.front()._data)
            {
                if (Q_UNLIKELY(_events.front()._data->observerData != _observerData))
                {
                    // TODO: i18n
                    qWarning("Unexpected node event observer data");
                    clear();
                    throw UnexpectedEventException();
                }
                
                if (_observerData)
                {
                    auto lock = _observerData->lockForRead();
                    
                    if (Q_UNLIKELY(
                            _eventPosition == _observerData->eventsEnd()
                            || _eventPosition != _observerData->findEvent(_events.front())
                        ))
                    {
                        // TODO: i18n
                        qWarning("Unexpected node event id");
                        clear();
                        throw UnexpectedEventException();
                    }
                }
            
                _step = _events.front()._data->steps.begin();
                _stepEnd = _events.front()._data->steps.end();
                
                initChunkIterators();
                
                if (_step != _stepEnd) 
                    return true;
                    
                if (_observerData)
                {
                    auto lock = _observerData->lockForRead();
                    ++_eventPosition;
                }
            }
            
            _events.pop_front();
        }
        
        _step = {};
        _stepEnd = {};
        
        initChunkIterators();
        
        return false;
    }
    
    // TODO: maybe use the cache to short-circuit mapping/size calculations?
    
    NodeAddress mapForward(NodeAddress from) const
    {
        if (_events.empty()) 
            return from;
        
        auto cacheIndex = qMakePair(from, _progress);
        
        if (_forwardMapCache.contains(cacheIndex))
        {
            auto i = noDetach(_forwardMapCache)[cacheIndex];
            
            // Prioritize this entry against being pruned in the future.
            _mapCacheQueue.splice(_mapCacheQueue.begin(), _mapCacheQueue, i);
            
            return (*i).toAddress;
        }
        
        NodeAddress result = _events.front().mapForward(from, _progress);
        
        {
            auto i = _mapCacheQueue.insert(
                    _mapCacheQueue.begin(), 
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
        if (_events.empty())
            return from;
        
        auto cacheIndex = qMakePair(from, _progress);
        
        if (_backwardMapCache.contains(cacheIndex))
        {
            auto i = noDetach(_backwardMapCache)[cacheIndex];
            
            // Prioritize this entry against being pruned in the future.
            _mapCacheQueue.splice(_mapCacheQueue.begin(), _mapCacheQueue, i);
            
            return (*i).toAddress;
        }
        
        NodeAddress result = _events.front().mapBackward(from, _progress);
        
        {
            auto i = _mapCacheQueue.insert(
                    _mapCacheQueue.begin(), 
                    { result, from, _progress }
                );
    
            _backwardMapCache[cacheIndex] = i;
            _forwardMapCache[qMakePair(result, _progress)] = i;
            
            pruneCaches();
        }
        
        return result;
    }
    
    std::size_t childCount(NodeAddress parent) const
    {
        if (_events.empty())
            return 0;
        
        auto cacheIndex = qMakePair(parent, _progress);
        
        if (_childCountCache.contains(cacheIndex))
        {
            auto i = noDetach(_childCountCache)[cacheIndex];
            
            _childCountCacheQueue.splice(_childCountCacheQueue.begin(), _childCountCacheQueue, i);
            
            return (*i).count;
        }
        
        std::size_t result = _events.front().childCount(parent, _progress);
        
        {
            auto i = _childCountCacheQueue.insert(
                    _childCountCacheQueue.begin(), 
                    { result, parent, _progress }
                );
    
            _childCountCache[cacheIndex] = i;
            
            pruneCaches();
        }
        
        return result;
    }
    
    bool isCoordinated() const { return _observerData; }
    
    std::size_t maxCacheSize() const { return _maxCacheSize; }
    void setMaxCacheSize(std::size_t size)
    {
        _maxCacheSize = size;
        
        if (size == 0)
            invalidateCaches();
        else
            pruneCaches();
    }
    
private:
    using step_iterator = QList<NodeEvent::Step>::const_iterator;
    
    using coarse_chunk_iterator = NodeEvent::chunk_map_t::const_iterator;
    using fine_chunk_iterator = NodeEvent::chunk_list_t::const_iterator;
    
    using r_coarse_chunk_iterator = std::reverse_iterator<coarse_chunk_iterator>;
    using r_fine_chunk_iterator = std::reverse_iterator<fine_chunk_iterator>;
    
    void initChunkIterators()
    {
        if (_events.empty() || _step == _stepEnd)
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
                _fChunk     = (*_cChunk).second.begin();
                
                break;
                
            case NodeOperation::Remove:
                _cChunk     = {};
                _fChunk     = {};
                
                _rCChunk    = std::make_reverse_iterator(
                                    (*_step).primaryChunks().end()
                                );
                _rFChunk    = std::make_reverse_iterator((*_rCChunk).second.end());
                
                break;
                
            default:
                Q_UNREACHABLE();
        }
    }
    
    void invalidateCaches()
    {
        _mapCacheQueue.clear();
        _forwardMapCache.clear();
        _backwardMapCache.clear();
        
        _childCountCacheQueue.clear();
        _childCountCache.clear();
    }
    
    void pruneCaches() const
    {
        while (_mapCacheQueue.size() > _maxCacheSize)
        {
            const auto& entry = _mapCacheQueue.back();
            
            _forwardMapCache.remove(qMakePair(entry.fromAddress, entry.progress));
            _backwardMapCache.remove(qMakePair(entry.toAddress, entry.progress));
            
            _mapCacheQueue.pop_back();
        }
        
        while (_childCountCacheQueue.size() > _maxCacheSize)
        {
            const auto& entry = _childCountCacheQueue.back();
            
            _childCountCache.remove(qMakePair(entry.address, entry.progress));
            _childCountCacheQueue.pop_back();
        }
    }

    std::deque<NodeEvent> _events;
    std::size_t _progress = 0;
    
    step_iterator _step = {};
    step_iterator _stepEnd = {};
    
    coarse_chunk_iterator   _cChunk = {};
    fine_chunk_iterator     _fChunk = {};
    r_coarse_chunk_iterator _rCChunk = {};
    r_fine_chunk_iterator   _rFChunk = {};
    
    // The default cache size is very generous with the assumption that the 
    // helper is essentially a temporary object. 
    std::size_t _maxCacheSize = 1024;
    
    QSharedPointer<TreeObserverData> _observerData;
    TreeObserverData::EventCollectionIterator _eventPosition = {};
            
    struct CacheQueueEntry
    {
        NodeAddress fromAddress;
        NodeAddress toAddress;
        std::size_t progress;
    };
    using map_cache_queue_t = std::list<CacheQueueEntry>;
    
    // A collection of cached node address mappings, in order of recent access.
    // If the size exceeds _maxCacheSize, then the oldest entries are deleted
    // first.
    mutable map_cache_queue_t _mapCacheQueue;
    
    // Fast lookup indices for node address mappings. 
    // TODO std::unordered_map is probably better (we don't need or want
    // implicit sharing)
    mutable QHash<QPair<NodeAddress, std::size_t>, 
            map_cache_queue_t::const_iterator> _forwardMapCache;
    mutable QHash<QPair<NodeAddress, std::size_t>, 
            map_cache_queue_t::const_iterator> _backwardMapCache;
    
    struct ChildCountQueueEntry
    {
        std::size_t count;
        NodeAddress address;
        std::size_t progress;
    };
    using child_count_cache_queue_t = std::list<ChildCountQueueEntry>;
        
    mutable child_count_cache_queue_t _childCountCacheQueue;
    
    mutable QHash<
            QPair<NodeAddress, std::size_t>,
            child_count_cache_queue_t::const_iterator
        > _childCountCache;
        
#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
};

}

template<class Tree>
using DataTreeObserver = aux_datatree::TreeObserver<Tree>;

using DataTreeNodeEvent = aux_datatree::NodeEvent;
using DataTreeNodeEventBuilder = aux_datatree::NodeEventBuilder;

}

Q_DECLARE_METATYPE(Addle::DataTreeNodeEvent)
