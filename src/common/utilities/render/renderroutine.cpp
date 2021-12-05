#include <QThread>
#include "renderroutine.hpp"

#include <QReadWriteLock>
#include <QSet>

#include "interfaces/rendering/irenderable.hpp"
#include <boost/range/adaptor/reversed.hpp>

#include "utilities/lockhandle.hpp"

#ifdef ADDLE_DEBUG
#include "utilities/debugging/qdebug_extensions.hpp"
#include "utilities/debugging/debugstring.hpp"
#endif

using namespace Addle;

aux_render::RoutineData::RoutineData(aux_render::RoutineBuilder&& builder)
    : name(builder.name()),
    opacity(qBound(0.0, builder.opacity(), 1.0)),
    compositionMode(builder.compositionMode()),
    passThroughMode(builder.passThroughMode()),
    background(builder.background())
{
    // the data tree utilities unfortunately require some additional features
    // before they can be used here, namely supporting a tree whose children are 
    // a sorted associative sequence with the likes of `node_insert_children` 
    // and `echo_tree`.
    //
    // Implementing those features will wait until the data tree utilities have 
    // an adequately robust set of unit tests, and probably some other 
    // refactoring. In the meantime, we'll do this the "old-fashioned" way with
    // recursion.
    
    auto builderComponents = std::move(builder).components();
    
    auto i = builderComponents.begin();
    const auto end = builderComponents.cend();
    for (; i != end; ++i)
    {
        QSharedDataPointer<aux_render::RoutineData> subRoutineData;
        
        if ((*i).subRoutineData())
        {
            assert(!(*i).subRoutineBuilder());
            assert(!(*i).entity());
            
            subRoutineData = std::move(*i).subRoutineData();
        }
        else if ((*i).subRoutineBuilder())
        {
            assert(!(*i).entity());
            subRoutineData = new aux_render::RoutineData(*(std::move(*i).subRoutineBuilder()));
        }
        else 
        {
            assert((*i).entity());
        }
        
        components.push_back({
                i.key(),
                std::move(subRoutineData),
                std::move(*i).entity()
            });
    }
}

aux_render::RoutineData::~RoutineData() noexcept 
{
    if (slotProxy) slotProxy->dispose();
    
    cacheMutex.lock();
    cacheMutex.unlock();
}    

aux_render::RoutineSlotProxy::RoutineSlotProxy(const RoutineData* data, const IRenderable* renderable)
    : _data(data)
{
    assert(data);
    
    const QObject* q = qobject_interface_cast<const QObject*>(renderable);
    assert(q);
    
    assert(QObject::connect(
            q, SIGNAL(destroyed()), 
            this, SLOT(onDestroyed()), 
            Qt::DirectConnection
        ));
    assert(QObject::connect(
            q, SIGNAL(renderRoutineChanged(RenderRoutineChangedEvent)), 
            this, SLOT(onRoutineChanged(RenderRoutineChangedEvent)),
            Qt::DirectConnection
        ));
    assert(QObject::connect(
            q, SIGNAL(renderChanged(QRegion, DataTreeNodeAddress)), 
            this, SLOT(onChanged(QRegion, DataTreeNodeAddress)),
            Qt::DirectConnection
        ));
    
    this->moveToThread(q->thread());
}
    
void aux_render::RoutineSlotProxy::dispose()
{
    if (Q_UNLIKELY(!_data)) return;
    
    QMutexLocker locker(&_data->cacheMutex);
    _data = nullptr;
    
    this->deleteLater();
}

void aux_render::RoutineSlotProxy::onDestroyed()
{
    if (_data) _data->setRenderable_p(nullptr);
}

void aux_render::RoutineSlotProxy::onRoutineChanged(RenderRoutineChangedEvent event)
{
    if (Q_LIKELY(_data)) _data->onRoutineChanged(std::move(event));
}

void aux_render::RoutineSlotProxy::onChanged(QRegion region, DataTreeNodeAddress entity)
{
    if (Q_LIKELY(_data)) _data->onChanged(std::move(region), std::move(entity));
}

#ifdef ADDLE_DEBUG
aux_render::RoutineQueryTimeout::RoutineQueryTimeout(
        int waited, 
        _WhichOp op, 
        _WhichLock lock
    )
    : std::runtime_error(qPrintable(
        //% "A lock timeout occurred in a query on a RenderRoutine\n"
        //% "  duration waited: %1 ms\n"
        //% "  query operation: %2\n"
        //% "  lock category:   %3"
        qtTrId("debug-messages.renderroutine.query-timeout-exception")
            .arg(waited)
            .arg(op)
            .arg(lock)
            
        // We can't use a meta enum to get key names because we need Moc to 
        // process RoutineSlotProxy and Moc can't process a Q_OBJECT and 
        // Q_GADGET in the same header/source pair -- it's absolutely not 
        // worth making another header over this trivial debug message.
    )),
    _op(op),
    _lock(lock),
    _waited(waited)
{
}
#else
aux_render::RoutineQueryTimeout::RoutineQueryTimeout(int waited)
    : _waited(waited)
{
}
#endif 

aux_render::RoutineComponentNameCollision::RoutineComponentNameCollision(QList<QByteArray> names)
#ifdef ADDLE_DEBUG
    : std::runtime_error(qPrintable(
        //% "Attempted to insert one or more components into RenderRoutine "
        //% "with names that are the same as preexisting routine components or "
        //% "subcomponents.\n"
        //% "  names: %1"
        qtTrId("debug-messages.renderroutine.component-id-collision-exception")
            .arg(aux_debug::debugString(names))
    )), 
    _names(names)
#else
    : _names(names)
#endif
{
}

RenderRoutine::RenderRoutine(QByteArray name)
    : _data(new aux_render::RoutineData(std::move(name)))
{
}

RenderRoutine::RenderRoutine(aux_render::RoutineBuilder builder)
    : _data(new aux_render::RoutineData(std::move(builder)))
{
}

std::optional<QRegion> RenderRoutine::region(DataTreeNodeAddress target, 
    bool skipCache,
    int timeout) const
{
    if (!_data) 
        return {};
    
    if (skipCache || !_data->slotProxy)
    {
        // Calculate without using the cache, by doing a full search of the
        // component tree and uniting the regions of all entities.
        
        auto component = aux_datatree::node_lookup_address(
                aux_datatree::tree_root(*this),
                target
            );
        
        if ((*component).isEntity())
        {
            const auto& entity = (*component).asEntity();
            
            auto lockHandle = ReadLockHandle::tryLock(entity.lock, timeout);
            if (Q_UNLIKELY(!lockHandle)) 
            {
                assert(timeout >= 0);
#ifdef ADDLE_DEBUG
                throw aux_render::RoutineQueryTimeout(
                        timeout, 
                        aux_render::RoutineQueryTimeout::_WhichOp_region,
                        aux_render::RoutineQueryTimeout::_WhichLock_entityReadLock
                    );
#else
                throw aux_render::RoutineQueryTimeout(timeout);
#endif
            }
            
            if (entity.region_ptr)
                return *entity.region_ptr;
            else
                return {};
        }
        
        assert((*component).isSubRoutine());
        QRegion result;
        
        for (auto&& subComponent : (*component).asSubRoutine().depthFirstSearch())
        {
            if (subComponent.isEntity())
            {
                const auto& entity = subComponent.asEntity();
                
                auto lockHandle = ReadLockHandle::tryLock(entity.lock, timeout);
                if (Q_UNLIKELY(!lockHandle)) 
                {
                    assert(timeout >= 0);
#ifdef ADDLE_DEBUG
                    throw aux_render::RoutineQueryTimeout(
                            timeout, 
                            aux_render::RoutineQueryTimeout::_WhichOp_region,
                            aux_render::RoutineQueryTimeout::_WhichLock_entityReadLock
                        );
#else
                    throw aux_render::RoutineQueryTimeout(timeout);
#endif
                }
            
                if (entity.region_ptr)
                    result = result.united(*entity.region_ptr);
                else
                    return {};
            }
        }
        
        return result;
    }
    
    auto cacheLockHandle = RecursiveMutexLockHandle::tryLock(_data->cacheMutex, timeout);
    if (Q_UNLIKELY(!cacheLockHandle))
    {
        assert(timeout >= 0);
#ifdef ADDLE_DEBUG
        throw aux_render::RoutineQueryTimeout(
                timeout, 
                aux_render::RoutineQueryTimeout::_WhichOp_region,
                aux_render::RoutineQueryTimeout::_WhichLock_cacheMutex
            );
#else
        throw aux_render::RoutineQueryTimeout(timeout);
#endif
    }
    
    if (Q_UNLIKELY(!_data->slotProxy))
    {
        // _data->renderableSignals could have been unset by a signal functor 
        // while we were waiting to acquire the mutex 
        cacheLockHandle.unlock();    
        return region(target, true);
    }
    
    if (target.isRoot())
    {
        if (_data->totalRegionCache)
            return *(_data->totalRegionCache);
        else if (_data->totalRegionIsUnbounded)
            return {};
    }
    else // !target.isRoot()
    {
        // check if componentAddress points to an entity with a cached region
        
        auto findEntity = _data->entityCacheByAddress.find(target);
        if (findEntity != _data->entityCacheByAddress.end())
        {
            _data->entityCache.splice(
                    _data->entityCache.begin(),
                    _data->entityCache,
                    (*findEntity).second
                );
            
            auto&& entityEntry = *((*findEntity).second);
            
            if (entityEntry.region)
                return *entityEntry.region;
            else if (entityEntry.regionIsUnbounded)
                return {};
        }
        
        // check if componentAddress points to a subroutine with a cached
        // region
        
        auto findSubRoutine = _data->subRoutineCacheByAddress.find(target);
        if (findSubRoutine != _data->subRoutineCacheByAddress.end())
        {
            _data->subRoutineCache.splice(
                    _data->subRoutineCache.begin(),
                    _data->subRoutineCache,
                    (*findSubRoutine).second
                );
            
            auto&& subRoutineEntry = *((*findSubRoutine).second);
            
            if (subRoutineEntry.region)
                return *subRoutineEntry.region;
            else if (subRoutineEntry.regionIsUnbounded)
                return {};
        }
    }
    
    if (Q_UNLIKELY(
        _data->slotProxy->thread() != QThread::currentThread()
    ))
    {
        // If the current thread is not the affinity thread of 
        // _data->renderableSignals then there could be potential races with
        // connecting subordinate signals and those signals being sent -- 
        // meaning it is not safe to build the cache from this thread
        //
        // This check comes after checks for entries in the existing cache
        // because those *are* safe across threads.
        
        cacheLockHandle.unlock();    
        return region(target, true);
    }
    
    Query q { _data.data(), target };
    q.op = Query::Op_CalcRegion;
    q.timeout = timeout;
    
    q.run();
    assert(q.hasResult());
    
    if (q.resultRegionIsUnbounded)
        return {};
    else
        return q.resultRegion;
}


aux_render::RoutineNodeHandle RenderRoutine::findComponent(DataTreeNodeAddress componentAddress,
    bool skipCache,
    int timeout) const
{
    if (!_data) 
        return {};
    
    if (skipCache || !_data->slotProxy)
    {
    }
}
    
aux_render::RoutineNodeHandle RenderRoutine::findComponentByName(QByteArray name,
    bool skipCache,
    int timeout) const
{
    
}

void RenderRoutine::addEntity_p(
        QSharedPointer<aux_render::Entity> entity,
        double z
    )
{
    initData();
    
    _data->components.insert(
            std::upper_bound(_data->components.begin(), _data->components.end(), z),
            {
                z,
                QSharedDataPointer<aux_render::RoutineData> {},
                std::move(entity)
            }
        );
}

void RenderRoutine::addSubRoutine_p(
        RenderRoutine subRoutine,
        double z
    )
{
    assert(subRoutine._data);
    initData();
    
    _data->components.insert(
            std::upper_bound(_data->components.begin(), _data->components.end(), z),
            {
                z,
                subRoutine._data
            }
        );
}

void aux_render::RoutineData::setRenderable_p(const IRenderable* renderable) const
{    
    const QMutexLocker cacheLock(&cacheMutex);
    
    if (slotProxy) slotProxy->dispose();
        
    invalidateCache();
    
    if (renderable)
        slotProxy = new aux_render::RoutineSlotProxy(this, renderable);
    else
        slotProxy = nullptr;
}

void RenderRoutine::Query::run()
{
    using namespace boost::adaptors;
    assert( !hasResult()
            && data 
            && data->slotProxy
            && data->slotProxy->thread() == QThread::currentThread());
    
    cursor = aux_render::RoutineNodeHandle(data);
    
    if (!target.isRoot())
    {
        // Find the nearest ancestor to the target that we have in the cache
        
        auto i = data->subRoutineCacheByAddress.lower_bound(target);
        while (i != data->subRoutineCacheByAddress.begin())
        {
            if (i != data->subRoutineCacheByAddress.end())
            {
                if ( (*i).first.commonAncestorIndex(target) == (*i).first.end() )
                {
                    // the cache contains no entries with a common ancestor 
                    // to target
                    break;
                }
                if ( (*i).first == target || (*i).first.isAncestorOf(target) )
                {
                    cursor = aux_render::RoutineNodeHandle((*(*i).second).subRoutineData);
                    cursorAddress = (*i).first;
                    break;
                }
            }
            --i;
        }
        
        // Move the cursor to the target
        
        auto diff = target.lowerTruncate(cursorAddress);
        assert(diff);
        
        for (auto index : (*diff))
        {
            if (!cursorAddress.isRoot() && cursor.routineData())
            {
                subRoutines.push_back({
                        cursor.routineData(),
                        cursorAddress
                    });
                
#ifdef ADDLE_DEBUG
                assert(data->subRoutineCacheByPointer.find(cursor.routineData()) 
                    == data->subRoutineCacheByPointer.end());
                
                if (cursor.routineData()->cacheMutex.tryLock(timeout))
                {
                    if (cursor.routineData()->slotProxy
                        && Q_UNLIKELY(
                            data->slotProxy->thread() 
                                != cursor.routineData()->slotProxy->thread()
                        ))
                    {
                        qWarning("%s", qUtf8Printable(
                            //% "Encountered a render subroutine associated to "
                            //% "an IRenderable with a different thread affinity "
                            //% "to the IRenderable of this routine.\n"
                            //% "  subroutine name: %1"
                            qtTrId("debug-messages.renderroutine.subordinate-differing-thread-affinity")
                                .arg(aux_debug::debugString(cursor.routineData()->name))
                        ));
                        
                        // This is not necessarily an error but *is* unexpected 
                        // and can lead to problems.
                    }
                    cursor.routineData()->cacheMutex.unlock();
                }
                else
                {
                    qWarning("%s", qUtf8Printable(
                        //% "Attempted lock of subroutine cache mutex timed out, "
                        //% "thread affinity of associated IRenderable could not "
                        //% "be verified.\n"
                        //% "  subroutine name: %1"
                        qtTrId("debug-messages.renderroutine.subordinate-thread-affinity-timeout")
                            .arg(aux_debug::debugString(cursor.routineData()->name))
                    ));
                    
                    // This is also not necessarily an error, but again is
                    // unexpected and can be an indicator of problems.
                }
#endif
            }

            cursor = aux_datatree::node_child_at(cursor, index);
            cursorAddress = std::move(cursorAddress) << index;
            
            if (Q_UNLIKELY(!cursor))
            {
                // previous cursor did not have a child at `index`, i.e., 
                // `target` is not a valid address in this routine tree
                // (this includes the case that a previous iteration of this
                // loop found an entity instead of a subroutine).
                
                commit();
                return; // throw?
            }
        }
    }
    
    assert(cursor && cursorAddress == target);

    resultNode = cursor;
    if (op == Op_FindComponentByAddress)
    {
        commit();
        return;
    }
    
    // Strictly speaking, neither a depth- nor breadth-first search can be 
    // assumed faster on average, but a BFS gives us a convenient heuristic -- 
    // 
    
    auto search = aux_datatree::BreadthFirstSearch(cursor);
    if (target.isRoot())
        search.next();
    
    while (search.hasNext())
    {
        cursor = search.current();
        cursorAddress = target + search.address();
    
        if (cursor.entity())
        {
            aux_render::RoutineData::EntityCacheEntry* entry;
            
            auto findEntity = data->entityCacheByPointer.find(cursor.entity());
            if (findEntity != data->entityCacheByPointer.end())
            {
                entry = &(*(*findEntity).second);
            }
            else
            {
                entities.push_back({
                        cursor.entity(),
                        cursorAddress
                    });
                entry = &(entities.back());
            }
            
            switch(op)
            {
                case Op_CalcRegion:
                {
                    QRegion cursorRegion;
                    bool cursorRegionIsUnbounded = false;
            
                    if (entry->region || entry->regionIsUnbounded)
                    {
                        assert(!(entry->region && entry->regionIsUnbounded));
                        
                        cursorRegionIsUnbounded = entry->regionIsUnbounded;
                        
                        if (!cursorRegionIsUnbounded)
                            cursorRegion = *(entry->region);
                    }
                    else
                    {
                        if (cursor.entity()->region_ptr)
                        {
                            const auto lockHandle = ReadLockHandle::tryLock(cursor.entity()->lock, timeout);
                            if (Q_UNLIKELY(!lockHandle))
                            {
                                assert(timeout >= 0);
                                commit();
#ifdef ADDLE_DEBUG
                                throw aux_render::RoutineQueryTimeout(
                                        timeout, 
                                        aux_render::RoutineQueryTimeout::_WhichOp_region,
                                        aux_render::RoutineQueryTimeout::_WhichLock_entityReadLock
                                    );
#else
                                throw aux_render::RoutineQueryTimeout(timeout);
#endif
                            }
                            entry->region = (cursorRegion = *(cursor.entity()->region_ptr));
                        }
                        else
                        {
                            cursorRegionIsUnbounded = true;
                            entry->regionIsUnbounded = true;
                        }
                    }
                    
                    assert(cursorRegion.isEmpty() || !cursorRegionIsUnbounded);
                    
                    updatePendingRegions(cursorRegion, cursorRegionIsUnbounded);
                    
                    if (cursorRegionIsUnbounded)
                    {
                        resultRegionIsUnbounded = true;
                        resultRegion = QRegion();
                        commit();
                        return;
                    }
                    else
                    {
                        resultRegion += cursorRegion;
                    }
                    
                    break; // switch
                }
                
                case Op_FindComponentByName:
                {
                    if (cursor.entity()->name == targetName)
                    {
                        resultNode = cursor;
                        resultAddress = cursorAddress;
                        commit();
                        return;
                    }
                    break; // switch
                }
                
                default:
                    Q_UNREACHABLE();
            }
        }
        else // !cursor.entity()
        {
            assert(cursor.routineData());
            
            aux_render::RoutineData::SubRoutineCacheEntry* entry = nullptr;
            
            auto findSubRoutine = data->subRoutineCacheByPointer.find(cursor.routineData());
            if (findSubRoutine != data->subRoutineCacheByPointer.end())
            {
                entry = &(*(*findSubRoutine).second);
            }
            else
            {
                subRoutines.push_back({
                        cursor.routineData(),
                        cursorAddress
                    });
                entry = &subRoutines.back();
            }
            
#ifdef ADDLE_DEBUG
            if (cursor.routineData()->cacheMutex.tryLock(timeout))
            {
                if (cursor.routineData()->slotProxy
                    && Q_UNLIKELY(
                        data->slotProxy->thread() 
                            != cursor.routineData()->slotProxy->thread()
                    ))
                {
                    qWarning("%s", qUtf8Printable(
                        qtTrId("debug-messages.renderroutine.subordinate-differing-thread-affinity")
                            .arg(aux_debug::debugString(cursor.routineData()->name))
                    ));
                }
                cursor.routineData()->cacheMutex.unlock();
            }
            else
            {
                qWarning("%s", qUtf8Printable(
                    qtTrId("debug-messages.renderroutine.subordinate-thread-affinity-timeout")
                        .arg(aux_debug::debugString(cursor.routineData()->name))
                ));
            }
#endif

            switch(op)
            {
                case Op_CalcRegion:
                {
                    if (entry->region || entry->regionIsUnbounded)
                    {
                        assert(!(entry->region && entry->regionIsUnbounded));
                        
                        updatePendingRegions(
                                entry->region.value_or(QRegion()),
                                entry->regionIsUnbounded
                            );
                        
                        if (entry->regionIsUnbounded)
                        {
                            resultRegionIsUnbounded = true;
                            resultRegion = QRegion();
                            commit();
                            return;
                        }
                        else
                        {
                            resultRegion += *(entry->region);
                            
                            search.skip();
                            continue; // while
                        }
                    }
                    else
                    {
                        pendingRegions.push_back({ cursorAddress, QRegion() });
                    }
                    
                    break; // switch
                }
                
                case Op_FindComponentByName:
                {
                    if (cursor.routineData()->name == targetName)
                    {
                        resultNode = cursor;
                        resultAddress = cursorAddress;
                        commit();
                        return;
                    }
                    
                    break; // switch
                }
                
                default:
                    Q_UNREACHABLE();
            }
        }
        
        search.next();
    }
    
    if (op == Op_CalcRegion)
    {
        // The full tree has been searched, so all pending regions are fully
        // calculated and we can write their values to their respective cache 
        // entries.
        
        auto sIt    = subRoutines.begin();
        auto sEnd   = subRoutines.end();
        
        for (const auto& pending : pendingRegions)
        {
            aux_render::RoutineData::SubRoutineCacheEntry* entry;
            
            auto findSubRoutine = data->subRoutineCacheByAddress.find(pending.address);
            if (findSubRoutine != data->subRoutineCacheByAddress.end())
            {
                entry = &(*(*findSubRoutine).second);
            }
            else
            {
                sIt = std::lower_bound(
                    sIt, sEnd, pending.address,
                    std::bind(aux_datatree::NodeAddressBFSComparator {},
                        std::bind(&aux_render::RoutineData::SubRoutineCacheEntry::address, std::placeholders::_1),
                        std::placeholders::_2
                    ));
                
                assert(sIt != sEnd && (*sIt).address == pending.address);
                
                entry = &(*sIt);
            }
            
            entry->region = pending.region;
        }
        
        pendingRegions.clear();
    }
    
    commit();
}

void RenderRoutine::Query::updatePendingRegions(QRegion cursorRegion, bool cursorRegionIsUnbounded)
{
    auto ancestorAddress = cursorAddress.parent();
    auto pendingIt = pendingRegions.end();
    
    while (!ancestorAddress.isRoot())
    {
        const auto end = pendingIt;
        pendingIt = std::lower_bound(
            pendingRegions.begin(), end, ancestorAddress,
            std::bind(aux_datatree::NodeAddressBFSComparator {},
                std::bind(&PendingRegionEntry::address, std::placeholders::_1),
                std::placeholders::_2
            ));
        
        if (pendingIt == end || (*pendingIt).address != ancestorAddress)
            return;
        
        if (cursorRegionIsUnbounded)
        {
            aux_render::RoutineData::SubRoutineCacheEntry* entry;
        
            auto cacheSFind = data->subRoutineCacheByAddress.find((*pendingIt).address);
            if (cacheSFind != data->subRoutineCacheByAddress.end())
            {
                entry = &(*(*cacheSFind).second);
            }
            else
            {
                auto querySFind = std::lower_bound(
                    subRoutines.begin(), subRoutines.end(), ancestorAddress,
                    std::bind(aux_datatree::NodeAddressBFSComparator {},
                        std::bind(&aux_render::RoutineData::SubRoutineCacheEntry::address, std::placeholders::_1),
                        std::placeholders::_2
                    ));
                
                assert(querySFind != subRoutines.end() && (*querySFind).address == ancestorAddress);
                
                entry = &(*querySFind);
            }
            
            entry->regionIsUnbounded = true;
            pendingIt = pendingRegions.erase(pendingIt);
        }
        else
        {
            (*pendingIt).region += cursorRegion;
        }
        
        ancestorAddress = std::move(ancestorAddress).parent();
    }
}
        
void RenderRoutine::Query::commit()
{
    if (op == Op_CalcRegion)
    {
        data->totalRegionIsUnbounded |= resultRegionIsUnbounded;
        if (target.isRoot() && !data->totalRegionIsUnbounded)
            data->totalRegionCache = resultRegion;
        else 
            data->totalRegionCache = {};
    }
    
    // The cache entry corresponding to the query result (if it exists) gets 
    // put in the front of the cache.
    
    auto resultEEntry = entities.cend();
    auto resultSEntry = subRoutines.cend();
    
    if (!target.isRoot())
    {
        // We expect the result entry to be near the front (quite possibly the 
        // first entry)
        
        if (resultNode.entity())
        {
            resultEEntry = entities.cbegin();
            while ((*resultEEntry).entity != resultNode.entity())
            {
                ++resultEEntry;
                assert(resultEEntry != entities.cend());
            }
            
            auto findEntity = data->entityCacheByPointer.find((*resultEEntry).entity);
            if (findEntity != data->entityCacheByPointer.cend())
            {
                data->entityCache.splice(
                        data->entityCache.begin(),
                        data->entityCache,
                        (*findEntity).second
                    );
            }
            else
            {
                auto i = data->entityCache.insert(data->entityCache.begin(), *resultEEntry);
                data->entityCacheByPointer[(*i).entity] = i;
                data->entityCacheByAddress[(*i).address] = i;
                if (!(*i).entity->name.isEmpty())
                    data->entityCacheByName[(*i).entity->name] = i;
                
                data->pruneCache();
            }
        }
        else if (resultNode.routineData())
        {
            resultSEntry = subRoutines.cbegin();
            while ((*resultSEntry).subRoutineData != resultNode.routineData())
            {
                ++resultSEntry;
                assert(resultSEntry != subRoutines.cend());
            }
            
            auto findSubRoutine = data->subRoutineCacheByPointer.find((*resultSEntry).subRoutineData);
            if (findSubRoutine != data->subRoutineCacheByPointer.cend())
            {
                data->subRoutineCache.splice(
                        data->subRoutineCache.begin(),
                        data->subRoutineCache,
                        (*findSubRoutine).second
                    );
            }
            else
            {
                auto i = data->subRoutineCache.insert(data->subRoutineCache.begin(), *resultSEntry);
                data->subRoutineCacheByPointer[(*i).subRoutineData] = i;
                data->subRoutineCacheByAddress[(*i).address] = i;
                if (!(*i).subRoutineData->name.isEmpty())
                    data->subRoutineCacheByName[(*i).subRoutineData->name] = i;
                
                data->pruneCache();
            }
        }
    }
    
    const auto eEnd = entities.cbegin() + std::min<std::size_t>(
            std::max<std::ptrdiff_t>(
                0, entities.size() - ((resultEEntry != entities.cend()) ? 1 : 0)
            ),
            data->maxEntityCacheCount - data->entityCache.size()
        );
        
    for (auto eIt = entities.cbegin(); eIt != eEnd; ++eIt)
    {
        if (eIt == resultEEntry) continue;
        
        auto i = data->entityCache.insert(data->entityCache.cend(), *eIt);
        data->entityCacheByPointer[(*i).entity] = i;
        data->entityCacheByAddress[(*i).address] = i;
        if (!(*i).entity->name.isEmpty())
            data->entityCacheByName[(*i).entity->name] = i;
    }
    
    assert(data->entityCache.size() <= data->maxEntityCacheCount);
    
    const auto sEnd = subRoutines.cbegin() + std::min<std::size_t>(
            std::max<std::ptrdiff_t>(
                0, subRoutines.size() - ((resultSEntry != subRoutines.cend()) ? 1 : 0)
            ),
            data->maxSubRoutineCacheCount - data->subRoutineCache.size()
        );
    
    for (auto sIt = subRoutines.cbegin(); sIt != sEnd; ++sIt)
    {
        if (sIt == resultSEntry) continue;
        
        auto i = data->subRoutineCache.insert(data->subRoutineCache.cend(), *sIt);
        data->subRoutineCacheByPointer[(*i).subRoutineData] = i;
        data->subRoutineCacheByAddress[(*i).address] = i;
        if (!(*i).subRoutineData->name.isEmpty())
            data->subRoutineCacheByName[(*i).subRoutineData->name] = i;
    }
    
    assert(data->subRoutineCache.size() <= data->maxSubRoutineCacheCount);
}


void aux_render::RoutineData::onRoutineChanged(RenderRoutineChangedEvent event) const 
{
    if (event.nodeEvent().isNull()) return;
    
    const QMutexLocker locker(&cacheMutex);
    
    for (const auto& h : event.nodeEvent())
    {
        if (Q_UNLIKELY(entityCache.empty() && subRoutineCache.empty())) return;
        
        switch(h.operation())
        {
            case DataTreeNodeEvent::Add:
            case DataTreeNodeEvent::Remove:
            {
                auto parentAddress = h.operatingChunk().address.parent();
                
                auto subRoutineIt = subRoutineCacheByAddress.lower_bound(h.operatingChunk().address);
                while (subRoutineIt != subRoutineCacheByAddress.end())
                {
                    if (!parentAddress.isAncestorOf((*subRoutineIt).first))
                        break;
                    
                    if (!(*(*subRoutineIt).second).subRoutineData->name.isEmpty())
                        subRoutineCacheByName.erase((*(*subRoutineIt).second).subRoutineData->name);
                    subRoutineCacheByPointer.erase((*(*subRoutineIt).second).subRoutineData);
                    subRoutineCache.erase((*subRoutineIt).second);
                    subRoutineCacheByAddress.erase(subRoutineIt);
                }
                
                auto entityIt = entityCacheByAddress.lower_bound(h.operatingChunk().address);
                while (entityIt != entityCacheByAddress.end())
                {
                    if (!parentAddress.isAncestorOf((*entityIt).first))
                        break;
                    
                    if (!(*(*entityIt).second).entity->name.isEmpty())
                        entityCacheByName.erase((*(*entityIt).second).entity->name);
                    entityCacheByPointer.erase((*(*entityIt).second).entity);
                    entityCache.erase((*entityIt).second);
                    entityCacheByAddress.erase(entityIt);
                }
                
                while (!parentAddress.isRoot())
                {
                    auto findSubRoutine = subRoutineCacheByAddress.find(parentAddress);
                    if (findSubRoutine != subRoutineCacheByAddress.end())
                        (*(*findSubRoutine).second).region = {};
                    
                    parentAddress = std::move(parentAddress).parent();
                }
                
                break;
            }
                
            default:
                Q_UNREACHABLE();
        }
        
        totalRegionCache = {};
    }
}

void aux_render::RoutineData::onChanged(QRegion region, DataTreeNodeAddress entity) const 
{
    if (Q_UNLIKELY(region.isNull() || entity.isRoot())) return;
    
    const QMutexLocker locker(&cacheMutex);
    
    auto findEntity = entityCacheByAddress.find(entity);
    if (findEntity != entityCacheByAddress.end())
    {
        (*(*findEntity).second).region = {};
        (*(*findEntity).second).regionIsUnbounded = false;
    }
    
    auto cursor = entity.parent();
    while (!cursor.isRoot())
    {
        auto findSubRoutine = subRoutineCacheByAddress.find(cursor);
        if (findSubRoutine != subRoutineCacheByAddress.end())
        {
            (*(*findSubRoutine).second).region = {};
            (*(*findSubRoutine).second).regionIsUnbounded = false;
        }
        cursor = std::move(cursor).parent();
    }
    
    totalRegionCache = {};
    totalRegionIsUnbounded = false;
}

void aux_render::RoutineData::pruneCache() const
{
    while (subRoutineCache.size() > maxSubRoutineCacheCount)
    {
        const auto& back = subRoutineCache.back();
        
        subRoutineCacheByAddress.erase(back.address);
        subRoutineCacheByPointer.erase(back.subRoutineData);
        if(!back.subRoutineData->name.isEmpty())
            subRoutineCacheByName.erase(back.subRoutineData->name);
        
        subRoutineCache.pop_back();
    }
    
    while (entityCache.size() > maxEntityCacheCount)
    {
        const auto& back = entityCache.back();
        entityCacheByAddress.erase(back.address);
        entityCacheByPointer.erase(back.entity);
        if(!back.entity->name.isEmpty())
            entityCacheByName.erase(back.entity->name);
        
        entityCache.pop_back();
    }
}

void aux_render::RoutineData::invalidateCache() const
{
    subRoutineCacheByAddress.clear();
    subRoutineCacheByName.clear();
    subRoutineCacheByPointer.clear();
    subRoutineCache.clear();
    
    entityCacheByAddress.clear();
    entityCacheByName.clear();
    entityCacheByPointer.clear();
    entityCache.clear();
    
    totalRegionCache = {};
}

RenderRoutineChangedEvent& RenderRoutineChangedEvent::moveRoot(DataTreeNodeAddress rel)
{
    _nodeEvent.moveRoot(rel);
    
    sub_routine_map_t newSubRoutineEvents;
    
    while (!_subRoutineEvents.empty())
    {
        auto i = _subRoutineEvents.begin();
        newSubRoutineEvents[rel + i.key()] = std::move(*i);
        _subRoutineEvents.erase(i);
    }
    
    _subRoutineEvents.swap(newSubRoutineEvents);
    
    assert(!_subRoutineEvents.contains(rel));
    auto& atRel = _subRoutineEvents[rel];
    
    atRel._opacity                  = _opacity;
    atRel._opacityChanged           = _opacityChanged;
    atRel._compositionMode          = _compositionMode;
    atRel._compositionModeChanged   = _compositionModeChanged;
    atRel._passThroughMode          = _passThroughMode;
    atRel._passThroughModeChanged   = _passThroughModeChanged;
    atRel._background               = std::move(_background);
    atRel._backgroundChanged        = _backgroundChanged;
        
    _opacityChanged         = false;
    _compositionModeChanged = false;
    _passThroughModeChanged = false;
    _backgroundChanged      = false;
    
    return *this;
}

RenderRoutineChangedEvent& 
RenderRoutineChangedEvent::populateZValues(const RenderRoutine& routine)
{
    auto searchRoot = aux_datatree::tree_root(routine);
        
    for ( auto parentAddress : noDetach(_nodeEvent.affectedParentsFromEnd()) )
    {
        auto parentNode = aux_datatree::node_lookup_address(
                searchRoot,
                parentAddress
            );
        
        assert(parentNode && (*parentNode).isSubRoutine());
        auto subRoutine = (*parentNode).asSubRoutine();
        
        QList<double> values;
        values.reserve(subRoutine.components().size());
        for (auto&& c : subRoutine.components())
            values.append(c.z());
        
        setZValues(parentAddress, std::move(values));
    }
    
    return *this;
}

//#include "utilities/render/renderroutine.moc"
