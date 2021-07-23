#include <functional>

#include "./observer.hpp"

using namespace Addle::aux_datatree;

template<bool additive>
std::optional<NodeAddress> Addle::aux_datatree::mapSingle_impl(
        const NodeAddress& from,
        const NodeChunk& rel)
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
                        result.append(index - rel.length);
                    else
                        return {};
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

template std::optional<NodeAddress> Addle::aux_datatree::mapSingle_impl<true>(const NodeAddress&, const NodeChunk&);
template std::optional<NodeAddress> Addle::aux_datatree::mapSingle_impl<false>(const NodeAddress&, const NodeChunk&);

template<bool additive>
std::pair<NodeChunk, NodeChunk> Addle::aux_datatree::mapSingleChunk_impl(
        const NodeChunk& from,
        const NodeChunk& rel)
{
    if (from.address.isRoot()
        || rel.address.isRoot()
        || !rel.length
    ) 
    {
        return { from, {} };
    }
    
    if (!from.length)
    {
        auto mappedAddress = mapSingle_impl<additive>(from.address, rel);
        if (mappedAddress)
            return { { *mappedAddress, 0 }, {} };
        else
            return {};
    }
    
    if (from.address.commonAncestorIndex(rel.address) 
            == from.address.parentIndex())
    {
        std::size_t fromIndex = from.address.lastIndex();
        std::size_t relIndex = rel.address.lastIndex();
        
        if (fromIndex + from.length <= relIndex)
            return { from, {} };
                    
        if (relIndex <= fromIndex)
        {
            if constexpr (additive)
            {
                return {{ NodeAddressBuilder(from.address).setLastIndex(fromIndex + rel.length), from.length }, {}};
            }
            else // !additive
            {
                if (relIndex + rel.length <= fromIndex)
                    return {{ NodeAddressBuilder(from.address).setLastIndex(fromIndex - rel.length), from.length }, {}};
                
                std::size_t overhang = relIndex + rel.length - fromIndex;
                
                if (overhang >= from.length)
                    return {};
                else
                    return {{ rel.address, from.length - overhang }, {}};
            }
        }
        else // fromIndex < relIndex
        {
            if constexpr (additive)
            {
                 return {
                    { from.address, relIndex - fromIndex },
                    { NodeAddressBuilder(from.address)
                            .setLastIndex(relIndex + rel.length), 
                    from.length - (relIndex - fromIndex) }
                };
            }
            else // !additive
            {
                std::size_t overhang = fromIndex + from.length - relIndex;
                    
                if (overhang <= rel.length)
                    return {{ from.address, relIndex - fromIndex }, {}};
                else
                    return {{ from.address, from.length - overhang }, {}};
            }
        }
    }
    else // from.address and rel.address are not siblings
    {
        if constexpr (!additive)
        {
            if (rel.coversAddress(from.address))
                return {};
        }
     
        auto mappedAddress = mapSingle_impl<additive>(from.address, rel);
        assert(mappedAddress);
        
        return { { *mappedAddress, from.length }, {} };
    }
}

template std::pair<NodeChunk, NodeChunk> Addle::aux_datatree::mapSingleChunk_impl<true>(const NodeChunk&, const NodeChunk&);
template std::pair<NodeChunk, NodeChunk> Addle::aux_datatree::mapSingleChunk_impl<false>(const NodeChunk&, const NodeChunk&);

QList<NodeChunk> NodeEvent::chunksAdded() const
{
    QList<NodeChunk> result;
    
    for (const StagingHandle& h : *this) 
    {
        if (h.operation() == Add)
            result += h.mapCurrentChunkForward(h.operatingChunk());
    }
    
    cleanupChunkSet(result, false);
    return result;
}

QList<NodeChunk> NodeEvent::chunksRemoved() const
{
    QList<NodeChunk> result;
    
    for (const StagingHandle& h : *this) 
    {
        if (h.operation() == Remove)
            result += h.mapCurrentChunkBackward(h.operatingChunk());
    }
    
    cleanupChunkSet(result);
    return result;
}


QList<NodeAddress> NodeEvent::affectedParents() const
{
    QList<NodeAddress> result;
    
    for (const StagingHandle& h : *this) 
    {
        auto mapped = h.mapCurrentBackward(h.operatingChunk().address.parent());
        if (!mapped) continue;
        
        auto lowerBound = std::lower_bound(
                result.begin(),
                result.end(),
                *mapped
            );
        
        if (lowerBound == result.end() || (*lowerBound) != *mapped)
            result.insert(lowerBound, std::move(*mapped));
    }
    
    return result;
}

QList<NodeAddress> NodeEvent::affectedParentsFromEnd() const
{
    QList<NodeAddress> result;
    
    for (const StagingHandle& h : *this) 
    {
        auto mapped = h.mapCurrentForward(h.operatingChunk().address.parent());
        if (!mapped) continue;
        
        auto lowerBound = std::lower_bound(
                result.begin(),
                result.end(),
                *mapped
            );
        
        if (lowerBound == result.end() || (*lowerBound) != *mapped)
            result.insert(lowerBound, std::move(*mapped));
    }
    
    return result;
}

bool NodeEvent::isAffectedParent(NodeAddress parent) const
{
    if (!_data) return false;

    for (const Step& step : _data->steps)
    {
        switch(step.operation())
        {
            case Add:
            case Remove:
            {
                auto find = step.primaryChunks().find(parent);
                if (find != step.primaryChunks().end())
                    return true;
            }
                
            default:
                Q_UNREACHABLE();
        }
        auto mapped = step.mapForward(parent);
        if (!mapped) return false;
        parent = std::move(*mapped);
    }
    
    return false;
}

bool NodeEvent::isAffectedParentFromEnd(NodeAddress parent) const
{
    using namespace boost::adaptors;
    
    if (!_data) return false;

    for (const Step& step : _data->steps | reversed)
    {
        switch(step.operation())
        {
            case Add:
            case Remove:
            {
                auto find = step.primaryChunks().find(parent);
                if (find != step.primaryChunks().end())
                    return true;
            }
                
            default:
                Q_UNREACHABLE();
        }
        auto mapped = step.mapBackward(parent);
        if (!mapped) return false;
        parent = std::move(*mapped);
    }
    
    return false;
}

void NodeEvent::addChunk(NodeChunk chunk)
{
    initData();
    
    if (!_data->steps.empty() && _data->steps.last().operation() == Add)
    {
        Step& step = _data->steps.last();
        
        _data->totalChunkCount += step.rebase(chunk);
        _data->totalChunkCount += step.addPrimaryChunk(std::move(chunk));
    }
    else
    {
        _data->steps.append(Step(Add));
        _data->totalChunkCount += 
            _data->steps.last().addPrimaryChunk(std::move(chunk));
    }
}

void NodeEvent::addChunks(QList<NodeChunk> chunks)
{
    if (chunks.empty()) return;
    
    std::sort(chunks.begin(), chunks.end());
    
    for (NodeChunk& chunk : chunks)
        addChunk(std::move(chunk));
}

void NodeEvent::removeChunk(NodeChunk chunk)
{
    initData();
    
    if (!_data->steps.empty() && _data->steps.last().operation() == Remove)
    {
        Step& step = _data->steps.last();
        
        QVarLengthArray<NodeChunk, 64> mapped;
        step.mapChunksBackward(make_inline_range(chunk), mapped);
        
        for (NodeChunk& mappedChunk : mapped)
            _data->totalChunkCount += step.addPrimaryChunk(std::move(mappedChunk));
    }
    else
    {
        _data->steps.append(Step(Remove));
        _data->totalChunkCount += 
            _data->steps.last().addPrimaryChunk(std::move(chunk));
    }
}

void NodeEvent::removeChunks(QList<NodeChunk> chunks)
{
    if (chunks.empty()) return;
    
    initData();
        
    if (!_data->steps.empty() && _data->steps.last().operation() == Remove)
    {
        Step& step = _data->steps.last();
        
        QVarLengthArray<NodeChunk, 64> buffer;
        step.mapChunksBackward(chunks, buffer);
        
        std::sort(buffer.rbegin(), buffer.rend());
        
        for (NodeChunk& chunk : buffer)
        {
            _data->totalChunkCount += step.addPrimaryChunk(std::move(chunk));
        }
    }
    else
    {
        _data->steps.append(Step(Remove));
        
        std::sort(chunks.rbegin(), chunks.rend());
        
        for (NodeChunk& chunk : chunks)
        {
            _data->totalChunkCount += 
                _data->steps.last().addPrimaryChunk(std::move(chunk));
        }
    }
}

std::optional<NodeAddress> NodeEvent::StagingHandle::mapPastForward(NodeAddress from) const
{
    assert(_step != _stepEnd);
    
    if (_step != _stepBegin)
    {
        auto step = _stepBegin;
        do
        {
            auto mapped = (*step).mapForward(from);
            if (!mapped) return {};
            
            from = std::move(*mapped);
            ++step;
        } while (step != _step);
    }
    
    return (*_step).mapForward(from, 0, _chunkIndex);
}

std::optional<NodeAddress> NodeEvent::StagingHandle::mapCurrentBackward(NodeAddress from) const
{
    assert(_step != _stepEnd);
    
    {
        auto mapped = (*_step).mapBackward(from, 0, _chunkIndex);
        if (!mapped) return {};
        
        from = std::move(*mapped);
    }
    
    auto step = std::make_reverse_iterator(_step);
    for (; step.base() != _stepBegin; ++step)
    {
        auto mapped = (*step).mapBackward(from);
        if (!mapped) return {};
        
        from = std::move(*mapped);
    } 
    
    return from;
}
        
std::optional<NodeAddress> NodeEvent::StagingHandle::mapCurrentForward(NodeAddress from) const
{
    assert(_step != _stepEnd);
    
    {
        auto mapped = (*_step).mapForward(from, _chunkIndex, SIZE_MAX);
        if (!mapped) return {};
        
        from = std::move(*mapped);
    }
    
    auto step = _step;
    ++step;
    
    for (; step != _stepEnd; ++step)
    {
        auto mapped = (*step).mapForward(from);
        if (!mapped) return {};
        
        from = std::move(*mapped);
    }
    
    return from;
}

std::optional<NodeAddress> NodeEvent::StagingHandle::mapFutureBackward(NodeAddress from) const
{
    assert(_step != _stepEnd);
    
    auto step = std::make_reverse_iterator(_stepEnd);
    ++step;
    
    for(; step.base() != _step; ++step)
    {
        auto mapped = (*step).mapBackward(from);
        if (!mapped) return {};
        
        from = std::move(*mapped);
    }
    
    return (*_step).mapBackward(from, _chunkIndex, SIZE_MAX);
}

QList<NodeChunk> NodeEvent::StagingHandle::mapPastChunksForward_impl(QList<NodeChunk> from) const
{
    assert(_step != _stepEnd);
    
    QList<NodeChunk> result;
    
    if (_step != _stepBegin)
    {
        auto step = _stepBegin;
        do
        {
            (*step).mapChunksForward(from, result);
            if (result.isEmpty()) return {};
                        
            result.swap(from);
            result.clear();
            
            ++step;
        } while (step != _step); 
    }
    
    (*_step).mapChunksForward(from, result, 0, _chunkIndex);
    return result;
}

QList<NodeChunk> NodeEvent::StagingHandle::mapCurrentChunksBackward_impl(QList<NodeChunk> from) const
{
    assert(_step != _stepEnd);
    
    QList<NodeChunk> result;

    (*_step).mapChunksBackward(from, result, 0, _chunkIndex);
    if (result.isEmpty()) return {};

    auto step = std::make_reverse_iterator(_step);
    for (; step.base() != _stepBegin; ++step)
    {
        result.swap(from);
        result.clear();
        
        (*step).mapChunksBackward(from, result);
        
        if (result.isEmpty()) return {};
    } 
    
    return result;
}

QList<NodeChunk> NodeEvent::StagingHandle::mapCurrentChunksForward_impl(QList<NodeChunk> from) const
{
    assert(_step != _stepEnd);
    
    QList<NodeChunk> result;
    
    (*_step).mapChunksForward(from, result, _chunkIndex + 1, SIZE_MAX);
    if (result.isEmpty()) return {};

    auto step = _step;
    ++step;
    
    for (; step != _stepEnd; ++step)
    {
        result.swap(from);
        result.clear();
        
        (*step).mapChunksForward(from, result);
        if (result.isEmpty()) return {};
    }
    
    return result;
}

QList<NodeChunk> NodeEvent::StagingHandle::mapFutureChunksBackward_impl(QList<NodeChunk> from) const
{
    assert(_step != _stepEnd);
    
    QList<NodeChunk> result;
    
    auto step = std::make_reverse_iterator(_stepEnd);
    ++step;
    
    if (step.base() != _step)
    {
        while (true)
        {
            (*step).mapChunksBackward(from, result);
            if (result.isEmpty()) return {};
            
            ++step;
            if (step.base() == _step) break;
            
            result.swap(from);
            result.clear();
        }
    }
    
    (*_step).mapChunksBackward(from, result, _chunkIndex, SIZE_MAX);
    return result;
}

// std::optional<NodeAddress> NodeEvent::mapBackward_impl(NodeAddress from, std::size_t progress) const
// {
//     using namespace boost::adaptors;
//     
//     if (!_data || from.isRoot() || !progress) 
//         return from;
//     
//     auto i = _data->steps.cbegin();
//     auto end = _data->steps.cend();
//     
//     if (Q_UNLIKELY(i == end)) return from; // impossible?
//     
//     if (progress >= _data->totalChunkCount)
//     {
//         progress = _data->totalChunkCount;
//         i = end;
//     }
//     else
//     {
//         while( progress > (*i).chunkCount() )
//         {
//             progress -= (*i).chunkCount();
//             ++i;
//             
//             assert(i != end);
//         }
//         ++i;
//     }
//     
//     NodeAddress result(from);
//     
//     auto j =    std::make_reverse_iterator(i);
//     auto rend = _data->steps.rend();
//     for(; j != rend; ++j)
//     {
//         auto next = (*j).mapBackward(result, progress);
//         
//         if (!next) return {};
//         
//         result = std::move(*next);
//         progress -= (*j).chunkCount();
//     }
//     
//     return result;
// }

// std::ptrdiff_t NodeEvent::Step::childCountOffset(
//         NodeAddress parent, 
//         std::size_t progress) const
// {
//     using namespace boost::adaptors;
//     
//     if (_primaryChunks.empty() || !progress)
//         return 0;
//     
//     progress = std::min(progress, _chunkCount);
//     
//     switch(_operation)
//     {
//         case NodeOperation::Add:
//         {
//             if (progress < _chunkCount)
//             {                        
//                 auto lowerBound = _primaryChunks.lower_bound(parent);
//                 auto lowerIter = _primaryChunks.begin();
//                 
//                 for (; lowerIter != lowerBound; ++lowerIter)
//                 {
//                     if (progress > (*lowerIter).second.size())
//                         progress -= (*lowerIter).second.size();
//                     else
//                         return 0;
//                 }
//             }
//             
//             std::ptrdiff_t result = 0;
//             
//             auto childrenFind = _primaryChunks.find(parent);
//             if (childrenFind != _primaryChunks.end())
//             {
//                 auto childIter = (*childrenFind).second.begin();
//                 auto childrenEnd = (*childrenFind).second.end();
//                 
//                 for (; childIter != childrenEnd; ++childIter)
//                 {
//                     if (!progress) break;
//                     
//                     result += (*childIter).length;
//                     --progress;
//                 }
//             }
//             
//             return result;
//         }
//             
//         case NodeOperation::Remove:
//         {
//             if (progress < _chunkCount)
//             {
//                 auto bound = std::make_reverse_iterator(
//                         _primaryChunks.upper_bound(parent)
//                     );
//                 auto upperIter = std::make_reverse_iterator(
//                         _primaryChunks.end()
//                     );
//                 
//                 for (; upperIter != bound; ++upperIter)
//                 {
//                     if (progress > (*upperIter).second.size())
//                         progress -= (*upperIter).second.size();
//                     else
//                         return 0;
//                 }
//             }
//             
//             std::ptrdiff_t result = 0;
//             
//             auto childrenFind = _primaryChunks.find(parent);
//             if (childrenFind != _primaryChunks.end())
//             {
//                 auto childIter = (*childrenFind).second.rbegin();
//                 auto childrenEnd = (*childrenFind).second.rend();
//                 
//                 for (; childIter != childrenEnd; ++childIter)
//                 {
//                     if (!progress) break;
//                     
//                     result -= (*childIter).length;
//                     --progress;
//                 }
//             }
//             
//             return result;
//         }
//         
// //                 case NodeOperation::Move: // TODO
//             
//         default:
//             Q_UNREACHABLE();
//     }
// }

template<bool Additive, bool ChunkIndicesAscending>
NodeEvent::Step::PrimaryChunkMapper<Additive, ChunkIndicesAscending>::PrimaryChunkMapper(
        const Step& step_,
        NodeChunk from_,
        std::size_t minChunkIndex_,
        std::size_t maxChunkIndex_
    )
        : step(step_), 
        minChunkIndex(minChunkIndex_),
        maxChunkIndex(std::min(maxChunkIndex_, step._chunkCount))
{
    assert(minChunkIndex <= maxChunkIndex);
    auto parentAddress = from_.address.parent();
    
    // this call will update chunkIndex 
    mappedParentAddress = mapAddress(parentAddress); 
    assert(chunkIndex <= maxChunkIndex);
    
    if (mappedParentAddress)
    {
        remainderChunk = {
            NodeAddressBuilder(*mappedParentAddress) << from_.address.lastIndex(),
            from_.length
        };
    }
    
    cChunk = step._primaryChunks.find(parentAddress);
    
    if (cChunk == step._primaryChunks.end()) return;
        // next call to yieldMapChunk() will skip the main loop and return one
        // value because fChunk == fChunkEnd
    
    if constexpr (ChunkIndicesAscending)
    {   
        fChunk = (*cChunk).second.cbegin();
        if (chunkIndex < minChunkIndex)
        {
            fChunk += minChunkIndex - chunkIndex;
            chunkIndex = minChunkIndex;
        }
        
        if (maxChunkIndex - chunkIndex < (*cChunk).second.size())
            fChunkEnd = fChunk + (maxChunkIndex - chunkIndex);
        else
            fChunkEnd = (*cChunk).second.cend();
    }
    else 
    {
        if (maxChunkIndex - chunkIndex <= (*cChunk).second.size())
        {
            fChunk = (*cChunk).second.cend() - (maxChunkIndex - chunkIndex);
            fChunkEnd = (*cChunk).second.cend() - minChunkIndex;
            
            chunkIndex = maxChunkIndex;
        }
    }
}

template NodeEvent::Step::PrimaryChunkMapper<true, true>::PrimaryChunkMapper(const Step&, NodeChunk, std::size_t, std::size_t);
template NodeEvent::Step::PrimaryChunkMapper<true, false>::PrimaryChunkMapper(const Step&, NodeChunk, std::size_t, std::size_t);
template NodeEvent::Step::PrimaryChunkMapper<false, true>::PrimaryChunkMapper(const Step&, NodeChunk, std::size_t, std::size_t);
template NodeEvent::Step::PrimaryChunkMapper<false, false>::PrimaryChunkMapper(const Step&, NodeChunk, std::size_t, std::size_t);

template<bool Additive, bool ChunkIndicesAscending>
std::optional<NodeAddress> 
NodeEvent::Step::PrimaryChunkMapper<Additive, ChunkIndicesAscending>::mapAddress(
        NodeAddress from
    )
{
    using namespace boost::adaptors;
    
    if (Q_UNLIKELY(!maxChunkIndex)) return from;
    // other fixed-point cases should be handled upstream
    
    NodeAddressBuilder result;
    chunkIndex = 0;
    
    auto lowerBound = step._primaryChunks.lower_bound(from);
    for (auto i = step._primaryChunks.begin(); i != lowerBound; ++i)
    {
        const NodeAddress& key = (*i).first;
        const chunk_list_t& chunks = (*i).second;
        
        if constexpr (ChunkIndicesAscending)
        {
            if (chunkIndex + chunks.size() < minChunkIndex)
            {
                chunkIndex += chunks.size();
                continue;
            }
        }
        else
        {
            if (chunkIndex + chunks.size() < step._chunkCount - maxChunkIndex)
            {
                chunkIndex += chunks.size();
                continue;
            }
        }
        
        if constexpr (Additive)
        {
            if (!key.isAncestorOf(from))
            {
                chunkIndex += chunks.size();
                continue;
            }
        }
        else // constexpr (!Additive)
        {
            if (key > result) break;
            if (!key.isAncestorOf(result) && key != result) 
            {
                chunkIndex += chunks.size();
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
        
        if constexpr (ChunkIndicesAscending)
        {
            j += minChunkIndex - chunkIndex;
            chunkIndex = minChunkIndex;
        }
        else 
        {
            j += step._chunkCount - maxChunkIndex - chunkIndex;
            chunkIndex = step._chunkCount - maxChunkIndex;
        }
        
        for (; j != chunksEnd; ++j)
        {
            if constexpr (ChunkIndicesAscending)
            {
                if (chunkIndex >= maxChunkIndex)
                {
                    result.append(index);
                    goto limitReached; // break from _primaryChunks loop
                }
            }
            else
            {
                if (chunkIndex >= step._chunkCount - minChunkIndex)
                {
                    result.append(index);
                    goto limitReached; // break from _primaryChunks loop
                }
            }
            
            bool relevant = false;
            if constexpr (Additive)
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
                        return {};
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
                
                chunkIndex += std::distance(j, chunksEnd);
                break; // chunk list loop
            }
            
            ++chunkIndex;
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

template std::optional<NodeAddress> NodeEvent::Step::PrimaryChunkMapper<true, true>::mapAddress(NodeAddress);
template std::optional<NodeAddress> NodeEvent::Step::PrimaryChunkMapper<true, false>::mapAddress(NodeAddress);
template std::optional<NodeAddress> NodeEvent::Step::PrimaryChunkMapper<false, true>::mapAddress(NodeAddress);
template std::optional<NodeAddress> NodeEvent::Step::PrimaryChunkMapper<false, false>::mapAddress(NodeAddress);

template<bool Additive, bool ChunkIndicesAscending>
std::optional<NodeChunk> 
NodeEvent::Step::PrimaryChunkMapper<Additive, ChunkIndicesAscending>::yieldMapChunk()
{
    if (!remainderChunk || !mappedParentAddress) 
        return (remainderChunk = {});
    
    if (Q_UNLIKELY(!maxChunkIndex)) 
        return std::exchange(remainderChunk, {});

    while (fChunk != fChunkEnd)
    {
        std::size_t relIndex = (*fChunk).address.lastIndex();
        std::size_t index = (*remainderChunk).address.lastIndex();
        if constexpr (Additive)
        {
            if (relIndex <= index)
            {
                (*remainderChunk).address = NodeAddressBuilder(std::move((*remainderChunk).address))
                    .offsetLastIndex((*fChunk).length);
                
                ++fChunk;
                ++chunkIndex;
                
                continue;
            }
            else if (relIndex < index + (*remainderChunk).length)
            {
                NodeChunk result {
                        (*remainderChunk).address,
                        (*fChunk).address.lastIndex() - index
                    };
                    
                (*remainderChunk).address 
                    = NodeAddressBuilder(std::move((*remainderChunk).address))
                        .offsetLastIndex(result.length);
                (*remainderChunk).length -= result.length;
                
                return result;
            }
            else // (*fChunk) is fully above `from`
            {
                return std::exchange(remainderChunk, {});
            }
        }
        else // constexpr (!Additive)
        {
            if (relIndex <= index)
            {
                if (relIndex + (*fChunk).length <= index)
                {
                    (*remainderChunk).address = NodeAddressBuilder(std::move((*remainderChunk).address))
                        .offsetLastIndex(-(*fChunk).length);
                }
                else
                {
                    std::size_t overhang = relIndex + (*fChunk).length - index;
                    
                    if (overhang < (*remainderChunk).length)
                    {
                        (*remainderChunk).address = NodeAddressBuilder(std::move((*remainderChunk).address))
                            .setLastIndex(relIndex);
                        (*remainderChunk).length -= overhang;
                    }
                    else
                    {
                        return (remainderChunk = {});
                    }
                }
                
                ++fChunk;
                ++chunkIndex;
                
                continue;
            }
            else if (relIndex < index + (*remainderChunk).length)
            {
                NodeChunk result {
                        (*remainderChunk).address,
                        relIndex - index
                    };
                
                if ((*remainderChunk).length == result.length)
                {
                    remainderChunk = {};
                }
                else
                {
                    (*remainderChunk).address = NodeAddressBuilder(std::move((*remainderChunk).address))
                        .offsetLastIndex(result.length);
                    (*remainderChunk).length -= result.length;
                }
                    
                return result;
            }
            else
            {
                return std::exchange(remainderChunk, {});
            }
        }
    }
    
    return std::exchange(remainderChunk, {});
}
            
template std::optional<NodeChunk> NodeEvent::Step::PrimaryChunkMapper<true, true>::yieldMapChunk();
template std::optional<NodeChunk> NodeEvent::Step::PrimaryChunkMapper<true, false>::yieldMapChunk();
template std::optional<NodeChunk> NodeEvent::Step::PrimaryChunkMapper<false, true>::yieldMapChunk();
template std::optional<NodeChunk> NodeEvent::Step::PrimaryChunkMapper<false, false>::yieldMapChunk();

template<typename InRange, class OutContainer>
void NodeEvent::Step::mapChunksForward(
        const InRange& in, 
        OutContainer& out, 
        std::size_t minChunkIndex,
        std::size_t maxChunkIndex) const
{
    assert(minChunkIndex <= maxChunkIndex);
    for (NodeChunk chunk : in)
    {        
        switch(_operation)
        {
            case Add:
            {
                PrimaryChunkMapper<true, true> mapper(*this, chunk, minChunkIndex, maxChunkIndex);
                
                while (auto mappedChunk = mapper.yieldMapChunk())
                    out.push_back(*mappedChunk);
                
                continue;
            }
                
            case Remove:
            {
                PrimaryChunkMapper<false, false> mapper(*this, chunk, minChunkIndex, maxChunkIndex);
                
                while (auto mappedChunk = mapper.yieldMapChunk())
                    out.push_back(*mappedChunk);
                
                continue;
            }
                
            default:
                Q_UNREACHABLE();
        }
    }
}

template<typename InRange, class OutContainer>
void NodeEvent::Step::mapChunksBackward(
        const InRange& in, 
        OutContainer& out, 
        std::size_t minChunkIndex,
        std::size_t maxChunkIndex) const
{
    assert(minChunkIndex <= maxChunkIndex);
    for (NodeChunk chunk : in)
    {   
        switch(_operation)
        {
            case Add:
            {
                PrimaryChunkMapper<false, true> mapper(*this, chunk, minChunkIndex, maxChunkIndex);
                
                while (auto mappedChunk = mapper.yieldMapChunk())
                    out.push_back(*mappedChunk);
                
                continue;
            }
                
            case Remove:
            {
                PrimaryChunkMapper<true, false> mapper(*this, chunk, minChunkIndex, maxChunkIndex);
                
                while (auto mappedChunk = mapper.yieldMapChunk())
                    out.push_back(*mappedChunk);
                
                continue;
            }
                
            default:
                Q_UNREACHABLE();
        }
    }
}

template void NodeEvent::Step::mapChunksForward(const QList<NodeChunk>&, QList<NodeChunk>&, std::size_t, std::size_t) const;
template void NodeEvent::Step::mapChunksBackward(const QList<NodeChunk>&, QList<NodeChunk>&, std::size_t, std::size_t) const;

std::optional<NodeChunk> NodeEvent::Step::mapChunkSubtractive(
        NodeChunk from, 
        std::size_t minChunkIndex,
        std::size_t maxChunkIndex) const
{
    assert(minChunkIndex <= maxChunkIndex);
    switch(_operation)
    {
        case Add:
        {
            PrimaryChunkMapper<false, true> mapper(*this, from, minChunkIndex, maxChunkIndex);
            auto result = mapper.yieldMapChunk();
            assert(!mapper.yieldMapChunk());
            
            if (result)
                return *result;
            else
                return {};
        }
            
        case Remove:
        {
            PrimaryChunkMapper<false, false> mapper(*this, from, minChunkIndex, maxChunkIndex);
            auto result = mapper.yieldMapChunk();
            assert(!mapper.yieldMapChunk());
            
            if (result)
                return *result;
            else
                return {};
        }
            
        default:
            Q_UNREACHABLE();
    }
}
                
signed NodeEvent::Step::rebase(const NodeChunk& rel)
{
    assert(_operation == NodeOperation::Add);
    
    {
        auto i = _primaryChunks.lower_bound(rel.address.parent());
        if (i == _primaryChunks.end()) return 0;
        
        auto j = std::lower_bound(
                (*i).second.begin(),
                (*i).second.end(),
                rel.address
            );
        if (j == (*i).second.end()) return 0;
    }
    
    signed result = 0;
    chunk_map_t tempPrimaryChunks;
    
    while (!_primaryChunks.empty())
    {
        auto cChunk = _primaryChunks.begin();
        auto fChunk = (*cChunk).second.begin();
        while (fChunk != (*cChunk).second.end())
        {
            auto mappedChunk = mapSingleChunk_impl<true>(*fChunk, rel);
            assert(mappedChunk.first.length);
            (*fChunk) = mappedChunk.first;
            
            if (mappedChunk.second.length)
            {
                fChunk = (*cChunk).second.insert(++fChunk, mappedChunk.second);
                ++result;
                ++_chunkCount;
            }
            
            ++fChunk;
        }
        
        auto mappedParentAddress = mapSingle_impl<true>((*cChunk).first, rel);
        assert(mappedParentAddress);
        
        auto nh = _primaryChunks.extract(cChunk);
        nh.key() = *mappedParentAddress;
        tempPrimaryChunks.insert(std::move(nh));
    }
    
    _primaryChunks = std::move(tempPrimaryChunks);
    
    return result;
}
    
signed NodeEvent::Step::addPrimaryChunk(NodeChunk&& chunk)
{
    if (chunk.length == 0 || chunk.address.isRoot())
        return 0;
    
    signed result = 0;
    
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
                        return 0;
                    
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
                result -= (*i).second.size();
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
        ++result;
        return result;
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
                return result;
            
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
                --result;
            }
            else
            {
                // Set the length of the lower sibling to encompass
                // itself and `chunk`
                (*lowerBound).length += chunk.length - overhang;
            }
            
            return result;
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
        return result;
    }
    
    // No merge
    siblings.insert(upperBound, std::move(chunk));
    ++_chunkCount;
    ++result;
    return result;
}

void NodeEvent::Step::moveRoot(NodeAddress rel)
{
    switch(_operation)
    {
        case Add:
        case Remove:
        {
            chunk_map_t newPrimaries;
            
            while (!_primaryChunks.empty())
            {
                for (auto& chunk : (*_primaryChunks.begin()).second)
                    chunk.address = rel + std::move(chunk.address);
                
                auto nh = _primaryChunks.extract(_primaryChunks.begin());
                nh.key() = rel + std::move(nh.key());
                newPrimaries.insert(std::move(nh));
            }
            
            _primaryChunks.swap(newPrimaries);
    
            for (auto& chunk : _secondaryChunks)
                chunk.address = rel + std::move(chunk.address);
            
            break;
        }
        default:
            Q_UNREACHABLE();
    }
}

void TreeObserverData::startRecording()
{
    const QWriteLocker locker(&_lock);
    
    assert(!_isRecording);
    
    _isRecording = true;
    _recording = NodeEvent();
}

NodeEvent TreeObserverData::finishRecording()
{
    const QWriteLocker locker(&_lock);
    
    assert(_isRecording);
    
    _isRecording = false;
    
    if (_recording.isNull())
        // An empty event will not be counted toward the observer's history
        return NodeEvent();

    _nodeRefMutex.lock();
    
    auto i = _nodeRefs.begin();
    while (i != _nodeRefs.end())
    {
        auto address = (*i).second->_address;
        auto mappedAddress = _recording.mapForward(address);
        
        if (mappedAddress && mappedAddress == address)
        {
            ++i;
        }
        else if (!mappedAddress)
        {
            (*i).second->_isDeleted = true;
            i = _nodeRefs.erase(i);
        }
        else
        {
            (*i).second->_address = *mappedAddress;
            
            auto extract = _nodeRefs.extract(i);
            extract.key() = *mappedAddress;
            
            i = _nodeRefs.insert(std::move(extract));
        }
    }
    
    _nodeRefMutex.unlock();

    const QMutexLocker eventListLocker(&_eventListMutex);
    
    if (_current == _events.begin() 
        && !(*_current).refCounter->count.loadRelaxed())
    {
        // The recorded event would immediately be released, so we will just
        // skip pushing it.
        return std::move(_recording);
    }
    
    const auto& result = ((*_current).event = std::move(_recording));
        
    _current = _events.emplace(_events.end());
    (*_current).refCounter = new EventEntryRefCounter;
    (*_current).refCounter->metaCount.ref();
    
#ifdef ADDLE_DEBUG 
    if (Q_UNLIKELY(_events.size() > 10))
    {
        std::size_t size = _events.size() - 1;
        unsigned long p = std::pow(10, std::floor(std::log10(size)));
        if (size == p
            || size == (p * 5)
            || (size < 100  && size % 10 == 0) 
            || (size < 1000 && size % 100 == 0))
        {
            qWarning("DataTreeObserver has recorded %lu unprocessed events.",
                     size);
            qWarning("At least %u lingering references are preventing these "
                    "events from being deleted.", 
                    _events.front().refCounter->count.loadRelaxed());
        }
    }
#endif 

    return result;
}

void TreeObserverData::releaseNodeEvent(node_event_iterator i)
{
    const QMutexLocker locker(&_eventListMutex);
    
    if (i == _events.begin())
    {
        for (; i != _current; ++i)
        {
            EventEntryRefCounter* const counter = (*i).refCounter;
            assert(counter);
            
            if (counter->count.loadRelaxed())
            {
                break;
            }
            else
            {
                if (counter->metaCount.deref())
                    counter->deletedByObserver = true;
                else
                    delete counter;
            }
        }
        
        _events.erase(_events.begin(), i);
    }
}

void TreeObserverData::removeNodeRefData(const GenericNodeRefData* data)
{
    assert(data);
    
    const QMutexLocker lock(&_nodeRefMutex);
    
    if (data->_isDeleted) return;
    
    NodeAddress address = data->_address;
    const auto& type = *data->_type;
    
    auto i = _nodeRefs.lower_bound(address);
    auto end = _nodeRefs.upper_bound(address);
    for(; i != end; ++i)
    {
        if ((*(*i).second->_type) == type)
        {
            _nodeRefs.erase(i);
            return;
        }
    }
    
    Q_UNREACHABLE();
}

bool ObservedTreeState::next()
{
    if (!_observerData) 
        return false;
    
    auto lock = _observerData->lockForRead();
    
    if (_eventIt == _observerData->_current) 
        return false;
    
    auto oldEventIt = _eventIt;
    
    _observerData->_eventListMutex.lock();
    ++_eventIt;
    _observerData->_eventListMutex.unlock();
    
    ref(_eventIt);
    deref(oldEventIt);
    
    return _eventIt != _observerData->_current;
}

std::optional<NodeAddress> ObservedTreeState::mapToCurrent(NodeAddress address) const
{
    if (!_observerData) return address;
    
    auto lock = _observerData->lockForRead();
    
    if (_eventIt == _observerData->_current) return address;
    
    QMutexLocker eventListLocker(&_observerData->_eventListMutex);
    
    auto i = _eventIt;
    auto end = _observerData->_current;
    for (; i != end; ++i)
    {
        auto mapped = (*i).event.mapForward(address);
        if (!mapped) return {};
        
        address = std::move(*mapped);
    }
    
    return address;
}
    
std::optional<NodeAddress> ObservedTreeState::mapFromCurrent(NodeAddress address) const
{
    if (!_observerData) return address;
    
    auto lock = _observerData->lockForRead();
    
    if (_eventIt == _observerData->_current) return address;
    
    QMutexLocker eventListLocker(&_observerData->_eventListMutex);
    
    auto i = std::make_reverse_iterator(_observerData->_current);
    auto end = std::make_reverse_iterator(_eventIt);
    for (; i != end; ++i)
    {
        auto mapped = (*i).event.mapBackward(address);
        if (!mapped) return {};
        
        address = std::move(*mapped);
    }
    
    return address;
}
