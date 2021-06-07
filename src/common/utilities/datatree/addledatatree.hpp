/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#pragma once

#include <algorithm>    // lexicographical_compare
#include <cstring>      // memcpy
#include <cmath>        // sqrt
#include <exception>
#include <iterator>
#include <optional>
#include <variant>
#include <vector>

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>

#include <boost/range.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <boost/mp11.hpp>
#include <boost/type_traits.hpp>

#include <QtDebug>

#include "./aux.hpp"
#include "utilities/metaprogramming.hpp"
#include "utilities/ranges.hpp"

namespace Addle {
namespace aux_datatree {
    
class AddleDataTree_TreeDataBase;
    
class AddleDataTree_NodeBase
{
public:
    virtual ~AddleDataTree_NodeBase() noexcept;
    
    using inner_child_container_t = std::vector<AddleDataTree_NodeBase*>; 
    using inner_iterator_t = typename inner_child_container_t::const_iterator;

protected:
    inline AddleDataTree_NodeBase() = default;
    
    NodeAddress address_impl() const;
    void reindexChildren(std::size_t startIndex = 0);
    
    const AddleDataTree_NodeBase* findEnd() const { return findEnd_impl(this); }
    AddleDataTree_NodeBase* findEnd() { return findEnd_impl(this); }
    
    template<typename Node_>
    static Node_* findEnd_impl(Node_*);
        
    inner_child_container_t _children;
    
    AddleDataTree_NodeBase* _parent = nullptr;
    
    std::size_t _index = 0;
    std::size_t _depth = 0;
    std::size_t _descendantCount = 0;
    
    AddleDataTree_NodeBase* _prev = nullptr;
    AddleDataTree_NodeBase* _end = nullptr;
    
    AddleDataTree_TreeDataBase* _treeData = nullptr;
    
    friend class AddleDataTree_TreeDataBase;
};

class AddleDataTree_TreeDataBase
{
public:
    virtual ~AddleDataTree_TreeDataBase() = default;
    
protected:
    AddleDataTree_TreeDataBase(AddleDataTree_NodeBase* root)
        : _root(root)
    {
        _root->_treeData = this;
        _root->_end = &_endSentinel;
        
        _endSentinel._prev = _root.get();
    }
    
    using inner_node_iterator_t = AddleDataTree_NodeBase::inner_iterator_t;
    
    const AddleDataTree_NodeBase* nodeAt(const NodeAddress& address) const;
    void invalidateAddressCache(const AddleDataTree_NodeBase* from) const;
        
    void updateMaxCacheAddressSize()
    {
        // ???
        _maxAddressCacheSize = std::max(
                (unsigned) (2 * std::sqrt(_root->_descendantCount)), 
                16U
            );
        pruneAddressCache();
    }
    
    void pruneAddressCache() const
    {
        while (_addressCacheQueue.size() > _maxAddressCacheSize)
        {
            auto& entry = _addressCacheQueue.back();
            _addressCache.erase(entry.address);
            _addressCacheByNode.erase(entry.node);
            
            _addressCacheQueue.pop_back();
        }
    }
    
    std::unique_ptr<AddleDataTree_NodeBase> _root;
    AddleDataTree_NodeBase _endSentinel;
    
    unsigned _maxAddressCacheSize = 64;
    
    struct AddressCacheEntry
    {
        const AddleDataTree_NodeBase* node;
        DataTreeNodeAddress address;
    };
    using address_cache_queue_t = std::list<AddressCacheEntry>;
    
    mutable address_cache_queue_t _addressCacheQueue;
    mutable std::map<
            DataTreeNodeAddress, 
            typename address_cache_queue_t::const_iterator> _addressCache;
    mutable std::unordered_map<
            const AddleDataTree_NodeBase*, 
            typename address_cache_queue_t::const_iterator> _addressCacheByNode;
    
    friend class AddleDataTree_NodeBase;
};

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

} // namespace aux_datatree
 
template<typename T>
class AddleDataTree
{
    struct Data;
public:
    class Node : private aux_datatree::AddleDataTree_NodeBase
    {
    public:
        using value_type        = Node;
        using reference         = Node&;
        using const_reference   = const Node&;
        using iterator          = aux_datatree::NodeIterator<AddleDataTree<T>>;
        using const_iterator    = aux_datatree::ConstNodeIterator<AddleDataTree<T>>;
        using difference_type   = std::ptrdiff_t;
        using size_type         = std::size_t;
        
        using child_iterator        = aux_datatree::ChildNodeIterator<AddleDataTree<T>>;
        using const_child_iterator  = aux_datatree::ConstChildNodeIterator<AddleDataTree<T>>;
        
        using child_range       = aux_datatree::ChildNodeRange<AddleDataTree<T>>;
        using const_child_range = aux_datatree::ConstChildNodeRange<AddleDataTree<T>>;
        
        using descendant_range          = aux_datatree::NodeRange<AddleDataTree<T>>;
        using const_descendant_range    = aux_datatree::ConstNodeRange<AddleDataTree<T>>;
        
        using ancestor_range        = aux_datatree::AncestorNodeRange<AddleDataTree<T>>;
        using const_ancestor_range  = aux_datatree::ConstAncestorNodeRange<AddleDataTree<T>>;
        
//         using leaf_range            = aux_datatree::LeafRange<AddleDataTree<T>>;
//         using const_leaf_range      = aux_datatree::ConstLeafRange<AddleDataTree<T>>;
//         
//         using branch_range          = aux_datatree::BranchRange<AddleDataTree<T>>;
//         using const_branch_range    = aux_datatree::ConstBranchRange<AddleDataTree<T>>;
        
        Node(const Node&)               = delete;
        Node(Node&&)                    = delete;
        
        Node& operator=(const Node&)    = delete;
        Node& operator=(Node&&)         = delete;
        
        virtual ~Node() = default;
        
        inline const Node* parent() const { return static_cast<const Node*>(_parent); }
        inline Node* parent() { return static_cast<Node*>(_parent); }
        
        inline std::size_t depth() const { return _depth; }
        inline std::size_t index() const { return _index; }
        
        DataTreeNodeAddress address() const { return this->address_impl(); }
        
        Node& root()
        { 
            return static_cast<Node&>(
                    *static_cast<Data*>(_treeData)->_root
                ); 
        }
        
        const Node& root() const 
        { 
            return static_cast<const Node&>(
                    *static_cast<Data*>(_treeData)->_root
                ); 
        }
        
        inline iterator begin() { return iterator(this); }
        inline const_iterator begin() const { return const_iterator(this); }
        inline const_iterator cbegin() const { return begin(); }
        
        inline iterator end()
        {
            return iterator(static_cast<Node*>(
                    Q_LIKELY(_end) ? _end : findEnd()
                ));
        }
        inline const_iterator end() const
        { 
            return const_iterator(static_cast<const Node*>(
                    Q_LIKELY(_end) ? _end : findEnd()
                ));
        }
        inline const_iterator cend() const { return end(); }
        
        inline std::size_t size() const { return _descendantCount + 1; }
        
        inline bool isLeaf() const { return _children.empty(); }
        inline bool isBranch() const { return !_children.empty(); }

        inline const T& value() const & { return _value; }
        inline T& value() & { return _value; }
        inline T&& value() && { return std::move(_value); }
        
        template<typename U>
        inline void setValue(U&& value)
        {
            _value = std::forward<U>(value);
        }
        
        template<typename U>
        inline Node& operator= (U&& value)
        {
            setValue(std::forward<U>(value));
            return *this;
        }
        
        inline operator const T& () const { return _value; }
        inline operator T& () & { return _value; }
        inline operator T&& () && { return std::move(_value); }
        
        inline std::size_t countChildren() const { return _children.size(); }
        
        inline child_range children() 
        { 
            return child_range(_children.begin(), _children.end());
        }
        
        inline const_child_range children() const 
        { 
            return const_child_range(_children.begin(), _children.end());
        }
                
        inline Node& childAt(std::size_t index)
        {
            assert(_children.size() > index); // TODO: exception
                
            return *(_children[index]);
        }
        
        inline const Node& childAt(std::size_t index) const
        {
            assert(_children.size() > index); // TODO: exception
            
            return *(_children[index]);
        }
        
        inline Node& descendantAt(DataTreeNodeAddress address)
        {
            if (!_parent)
            {
                auto node = static_cast<Data*>(_treeData)->nodeAt(address);
                return *const_cast<Node*>(static_cast<const Node*>(node));
            }
            else
            {
                return *::Addle::aux_datatree::node_lookup_address(
                        this,
                        address
                    );
            }
        }
        
        inline const Node& descendantAt(DataTreeNodeAddress address) const
        {
            if (!_parent)
            {
                return *static_cast<const Node*>(
                        static_cast<Data*>(_treeData)->nodeAt(address)                    
                    );
            }
            else
            {
                return *::Addle::aux_datatree::node_lookup_address(
                        this,
                        address
                    );
            }
        }
        
        inline Node& operator[](std::size_t index)
        {
            return childAt(index);
        }
        
        inline const Node& operator[](std::size_t index) const
        {
            return childAt(index);
        }
        
        inline descendant_range descendants()
        {
            if (!_children.empty())
            {
                return descendant_range(_children.front(), _end);
            }
            else
            {
                return descendant_range();
            }
        }
        
        inline const_descendant_range descendants() const
        {
            if (!_children.empty())
            {
                return descendant_range(_children.front(), _end);
            }
            else
            {
                return descendant_range();
            }
        }
        
        inline ancestor_range ancestors() { return ancestor_range( _parent, (Node*) nullptr ); }
        inline const_ancestor_range ancestors() const { return const_ancestor_range( (const Node*) _parent, (const Node*) nullptr ); }
        
        inline ancestor_range ancestorsAndSelf() { return ancestor_range( this, (Node*) nullptr ); }
        inline const_ancestor_range ancestorsAndSelf() const { return const_ancestor_range( this, (const Node*) nullptr ); }
        
//         inline leaf_range leaves()
//         { 
//             using leaf_iterator_t = typename leaf_range::iterator;
//             return leaf_range(
//                     leaf_iterator_t ( this, _end ), leaf_iterator_t ( _end, _end )
//                 );
//         }
//         
//         inline const_leaf_range leaves() const
//         { 
//             using leaf_iterator_t = typename const_leaf_range::iterator;
//             return const_leaf_range(
//                     leaf_iterator_t ( this, _end ), leaf_iterator_t ( _end, _end )
//                 );
//         }
//         
//         
//         inline branch_range branches()
//         { 
//             using branch_iterator_t = typename branch_range::iterator;
//             return branch_range(
//                     branch_iterator_t ( this, _end ), branch_iterator_t ( _end, _end )
//                 );
//         }
//         
//         inline const_branch_range branches() const
//         { 
//             using branch_iterator_t = typename const_branch_range::iterator;
//             return const_branch_range(
//                     branch_iterator_t ( this, _end ), branch_iterator_t ( _end, _end )
//                 );
//         }
        
        template<typename U = T>
        inline Node& appendChild(U&& branchValue = {})
        {
            return *appendChild_impl(std::forward<U>(branchValue));
        }

        template<typename U = T>
        inline child_iterator insertChild(const_child_iterator pos, U&& branchValue = {})
        {
            return insertChild_impl(pos, std::forward<U>(branchValue));
        }
    
        template<typename U = T>
        inline Node& insertChild(std::size_t pos, U&& branchValue = {})
        {
            return *insertChild_impl(_children.begin() + pos, std::forward<U>(branchValue));
        }
        
        template<typename Range>
        inline child_range appendChildren(Range&& childValues)
        {
            return appendChildren_impl(std::forward<Range>(childValues));
        }
        
        template<typename U>
        inline child_range appendChildren(const std::initializer_list<U>& childValues)
        {
            return appendChildren_impl(childValues);
        }
        
        template<typename Range>
        inline child_range insertChildren(const_child_iterator pos, Range&& childValues)
        {
            return insertChildren_impl(
                    pos,
                    std::forward<Range>(childValues)
                );
        }
        
        template<typename Range>
        inline child_range insertChildren(std::size_t pos, Range&& childValues)
        {
            return insertChildren_impl(
                    _children.begin() + pos,
                    std::forward<Range>(childValues)
                );
        }
        
        template<typename U>
        inline child_range insertChildren(const_child_iterator pos, const std::initializer_list<U>& childValues)
        {
            return insertChildren_impl(
                    pos,
                    childValues
                );
        }
        
        template<typename U>
        inline child_range insertChildren(std::size_t pos, const std::initializer_list<U>& childValues)
        {
            return insertChildren_impl(
                    _children.begin() + pos,
                    childValues
                );
        }

        child_iterator removeChildren(child_range children)
        {
            return removeChildren_impl(children);
        }
        
        void removeChildren(std::size_t pos, std::size_t count)
        {
            auto&& begin    = _children.begin() + pos;
            auto&& end      = _children.begin() + pos + count;
            
            // TODO error check?
            removeChildren_impl(
                    child_range(
                        begin, 
                        end 
                    )
                );  
        }
        
        void removeDescendant(DataTreeNodeAddress address)
        {
            assert(!address.isRoot());
            
            descendantAt(address.parent()).removeChildren(address.lastIndex(), 1);
        }
    
    protected:
        explicit inline Node(T&& value = {})
            : _value(std::move(value))
        {
        }
        
        explicit inline Node(const T& value)
            : _value(value)
        {
        }
        
    private:
        
        template<typename Range>
        child_range insertChildren_impl(const_child_iterator pos, Range&& childValues);
        
        template<typename Range>
        child_range appendChildren_impl( Range&& childValues )
        {
            return insertChildren_impl( _children.cend(), std::forward<Range>(childValues) );
        }
        
        child_iterator insertChild_impl( const_child_iterator pos, const T& v )
        {
            child_range result = insertChildren_impl( pos, make_inline_ref_range(v) );
            return result.begin();
        }
                
        child_iterator insertChild_impl( const_child_iterator pos, T&& v )
        {
            child_range result = insertChildren_impl( pos, make_inline_ref_range(std::move(v)) );
            return result.begin();
        }
        
        child_iterator appendChild_impl( const T& v )
        {
            child_range result = appendChildren_impl( make_inline_ref_range(v) );
            return result.begin();
        }
        
        child_iterator appendChild_impl( T&& v )
        {
            child_range result = appendChildren_impl( make_inline_ref_range(std::move(v)) );
            return result.begin();
        }
        
        child_iterator removeChildren_impl( child_range removals );
                
        T _value;
        
        friend class AddleDataTree<T>;
        friend struct AddleDataTree<T>::Data;
        
        friend class aux_datatree::ChildNodeIterator<AddleDataTree<T>, true>;
        friend class aux_datatree::ChildNodeIterator<AddleDataTree<T>, false>;
        
        friend Node* datatree_node_parent(Node* node) 
        { 
            return Q_LIKELY(node) ? static_cast<Node*>(node->_parent) : nullptr; 
        }
        friend const Node* datatree_node_parent(const Node* node)
        { 
            return Q_LIKELY(node) ? static_cast<const Node*>(node->_parent) : nullptr; 
        }
        
        friend std::size_t datatree_node_index(const Node* node) { return Q_LIKELY(node) ? node->_index : 0; } 
        friend std::size_t datatree_node_depth(const Node* node) { return Q_LIKELY(node) ? node->_depth : 0; } 
        
        friend Node* datatree_node_root(Node* node)
        { 
            return Q_LIKELY(node) ? std::addressof(node->root()) : nullptr; 
        }
        friend const Node* datatree_node_root(const Node* node) 
        { 
            return Q_LIKELY(node) ? std::addressof(node->root()) : nullptr; 
        }
        
        friend T& datatree_node_value(Node* node)
        {
            assert(node); return node->value();
        }
        
        friend const T& datatree_node_value(const Node* node)
        {
            assert(node); return node->value();
        }
        
        template<typename U>
        friend void datatree_node_set_value(Node* node, U&& v)
        {
            if (Q_LIKELY(node)) node->setValue(std::forward<U>(v));
        }
    
        friend std::size_t datatree_node_child_count(const Node* node)
        { 
            return Q_LIKELY(node) ? node->countChildren() : 0;
        }
        
        friend aux_datatree::ConstChildNodeIterator<AddleDataTree<T>> datatree_node_children_begin(const Node* node)
        {
            using iterator_t = aux_datatree::ConstChildNodeIterator<AddleDataTree<T>>;
            return Q_LIKELY(node) ? iterator_t ( node->_children.begin() ) : iterator_t {};
        }
        
        friend aux_datatree::ChildNodeIterator<AddleDataTree<T>> datatree_node_children_begin(Node* node)
        {
            using iterator_t = aux_datatree::ChildNodeIterator<AddleDataTree<T>>;
            return Q_LIKELY(node) ? iterator_t ( node->_children.begin() ) : iterator_t {};
        }
        
        friend aux_datatree::ConstChildNodeIterator<AddleDataTree<T>> datatree_node_children_end(const Node* node)
        {
            using iterator_t = aux_datatree::ConstChildNodeIterator<AddleDataTree<T>>;
            return Q_LIKELY(node) ? iterator_t ( node->_children.end() ) : iterator_t {};
        }
        
        friend aux_datatree::ChildNodeIterator<AddleDataTree<T>> datatree_node_children_end(Node* node)
        {
            using iterator_t = aux_datatree::ChildNodeIterator<AddleDataTree<T>>;
            return Q_LIKELY(node) ? iterator_t ( node->_children.end() ) : iterator_t {};
        }
                
        template<typename NodePointer>
        static inline NodePointer node_child_at_impl(NodePointer node, std::size_t index)
        {
            static_assert( 
                std::is_same<std::remove_const_t<std::remove_pointer_t<NodePointer>>, Node>::value 
            );
            
            if (Q_UNLIKELY(!node || index >= node->_children.size()))
                return nullptr;
            else
                return static_cast<NodePointer>(node->_children[index]);
        }
        
        friend Node* datatree_node_child_at(Node* node, std::size_t index) { return node_child_at_impl(node, index); }
        friend const Node* datatree_node_child_at(const Node* node, std::size_t index) { return node_child_at_impl(node, index); }
        
        friend Node* datatree_node_dfs_end(Node* node)
        { 
            return Q_LIKELY(node) ? static_cast<Node*>(node->end()) : nullptr;
        }
        
        friend const Node* datatree_node_dfs_end(const Node* node)
        { 
            return Q_LIKELY(node) ? static_cast<const Node*>(node->end()) : nullptr;
        }
        
        friend Node* datatree_node_dfs_next(Node* node)
        { 
            if (Q_UNLIKELY(!node)) return nullptr;
            
            if (!node->_children.empty())
                return static_cast<Node*>(node->_children.front());
            else if (node->_end)
                return static_cast<Node*>(node->_end);
            else
                return static_cast<Node*>(node->findEnd());
        }
        
        friend const Node* datatree_node_dfs_next(const Node* node)
        {
            if (Q_UNLIKELY(!node)) return nullptr;
            
            if (!node->_children.empty())
                return static_cast<const Node*>(node->_children.front());
            else if (node->_end)
                return static_cast<const Node*>(node->_end);
            else
                return static_cast<const Node*>(node->findEnd());
        }
        
//         friend bool datatree_node_is_branch(const Node* node)
//         {
//             return node && node->isBranch();
//         }
//         
//         friend bool datatree_node_is_leaf(const Node* node)
//         {
//             return node && node->isLeaf();
//         }
        
        friend aux_datatree::NodeAddress datatree_node_address(const Node* node)
        {
            return node ? node->address() : aux_datatree::NodeAddress();
        }
        
        template<typename Range>
        friend child_iterator datatree_node_insert_children(
                Node* parent, 
                child_iterator pos, 
                Range&& childValues
            )
        {
            if (Q_LIKELY(parent))
                return parent->insertChildren(pos, std::forward<Range>(childValues)).end();
            else
                return child_iterator {};
        }
        
        friend child_iterator datatree_node_remove_children(
                Node* parent, 
                child_iterator begin,
                child_iterator end 
            )
        {
            if (Q_LIKELY(parent))
                return parent->removeChildren(child_range(begin, end));
        }
    };
    
    using value_type        = typename Node::value_type;
    using reference         = typename Node::reference;
    using const_reference   = typename Node::const_reference;
    using iterator          = typename Node::iterator;
    using const_iterator    = typename Node::const_iterator;
    using difference_type   = typename Node::difference_type;
    using size_type         = typename Node::size_type;
    
    using child_range       = typename Node::child_range;
    using const_child_range = typename Node::const_child_range;
    
    using descendant_range          = typename Node::descendant_range;
    using const_descendant_range    = typename Node::const_descendant_range;
    
    using ancestor_range        = typename Node::ancestor_range;
    using const_ancestor_range  = typename Node::const_ancestor_range;
        
//     using leaf_range        = typename Node::leaf_range;
//     using const_leaf_range  = typename Node::const_leaf_range;
//     
//     using branch_range        = typename Node::branch_range;
//     using const_branch_range  = typename Node::const_branch_range;
    
    inline AddleDataTree()
        : _data(new Data)
    {
    }
    
    explicit inline AddleDataTree(const T& v)
        : _data(new Data(v))
    {
    }
    
    explicit inline AddleDataTree(T&& v)
        : _data(new Data(std::move(v)))
    {
    }
    
    ~AddleDataTree() = default;
    
    AddleDataTree(AddleDataTree&&) = default;
    AddleDataTree& operator=(AddleDataTree&&) = default;
    
    AddleDataTree(const AddleDataTree&) = delete;
    AddleDataTree& operator=(const AddleDataTree&) = delete;
    
    inline Node& root() { return static_cast<Node&>(*_data->_root); }
    inline const Node& root() const { return static_cast<const Node&>(*_data->_root); }
    
    inline std::size_t size() const { return _data->root.size(); }
    
    inline iterator begin() { return static_cast<Node&>(*_data->_root).begin(); }
    inline const_iterator begin() const { return _data->root.begin(); }
    inline const_iterator cbegin() const { return _data->root.cbegin(); }
    
    inline iterator end() { return static_cast<Node&>(*_data->_root).end(); }
    inline const_iterator end() const { return _data->root.end(); }
    inline const_iterator cend() const { return _data->root.cend(); }
    
    inline Node& nodeAt(const DataTreeNodeAddress& address)
    {
        const aux_datatree::AddleDataTree_NodeBase* node 
            = _data->nodeAt(address);
        return const_cast<Node&>(static_cast<const Node&>(*node));
    }
    
    inline const Node& nodeAt(const DataTreeNodeAddress& address) const
    {
        return static_cast<const Node&>(*_data->nodeAt(address));
    }
    
//     inline leaf_range leaves() { return _data->root.leaves(); }
//     inline const_leaf_range leaves() const { return _data->root.leaves(); }
//     
//     inline branch_range branches() { return _data->root.branches(); }
//     inline const_branch_range branches() const { return _data->root.branches(); }
    
private:
    struct Data : private aux_datatree::AddleDataTree_TreeDataBase
    {
        inline Data();
        inline Data(const T&);
        inline Data(T&&);
        
        friend class AddleDataTree<T>;
        friend class AddleDataTree<T>::Node;
    };
    
    friend const Node* datatree_root(const AddleDataTree& tree)
    {
        return &(tree.root());
    }
    
    friend Node* datatree_root(AddleDataTree& tree)
    {
        return &(tree.root());
    }
    
    std::unique_ptr<Data> _data;
};

template<typename T>
template<typename Range>
typename AddleDataTree<T>::Node::child_range AddleDataTree<T>::Node::insertChildren_impl(const_child_iterator pos, Range&& childValues)
{
    using namespace boost::adaptors;
    using U = typename boost::range_value<boost::remove_cv_ref_t<Range>>::type;
    using _use_buffered_insert = boost::mp11::mp_and<
            std::is_convertible<
                typename boost::range_category<boost::remove_cv_ref_t<Range>>::type,
                std::forward_iterator_tag
            >,
            std::is_nothrow_constructible<T, U&&>,
            boost::mp11::mp_bool<noexcept( 
                *std::declval<typename boost::range_iterator<boost::remove_cv_ref_t<Range>>::type>() 
            )>
        >;
        
    static_assert(std::is_constructible_v<T, U&&>);
        
    if (boost::empty(childValues)) 
        return child_range();
    
    std::size_t startIndex = std::distance(_children.cbegin(), pos.base());
    std::size_t count;
    
    if constexpr (!_use_buffered_insert::value)
    {
        count = boost::size(childValues);
        
        _children.insert(pos.base(), count, nullptr);
        
        aux_datatree::AddleDataTree_NodeBase** i = _children.data() + startIndex;
        for (auto&& v : childValues)
        {
            *i = new Node(std::forward<decltype(v)>(v));
            ++i;
        }
    }
    else
    {
        QVarLengthArray<aux_datatree::AddleDataTree_NodeBase*, 256> nodes;
        reserve_for_size_if_forward_range(nodes, childValues);
        
        try 
        {
            for (auto&& v : childValues)
                nodes.append(new Node( std::forward<decltype(v)>(v) ));
        }
        catch(...)
        {
            for (aux_datatree::AddleDataTree_NodeBase* node : reverse(nodes))
                delete node;
            
            throw;
        }
        
        count = nodes.size();
        
        _children.insert(pos.base(), count, nullptr);
            
        std::memcpy(
                _children.data() + startIndex, 
                nodes.data(), 
                nodes.size() * sizeof(Node*)
            );
    }
    
    this->reindexChildren(startIndex);
    static_cast<Data*>(_treeData)->invalidateAddressCache(this);
        
    {
        Node* cursor = this;
        do
        {
            cursor->_descendantCount += count;
            cursor = static_cast<Node*>(cursor->_parent);
        } while (cursor);
    }
        
    return child_range(
            _children.begin() + startIndex,
            _children.begin() + startIndex + count
        );
}


template<typename T>
typename AddleDataTree<T>::Node::child_iterator AddleDataTree<T>::Node::removeChildren_impl(child_range removals)
{
    if (removals.empty())
        return child_iterator();
    
    //assert(branch.children.size() >= startIndex + count); // TODO: exception
    //assert(_treeData);
    
    std::size_t deficit = 0;
    for (const auto& removal : removals)
    {
        deficit += 1 + removal._descendantCount;
    }
    
    Node* cursor = this;
    do
    {
        cursor->_descendantCount -= deficit;
        cursor = static_cast<Node*>(cursor->_parent);
    } while (cursor);

    static_cast<Data*>(_treeData)->invalidateAddressCache(
            static_cast<Node*>(removals.begin())
        );
    
    auto begin = removals.begin().base();
    auto end = removals.end().base();
    
    std::size_t startIndex = std::distance(_children.cbegin(), begin);
    
    {
        auto&& rend = std::make_reverse_iterator(begin);
        for (auto i = std::make_reverse_iterator(end); i != rend; ++i)
            delete *i;
    }
    
    _children.erase(begin, end);
    
    reindexChildren(startIndex);
    
    return child_iterator(_children.begin() + startIndex);
}

template<typename T>
AddleDataTree<T>::Data::Data()
    : aux_datatree::AddleDataTree_TreeDataBase(new Node)
{
}

template<typename T>
AddleDataTree<T>::Data::Data(const T& value)
    : aux_datatree::AddleDataTree_TreeDataBase(new Node(value))
{
}

template<typename T>
AddleDataTree<T>::Data::Data(T&& value)
    : aux_datatree::AddleDataTree_TreeDataBase(new Node(std::move(value)))
{
}

template<typename T>
struct aux_datatree::datatree_traits<AddleDataTree<T>>
{
    using node_handle = typename AddleDataTree<T>::Node*;
    using const_node_handle = const typename AddleDataTree<T>::Node*;
    
    using node_value_type = T;
};

template<typename T, bool IsConst>
class aux_datatree::ChildNodeIterator<AddleDataTree<T>, IsConst>
    : public boost::iterator_adaptor<
        aux_datatree::ChildNodeIterator<AddleDataTree<T>, IsConst>,
        aux_datatree::AddleDataTree_NodeBase::inner_iterator_t,
        boost::mp11::mp_if_c<
            IsConst,
            const typename AddleDataTree<T>::Node,
            typename AddleDataTree<T>::Node
        >
    >
{
    using base_t = aux_datatree::AddleDataTree_NodeBase::inner_iterator_t;
    using adaptor_t = boost::iterator_adaptor<
            aux_datatree::ChildNodeIterator<AddleDataTree<T>, IsConst>,
            aux_datatree::AddleDataTree_NodeBase::inner_iterator_t,
            boost::mp11::mp_if_c<
                IsConst,
                const typename AddleDataTree<T>::Node,
                typename AddleDataTree<T>::Node
            >
        >;
    using node_t = boost::mp11::mp_if_c<
            IsConst,
            const typename AddleDataTree<T>::Node,
            typename AddleDataTree<T>::Node
        >;
    using handle_t = boost::mp11::mp_if_c<
            IsConst,
            const typename AddleDataTree<T>::Node*,
            typename AddleDataTree<T>::Node*
        >;
        
public:
    ChildNodeIterator() = default;
    ChildNodeIterator(const ChildNodeIterator&) = default;
    ChildNodeIterator(ChildNodeIterator&&) = default;
    
//     ChildNodeIterator(handle_t node)
//         : adaptor_t([node]() -> base_t {
//             if (Q_UNLIKELY(!node))
//                 return base_t {};
//             
//             assert(node->_parent);
//             // No situation should result in a child node iterator being 
//             // made for a root node.
//             
//             return std::visit(
//                     typename AddleDataTree<T>::Node::dataVisitor_getChildrenBegin {}, 
//                     node->_parent->_nodeData
//                 ) + node->_index;
//         }())
//     {
//     }
    
    ChildNodeIterator(const base_t& i)
        : adaptor_t(i)
    {
    }
    
    ChildNodeIterator(const adaptor_t& i)
        : adaptor_t(i)
    {
    }
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, ChildNodeIterator<AddleDataTree<T>, false>>::value, 
            void*
        > = nullptr
    >
    ChildNodeIterator(const MutableIterator& i)
        : adaptor_t(i.base())
    {
    }
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, ChildNodeIterator<AddleDataTree<T>, false>>::value,
            void*
        > = nullptr
    >
    ChildNodeIterator(MutableIterator&& i)
        : adaptor_t(std::move(i.base()))
    {
    }
    
    ChildNodeIterator& operator=(const ChildNodeIterator&) = default;
    ChildNodeIterator& operator=(ChildNodeIterator&&) = default;
    
    inline operator handle_t () const 
    { 
        return static_cast<handle_t>(*(this->base())); 
    }
    
private:
    node_t& dereference() const 
    {
        return static_cast<node_t&>(**(this->base()));
    }
    
    friend class ChildNodeIterator<AddleDataTree<T>, !IsConst>;
    friend class boost::iterator_core_access;
};

} // namespace Addle

