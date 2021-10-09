#pragma once

#include <algorithm>
#include <deque>
#include <unordered_set>

#include <boost/container_hash/extensions.hpp>

#include <QHash>
#include <QModelIndex>
#include <QAbstractItemModel>

#include "utilities/datatree/observer.hpp"

namespace Addle {

// 
template<class ItemModel, class Tree>
class ObservedTreeItemModelHelperBase
{
    struct CacheEntry;
    using handle_t = aux_datatree::const_node_handle_t<Tree>;
    
    ObservedTreeItemModelHelperBase(const ObservedTreeItemModelHelperBase&) = delete;
    ObservedTreeItemModelHelperBase& operator=(const ObservedTreeItemModelHelperBase&) = delete;
    
protected:
    ObservedTreeItemModelHelperBase()
    {
        static_assert(std::is_base_of_v<QAbstractItemModel, ItemModel>);
    }
    
    ~ObservedTreeItemModelHelperBase();
    
    void setTree(const Tree& tree);
    
    void unsetTree();
    
    QModelIndex index_impl(int row, const QModelIndex& parent) const;
    
    QModelIndex parent_impl(const QModelIndex& index) const;
    
    bool hasChildren_impl(const QModelIndex& parent) const;
    
    std::size_t rowCount_impl(const QModelIndex& parent) const;
    
    handle_t getNode(const QModelIndex& index) const;
    QModelIndex getIndex(DataTreeNodeAddress address) const;
    
    void onNodesChanged(DataTreeNodeEvent event);
    
    handle_t _root = {};
    aux_datatree::ObservedTreeState _state;
    
private:
    void pruneTransientCache() const;
    
    std::size_t _maxCacheSize = 1024;
    
    using transient_cache_t = std::list<CacheEntry*>;
    using persistent_cache_t = std::unordered_set<CacheEntry*>;
    
    struct CacheEntry
    {
        DataTreeNodeAddress address;
        
        handle_t directHandle = {};
        //DataTreeNodeAddress mappedAddress;
        
        std::size_t childCount = SIZE_MAX;
        
        typename transient_cache_t::iterator    transientIt;
    };
    
    mutable transient_cache_t                   _transientCache;
    mutable persistent_cache_t                  _persistentCache;
    
    mutable std::unordered_map<
            DataTreeNodeAddress,
            CacheEntry*
        >                                       _locationCache;
        
    mutable std::size_t _cachedRootChildCount = SIZE_MAX;
};

template<class ItemModel, class Tree>
ObservedTreeItemModelHelperBase<ItemModel, Tree>::~ObservedTreeItemModelHelperBase()
{
    qDebug("Destroying ObservedTreeItemModelHelperBase");
    for (CacheEntry* entry : _persistentCache)
        delete entry;
    
    for (CacheEntry* entry : _transientCache)
        delete entry;
}

template<class ItemModel, class Tree>
void ObservedTreeItemModelHelperBase<ItemModel, Tree>::setTree(const Tree& tree)
{
    static_cast<ItemModel*>(this)->beginResetModel();
    
    for (CacheEntry* entry : _persistentCache)
        delete entry;
    
    for (CacheEntry* entry : _transientCache)
        delete entry;
    
    _persistentCache.clear();
    _transientCache.clear();
    _locationCache.clear();
    _cachedRootChildCount = SIZE_MAX;

    if constexpr 
        (boost::mp11::mp_valid<aux_datatree::_tree_observer_t, Tree&>::value)
    {
        _state = aux_datatree::ObservedTreeState(
                aux_datatree::tree_observer(tree)
            );
    }
    else
    {
        // item model is responsible for attaching observer to _state
        _state.clear();
    }
    _root = aux_datatree::tree_root(tree);
    
    static_cast<ItemModel*>(this)->endResetModel();
}

template<class ItemModel, class Tree>
void ObservedTreeItemModelHelperBase<ItemModel, Tree>::unsetTree()
{
    static_cast<ItemModel*>(this)->beginResetModel();
    
    for (CacheEntry* entry : _persistentCache)
        delete entry;
    
    for (CacheEntry* entry : _transientCache)
        delete entry;
    
    _persistentCache.clear();
    _transientCache.clear();
    _locationCache.clear();
    _cachedRootChildCount = SIZE_MAX;
    
    _state.clear();
    _root = {};
    
    static_cast<ItemModel*>(this)->endResetModel();
}

template<class ItemModel, class Tree>
QModelIndex ObservedTreeItemModelHelperBase<ItemModel, Tree>::index_impl(
        int row, const QModelIndex& parent) const
{
    if (Q_UNLIKELY(!_root)) return QModelIndex();
    
    CacheEntry* parentEntry 
        = static_cast<CacheEntry*>(parent.internalPointer());
    
    DataTreeNodeAddress parentAddress = parentEntry ? 
        parentEntry->address : DataTreeNodeAddress();
    
    return getIndex(parentAddress << row);
}

template<class ItemModel, class Tree>
QModelIndex ObservedTreeItemModelHelperBase<ItemModel, Tree>::parent_impl(
        const QModelIndex& index) const
{
    if (Q_UNLIKELY(!_root)) return QModelIndex();
    
    CacheEntry* entry = static_cast<CacheEntry*>(index.internalPointer());
    
    if (Q_UNLIKELY(!entry)) return QModelIndex();
            
    return getIndex(entry->address.parent());
}

template<class ItemModel, class Tree>
bool ObservedTreeItemModelHelperBase<ItemModel, Tree>::hasChildren_impl(
        const QModelIndex& parent) const
{
    if (Q_UNLIKELY(!_root)) return false;
    
    auto lock = _state.lockObserverForRead();
    
    CacheEntry* parentEntry = 
        static_cast<CacheEntry*>(parent.internalPointer());
    if (parentEntry && parentEntry->transientIt != _transientCache.end())
    {
        _transientCache.splice(
                _transientCache.begin(), 
                _transientCache, 
                parentEntry->transientIt
            );
    }
    
    {
        std::size_t cachedCount = parentEntry ? 
            parentEntry->childCount : _cachedRootChildCount;
        
        if (cachedCount != SIZE_MAX)
            return static_cast<bool>(cachedCount);
    }
    
    DataTreeNodeAddress parentAddress = parentEntry ? 
        parentEntry->address : DataTreeNodeAddress();
    
    auto mappedAddress = _state.mapToCurrent(parentAddress);
    
    if (Q_LIKELY(mappedAddress))
    {
        if (parentEntry && !parentEntry->directHandle)
        {
            parentEntry->directHandle = aux_datatree::node_lookup_address(
                    _root,
                    *mappedAddress
                );
        }
        
        const handle_t& node = parentEntry ? parentEntry->directHandle : _root;
        
        return aux_datatree::node_children_begin(node) 
            != aux_datatree::node_children_end(node);
    }
    else
    {
        // See note in analogous section of rowCount_impl
        
        for (QModelIndex index : noDetach( 
            static_cast<const ItemModel*>(this)->persistentIndexList() ))
        {
            const DataTreeNodeAddress& address 
                = static_cast<const CacheEntry*>(index.internalPointer())
                ->address;
            
            if (parentAddress.isAncestorOf(address))
                return true;
        }
        
        return false;
    }
}

template<class ItemModel, class Tree>
std::size_t ObservedTreeItemModelHelperBase<ItemModel, Tree>::rowCount_impl(
        const QModelIndex& parent) const
{
    if (Q_UNLIKELY(!_root)) return 0;
    
    auto lock = _state.lockObserverForRead();
    
    CacheEntry* parentEntry = 
        static_cast<CacheEntry*>(parent.internalPointer());
    if (parentEntry && parentEntry->transientIt != _transientCache.end())
    {
        _transientCache.splice(
                _transientCache.begin(), 
                _transientCache, 
                parentEntry->transientIt
            );
    }
    
    std::size_t& cachedResult = parentEntry ? 
        parentEntry->childCount : _cachedRootChildCount;
    
    if (cachedResult != SIZE_MAX)
        return cachedResult;
    
    DataTreeNodeAddress parentAddress = parentEntry ? 
        parentEntry->address : DataTreeNodeAddress();
    
    auto mappedAddress = _state.mapToCurrent(parentAddress);
    
    if (Q_LIKELY(mappedAddress))
    {
        if (parentEntry && !parentEntry->directHandle)
        {
            parentEntry->directHandle = aux_datatree::node_lookup_address(
                    _root,
                    *mappedAddress
                );
        }
        
        const handle_t& node = parentEntry ? parentEntry->directHandle : _root;
        
        return (cachedResult = aux_datatree::node_child_count(node));
    }
    else
    {
        // The node identified by `parent` has been deleted by a pending 
        // DataTreeNodeEvent and no cached child count exists for it. This
        // is unusual but not unavoidable and may be caused by an awkwardly
        // timed UI event and/or full cache.
        // 
        // There's no reliable and efficient way to reverse engineer the
        // node's child count from the current state. The best we can do is
        // find the smallest value that won't cause glitches, i.e., that does
        // not contradict the existence of any existing persistent indices.
        //
        // If the pending DataTreeNodeEvent that deleted this node is processed 
        // within the current cycle of the UI thread's event loop, this should 
        // not result in any noticeable side-effects.
        
        std::size_t result = 0;
        for (QModelIndex index : noDetach( 
            static_cast<const ItemModel*>(this)->persistentIndexList() ))
        {
            const DataTreeNodeAddress& address 
                = static_cast<const CacheEntry*>(index.internalPointer())
                ->address;
            
            if (parentAddress.isAncestorOf(address))
                result = std::max(result, address[parentAddress.size()]);
        }
        
        return result; // this number is unscrupulous. do not cache it.
    }
}

template<class ItemModel, class Tree>
typename ObservedTreeItemModelHelperBase<ItemModel, Tree>::handle_t 
ObservedTreeItemModelHelperBase<ItemModel, Tree>::getNode(
        const QModelIndex& index) const
{
    auto lock = _state.lockObserverForRead();
    
    CacheEntry* entry = static_cast<CacheEntry*>(index.internalPointer());
    
    if (entry && entry->transientIt != _transientCache.end())
    {
        _transientCache.splice(
                _transientCache.begin(), 
                _transientCache, 
                entry->transientIt
            );
    }
    
    if (entry && entry->directHandle)
        return entry->directHandle;
    
    DataTreeNodeAddress address = entry ?
        entry->address : DataTreeNodeAddress();
    
    auto mappedAddress = _state.mapToCurrent(address);
        
    if (!_state.isCurrent() && !mappedAddress)
    {
        return {};
    }
    
    if (!entry) 
        return _root;
    else
        return (
            entry->directHandle 
                = aux_datatree::node_lookup_address(_root, address)
        );
}

template<class ItemModel, class Tree>
QModelIndex ObservedTreeItemModelHelperBase<ItemModel, Tree>::getIndex(
        DataTreeNodeAddress address) const
{
    if (address.isRoot())
        return QModelIndex();
    
    auto lock = _state.lockObserverForRead();
    
    CacheEntry* entry;
    
    auto find = _locationCache.find(address);
    if (find != _locationCache.end())
    {
        entry = (*find).second;
        
        if (entry->transientIt != _transientCache.end())
        {
            _transientCache.splice(
                    _transientCache.begin(), 
                    _transientCache, 
                    entry->transientIt
                );
        }
    }
    else
    {
        entry = new CacheEntry { address };
        entry->transientIt = _transientCache.insert(
                _transientCache.begin(), 
                entry
            );
        _locationCache.insert(std::make_pair(address, entry));
        
        pruneTransientCache();
    }
    
    return static_cast<const ItemModel*>(this)
        ->createIndex(address.lastIndex(), 0, entry);
}

template<class ItemModel, class Tree>
void ObservedTreeItemModelHelperBase<ItemModel, Tree>::onNodesChanged(
        DataTreeNodeEvent event)
{
    auto lock = _state.lockObserverForRead();
    assert(_state.event() == event);
    
    emit static_cast<ItemModel*>(this)->layoutAboutToBeChanged();
    
    persistent_cache_t persistentsToKeep;
    
    const auto fromPersistentIndices = 
        static_cast<ItemModel*>(this)->persistentIndexList();
        
    QModelIndexList toPersistentIndices;
    toPersistentIndices.reserve(fromPersistentIndices.size());
    
    _locationCache.clear();
    
    for (QModelIndex index : fromPersistentIndices)
    {
        CacheEntry* entry = static_cast<CacheEntry*>(index.internalPointer());
        assert(entry);
            
        auto address = event.mapForward(entry->address);
        if (!address)
        {
            toPersistentIndices.append(QModelIndex());
            continue;
        }
        
        persistentsToKeep.insert(entry);
        entry->address = std::move(*address);
        
        // move entry from transient cache to persistent cache
        if (entry->transientIt != _transientCache.end())
        {
            _persistentCache.insert(entry);
            _transientCache.erase(entry->transientIt);
            entry->transientIt = _transientCache.end();
        }
        
        toPersistentIndices.append(
            static_cast<ItemModel*>(this)->createIndex(
                entry->address.lastIndex(),
                0,
                entry
            ));
    }
    
    for (CacheEntry* entry : _transientCache)
        delete entry;
    
    _transientCache.clear();
    
    _cachedRootChildCount = SIZE_MAX;
    
    // Clean up persistent cache
    for (CacheEntry* entry : _persistentCache)
    {
        if (persistentsToKeep.find(entry) == persistentsToKeep.end())
        {
            delete entry;
        }
        else
        {
            entry->childCount = SIZE_MAX;
            //entry->directHandle = {}; 
            _locationCache.insert(std::make_pair(entry->address, entry));
        }
    }
    _persistentCache = std::move(persistentsToKeep);
    
    static_cast<ItemModel*>(this)->changePersistentIndexList(
            fromPersistentIndices,
            toPersistentIndices
        );

    _state.next();
    
    emit static_cast<ItemModel*>(this)->layoutChanged();
}

template<class ItemModel, class Tree>
void ObservedTreeItemModelHelperBase<ItemModel, Tree>::pruneTransientCache() const
{
    if (_transientCache.size() <= _maxCacheSize) 
        return;
        
    const auto persistentIndices = static_cast<const ItemModel*>(this)->persistentIndexList();
    
    for (const auto& index : persistentIndices)
    {
        CacheEntry* entry = static_cast<CacheEntry*>(index.internalPointer());
        
        if (entry->transientIt != _transientCache.cend())
        {
            _persistentCache.insert(entry);
            _transientCache.erase(entry->transientIt);
            entry->transientIt = _transientCache.end();
        }
    }
        
    // To amortize the cost of iterating the persistent index list, we'll prune
    // to smaller than the max size, but always leave at least one entry (since 
    // it was presumably just created and the calling function will assume it 
    // still exists).
    const std::size_t pruneSize = std::max(
            (std::size_t)(_maxCacheSize - persistentIndices.size()), 
            (std::size_t)1
        );
    
    while (_transientCache.size() > pruneSize)
    {
        CacheEntry* entry = _transientCache.back();
    
        assert( _locationCache.erase(entry->address) );
        delete entry;
        
        _transientCache.pop_back();
    }
}

} // namespace Addle
