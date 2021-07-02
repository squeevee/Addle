#include "./addledatatree.hpp"

using namespace Addle::aux_datatree;

AddleDataTree_NodeBase::~AddleDataTree_NodeBase() noexcept
{
    using namespace boost::adaptors;
    
    if (Q_UNLIKELY(!_treeData)) return; // end sentinel
    
    if (_nodeRefData)
    {
        assert(_treeData);
        
        const QMutexLocker nodeRefLock(&_treeData->_nodeRefMutex);
        _nodeRefData->_node.storeRelaxed(nullptr);
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
    assert(_treeData);
    const QMutexLocker lock(&_treeData->_cacheMutex);
    
    auto find = _treeData->_addressCacheByNode.find(this);
    if (find != _treeData->_addressCacheByNode.end())
    {
        _treeData->_addressCacheQueue.splice(
                _treeData->_addressCacheQueue.begin(),
                _treeData->_addressCacheQueue,
                (*find).second
            );
        
        return (*(*find).second).address;
    }
    
    NodeAddress result = aux_datatree::calculate_node_address(
            static_cast<const nodebase_traverse_helper*>(this)
        );
        
    auto i = _treeData->_addressCacheQueue.insert(
            _treeData->_addressCacheQueue.begin(),
            {
                this,
                result
            }
        );
    
    _treeData->_addressCacheByNode.insert( std::make_pair(this, i) );
    _treeData->_addressCache.insert( std::make_pair(result, i) );
    
    _treeData->pruneAddressCache();
    
    return result;
}

const AddleDataTree_NodeBase* AddleDataTree_TreeDataBase::nodeAt(const NodeAddress& address) const
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
    
    const AddleDataTree_NodeBase* result = aux_datatree::node_lookup_address(
            static_cast<const nodebase_traverse_helper*>(_root.get()),
            address
        );
        
    auto i = _addressCacheQueue.insert(
            _addressCacheQueue.begin(),
            {
                result,
                address
            }
        );
    
    _addressCacheByNode.insert( std::make_pair(result, i) );
    _addressCache.insert( std::make_pair(address, i) );
    
    pruneAddressCache();
    
    return result;
}

void AddleDataTree_NodeBase::reindexChildren(std::size_t startIndex)
{
    if (_children.empty())
    {
        assert(startIndex == 0);
        
        if (!_end) _end = findEnd();
        _end->_prev = this;
        return;
    }

    assert(startIndex < _children.size());
    
    auto end = _end ? _end : findEnd();
    
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
        (*i)->_prev = child;
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
    
    _children.erase(begin, end);
    
    reindexChildren(std::distance(_children.cbegin(), begin));
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

template<typename Node_>
Node_* AddleDataTree_NodeBase::findEnd_impl(Node_* node)
{
    static_assert(
        std::is_same_v<std::remove_const_t<Node_>, AddleDataTree_NodeBase>
    );
    
    assert(node);
    
    Node_* parent;
    while ((parent = node->_parent))
    {
        std::size_t nextIndex = node->_index + 1;
        
        if (parent->_children.size() > nextIndex)
            return parent->_children[nextIndex];
        
        node = parent;
    }
    
    return &(node->_treeData->_endSentinel);
}

template const AddleDataTree_NodeBase* AddleDataTree_NodeBase::findEnd_impl<const AddleDataTree_NodeBase>(const AddleDataTree_NodeBase*);
template AddleDataTree_NodeBase* AddleDataTree_NodeBase::findEnd_impl<AddleDataTree_NodeBase>(AddleDataTree_NodeBase*);
