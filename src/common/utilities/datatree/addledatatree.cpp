#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
#include <QtDebug>
#include <QMetaEnum> // for introspection of ValencyHint in error
#endif // defined(ADDLE_DEBUG) || defined(ADDLE_TEST)

#include "./addledatatree.hpp"

using namespace Addle::aux_datatree;

AddleDataTree_NodeBase::~AddleDataTree_NodeBase() noexcept
{
    using namespace boost::adaptors;
    
    if (Q_LIKELY(_nodeRefData))
    {
        _nodeRefData->node.storeRelaxed(nullptr);
        if (!_nodeRefData->refCount.deref())
            delete _nodeRefData;
    }
    
    for (AddleDataTree_NodeBase* child : reverse(_children))
        delete child;
}

// Partial datatree node implementation for calculating node address
struct nodebase_traverse_helper : AddleDataTree_NodeBase
{
    friend const nodebase_traverse_helper* 
        datatree_node_parent(const nodebase_traverse_helper* node)
    {
        return static_cast<const nodebase_traverse_helper*>(node->_parent);
    }
    
    friend std::size_t datatree_node_index(const nodebase_traverse_helper* node)
    {
        return node->_index;
    }   
    
    friend std::size_t datatree_node_depth(const nodebase_traverse_helper* node)
    {
        return node->_depth;
    }
    
    friend std::size_t datatree_node_child_count(const nodebase_traverse_helper* node)
    {
        return node->_children.size();
    }
    
    friend const nodebase_traverse_helper* 
        datatree_node_child_at(const nodebase_traverse_helper* node, std::size_t index)
    {
        return static_cast<const nodebase_traverse_helper*>(node->_children[index]);
    }
};

NodeAddress AddleDataTree_NodeBase::address_impl() const
{
//     assert(_treeData);
//     const QMutexLocker lock(&_treeData->_cacheMutex);
//     
//     auto find = _treeData->_addressCacheByNode.find(this);
//     if (find != _treeData->_addressCacheByNode.end())
//     {
//         _treeData->_addressCacheQueue.splice(
//                 _treeData->_addressCacheQueue.begin(),
//                 _treeData->_addressCacheQueue,
//                 (*find).second
//             );
//         
//         return (*(*find).second).address;
//     }
    
    NodeAddress result = aux_datatree::calculate_node_address(
            static_cast<const nodebase_traverse_helper*>(this)
        );
        
//     auto i = _treeData->_addressCacheQueue.insert(
//             _treeData->_addressCacheQueue.begin(),
//             {
//                 this,
//                 result
//             }
//         );
//     
//     _treeData->_addressCacheByNode.insert( std::make_pair(this, i) );
//     _treeData->_addressCache.insert( std::make_pair(result, i) );
//     
//     _treeData->pruneAddressCache();
    
    return result;
}

template<bool Throws>
const AddleDataTree_NodeBase* AddleDataTree_TreeDataBase::nodeAt(const NodeAddress& address) const noexcept(!Throws)
{
    const QMutexLocker lock(&_cacheMutex);
    
    auto find = _addressCache.find(address);
    if (find != _addressCache.end())
    {
        _addressCacheQueue.splice(
                _addressCacheQueue.begin(),
                _addressCacheQueue,
                (*find).second
            );
        
        return (*(*find).second).node;
    }
    
    auto result = static_cast<const AddleDataTree_NodeBase*>(
            aux_datatree::node_lookup_address(
                static_cast<const nodebase_traverse_helper*>(_root.get()),
                address
            )
        );
    
    if (Q_LIKELY(result))
    {
        auto i = _addressCacheQueue.insert(
                _addressCacheQueue.begin(), 
                { result, address }
            );
        
        _addressCacheByNode.insert( std::make_pair(result, i) );
        _addressCache.insert( std::make_pair(address, i) );
        
        pruneAddressCache();
    }
    else if constexpr (Throws)
    {
        QString addressString;
#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
        QDebug(&addressString) << address;
#endif
        throw std::out_of_range(
                //% "Node at %1 not found."
                qtTrId("debug-messages.addledatatree-nodeat-out-of-range")
                    .arg(addressString)
                    .toStdString()
            );
    }
    
    return result;
}

template const AddleDataTree_NodeBase* AddleDataTree_TreeDataBase::nodeAt<true>(const NodeAddress&) const;
template const AddleDataTree_NodeBase* AddleDataTree_TreeDataBase::nodeAt<false>(const NodeAddress&) const noexcept;

void AddleDataTree_NodeBase::reindexChildren(std::size_t startIndex)
{
    if (_children.empty())
    {
        assert(startIndex == 0);
        
        if (!_end) _end = findEnd();
        _end->_prev = this;
        return;
    }

    auto end = _end ? _end : findEnd();
    
    if (startIndex < _children.size())
    {
        auto i = _children.begin() + startIndex;
        auto&& childrenEnd = _children.end();
        
        if (startIndex > 0)
        {
            AddleDataTree_NodeBase* prevChild = _children[startIndex - 1];
            (*i)->_prev = prevChild;
            end->_prev->_end = (*i);
        }
        
        const std::size_t depth = _depth + 1;
        std::size_t index = startIndex; 
        
        while (true)
        {
            AddleDataTree_NodeBase* child = *i;
            
            child->_parent = this;
            child->_index = index;
            child->_depth = depth;
            child->_treeData = _treeData;
        
            ++index;
            ++i;
            
            if (i == childrenEnd) break;
            
            child->_end = *i;
            
            if ((*i)->_prev)
                (*i)->_prev->_end = (*i)->_prev->findEnd();
            (*i)->_prev = child;
        }
    }
    
    if (_children.back()->_children.empty())
    {
        _children.back()->_end = end;
        end->_prev = _children.back();
    }
    
    if (Q_LIKELY(_parent))
        // `_end` is unset in branches below root to avoid stale linkage. 
        _end = nullptr;
}

void AddleDataTree_NodeBase::removeChildren_impl(inner_iterator_t begin, inner_iterator_t end)
{
    if (begin == end) return;
    
    std::size_t deficit = 0;
    for (auto i = begin; i != end; ++i)
    {
        deficit += 1 + (*i)->_descendantCount;
    }
    
    AddleDataTree_NodeBase* cursor = this;
    do
    {
        cursor->_descendantCount -= deficit;
        cursor = cursor->_parent;
    } while (cursor);

    _treeData->invalidateAddressCache(*begin);
    
    {
        auto&& rend = std::make_reverse_iterator(begin);
        for (auto j = std::make_reverse_iterator(end); j != rend; ++j)
            delete (*j);
    }
    
    std::size_t reindex = std::distance(_children.cbegin(), begin);
    _children.erase(begin, end);
    
    reindexChildren(reindex);
}

void AddleDataTree_TreeDataBase::invalidateAddressCache(const AddleDataTree_NodeBase* from) const
{
    const QMutexLocker lock(&_cacheMutex);
    if (_addressCache.empty()) return;
    
    DataTreeNodeAddress address;
        
    auto findFrom = _addressCacheByNode.find(from);
    if (findFrom != _addressCacheByNode.end())
    {
        address = (*(*findFrom).second).address;
    }
    else
    {
        address = ::Addle::aux_datatree::calculate_node_address(
                static_cast<const nodebase_traverse_helper*>(from)
            );
    }
    
    auto i = _addressCache.lower_bound(address);
    while (i != _addressCache.end())
    {
        auto j = (*i).second;
        
        auto ancestor = address.commonAncestorIndex((*j).address);
        if (
            ancestor != address.selfIndex() 
            && ancestor != address.parentIndex()
        )
        {
            break;
        }
        
        auto findPeer = _addressCacheByNode.find((*j).node);
        assert (findPeer != _addressCacheByNode.end());
        
        _addressCacheByNode.erase(findPeer);
        i = _addressCache.erase(i);
        _addressCacheQueue.erase(j);
    }
}

template<bool IsConst>
AddleDataTree_NodeBase::_ptr_t<IsConst> AddleDataTree_NodeBase::findEnd_impl(_ptr_t<IsConst> node)
{
    assert(node);
    _ptr_t<IsConst> parent;
    while ((parent = node->_parent))
    {
        std::size_t nextIndex = node->_index + 1;
        
        if (parent->_children.size() > nextIndex)
            return parent->_children[nextIndex];
        
        node = parent;
    }
    
    return &(node->_treeData->_endSentinel);
}

template const AddleDataTree_NodeBase* AddleDataTree_NodeBase::findEnd_impl<true>(const AddleDataTree_NodeBase*);
template AddleDataTree_NodeBase* AddleDataTree_NodeBase::findEnd_impl<false>(AddleDataTree_NodeBase*);

void AddleDataTree_NodeBase::throw_childOutOfRange(std::size_t index) const
{
    throw std::out_of_range(
            //% "Attempted out-of-range access of child node.\n"
            //% "index: %1\n"
            //% "range: [0, %2)"
            qtTrId("debug-messages.addledatatree-child-out-of-range")
                .arg(index)
                .arg(_children.size())
                .toStdString()
        );
}

void AddleDataTree_NodeBase::throw_insertPosOutOfRange(std::size_t pos) const
{
    throw std::out_of_range(
            //% "Attempted insert to out-of-range child node "
            //% "position.\n"
            //% "  pos: %1\n"
            //% "range: [0, %2]"
            qtTrId("debug-messages.addledatatree-insert-pos-out-of-range")
                .arg(pos)
                .arg(_children.size())
                .toStdString()
        );
}

void AddleDataTree_NodeBase::throw_removeIndicesOutOfRange(std::size_t pos, std::size_t count) const
{
    throw std::out_of_range(
            //% "Attempted to remove out-of-range child nodes.\n"
            //% "  pos: %1\n"
            //% "count: %2\n"
            //% "range: [0, %3)"
            qtTrId("debug-messages.addledatatree-remove-pos-out-of-range")
                .arg(pos)
                .arg(count)
                .arg(_children.size())
                .toStdString()
        );
}
    
ValencyHintViolationError::ValencyHintViolationError(
        Why why, 
        ValencyHint valencyHint, 
        NodeAddress address
    )
    : std::logic_error([&]() -> std::string {
        QString addressString;
        QString valencyHintString;
#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
        QDebug(&addressString) << address;
        valencyHintString = QString(
                QMetaEnum::fromType<ValencyHint>().key((int)valencyHint)
            );
#else
        valencyHintString = QString::number((int)valencyHint);
#endif // defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
        
        switch(why)
        {
            case ChildrenInLeaf:
                //% "A node with \"leaf\" valency hint is not allowed to have "
                //% "children.\n"
                //% "      address: %1\n"
                //% "  valencyHint: %2"
                return qtTrId("debug-messages.addledatatree-add-children-to-leaf")
                    .arg(addressString)
                    .arg(valencyHintString)
                    .toStdString();
                
            case InappropriateValue:
                //% "Inappropriate value for this node's valency hint."
                //% "      address: %1\n"
                //% "  valencyHint: %2"
                return qtTrId("debug-messages.addledatatree-valency-hint-inappropriate-value")
                    .arg(addressString)
                    .arg(valencyHintString)
                    .toStdString();
                
            default:
                Q_UNREACHABLE();
        }
    }()),
    _why(why),
    _valencyHint(valencyHint),
    _address(address)
{
}
