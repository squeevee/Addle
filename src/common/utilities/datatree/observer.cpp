#include "./observer.hpp"

using namespace Addle::aux_datatree;

template<bool additive>
NodeAddress Addle::aux_datatree::mapSingle_impl(
        const NodeAddress& from,
        const NodeChunk& rel,
        bool* terminal)
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

template NodeAddress Addle::aux_datatree::mapSingle_impl<true>(const NodeAddress&, const NodeChunk&, bool* terminal);
template NodeAddress Addle::aux_datatree::mapSingle_impl<false>(const NodeAddress&, const NodeChunk&, bool* terminal);

std::ptrdiff_t NodeEvent::Step::childCountOffset(
        NodeAddress parent, 
        std::size_t progress) const
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
NodeAddress NodeEvent::Step::primaryMap_impl(
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

template NodeAddress NodeEvent::Step::primaryMap_impl<true, true>(const NodeAddress&, std::size_t, bool*) const;
template NodeAddress NodeEvent::Step::primaryMap_impl<true, false>(const NodeAddress&, std::size_t, bool*) const;
template NodeAddress NodeEvent::Step::primaryMap_impl<false, true>(const NodeAddress&, std::size_t, bool*) const;
template NodeAddress NodeEvent::Step::primaryMap_impl<false, false>(const NodeAddress&, std::size_t, bool*) const;

void NodeEvent::Step::rebase(const NodeChunk& rel)
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
    
void NodeEvent::Step::addPrimaryChunk(NodeChunk&& chunk)
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


void NodeEvent::StepBuilder::addPrimaryChunk(NodeChunk chunk)
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

NodeEvent::StepBuilder NodeEvent::addStep(NodeOperation type)
{
    initData();
    
    bool isNew = false;
    
    if (_data->steps.isEmpty() || _data->steps.last().operation() != type)
    {
        _data->steps.append( Step(type) );
        isNew = true;
    }
    
    Step& step = _data->steps.last();
    return StepBuilder(step, _data->totalChunkCount, isNew);
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
        bool isDeleted = false;
        auto address = (*i).second->address();
        auto mappedAddress = _recording.mapForward(address, SIZE_MAX, &isDeleted);
        
        if (!isDeleted && mappedAddress == address)
        {
            ++i;
            continue;
        }
        
        if (isDeleted)
        {
            (*i).second->markDeleted();
            i = _nodeRefs.erase(i);
        }
        else
        {
            (*i).second->setAddress(mappedAddress);
            
            auto extract = _nodeRefs.extract(i);
            extract.key() = mappedAddress;
            
            i = _nodeRefs.insert(std::move(extract));
        }
    }
    
    _nodeRefMutex.unlock();

    const QMutexLocker eventListLocker(&_eventListMutex);
    
    if (_current == _events.begin() && !(*_current).refCounter->count.loadRelaxed())
        // The recorded event would immediately be released, so we will just
        // skip pushing it.
        return std::move(_recording);
    
    const auto& result = ((*_current).event = std::move(_recording));
        
    _current = _events.emplace(_events.end());
    (*_current).refCounter = new EventEntryRefCounter;
    (*_current).refCounter->metaCount.ref();

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
    
    if (data->isDeleted()) return;
    
    NodeAddress address = data->address();
    const auto& type = data->type();
    
    auto i = _nodeRefs.lower_bound(address);
    auto end = _nodeRefs.upper_bound(address);
    for(; i != end; ++i)
    {
        if ((*i).second->type() == type)
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

NodeAddress ObservedTreeState::mapToCurrent(NodeAddress address, bool* wasDeleted) const
{
    if (wasDeleted) 
        *wasDeleted = false;
    
    if (!_observerData) return address;
    
    auto lock = _observerData->lockForRead();
    
    if (_eventIt == _observerData->_current) return address;
    
    QMutexLocker eventListLocker(&_observerData->_eventListMutex);
    
    auto i = _eventIt;
    auto end = _observerData->_current;
    for (; i != end; ++i)
    {
        address = (*i).event.mapForward(address, SIZE_MAX, wasDeleted);
        if (wasDeleted && *wasDeleted) break;
    }
    
    return address;
}
    
NodeAddress ObservedTreeState::mapFromCurrent(NodeAddress address, bool* wasAdded) const
{
    if (wasAdded) 
        *wasAdded = false;
    
    if (!_observerData) return address;
    
    auto lock = _observerData->lockForRead();
    
    if (_eventIt == _observerData->_current) return address;
    
    QMutexLocker eventListLocker(&_observerData->_eventListMutex);
    
    auto i = std::make_reverse_iterator(_observerData->_current);
    auto end = std::make_reverse_iterator(_eventIt);
    for (; i != end; ++i)
    {
        address = (*i).event.mapBackward(address, SIZE_MAX, wasAdded);
        if (wasAdded && *wasAdded) break;
    }
    
    return address;
}
