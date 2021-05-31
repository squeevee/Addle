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
#include "utilities/collections.hpp"

namespace Addle {
 
template<typename T>
class BasicDataTree
{
    struct Data;
public:
    class Node
    {

    public:
        using child_container_t = std::vector<Node*>; 
            // TODO: Made public temporarily to fix iterator access problem
        
        using value_type        = Node;
        using reference         = Node&;
        using const_reference   = const Node&;
        using iterator          = aux_datatree::NodeIterator<BasicDataTree<T>>;
        using const_iterator    = aux_datatree::ConstNodeIterator<BasicDataTree<T>>;
        using difference_type   = std::ptrdiff_t;
        using size_type         = std::size_t;
        
        using child_iterator        = aux_datatree::ChildNodeIterator<BasicDataTree<T>>;
        using const_child_iterator  = aux_datatree::ConstChildNodeIterator<BasicDataTree<T>>;
        
        using child_range       = aux_datatree::ChildNodeRange<BasicDataTree<T>>;
        using const_child_range = aux_datatree::ConstChildNodeRange<BasicDataTree<T>>;
        
        using descendant_range          = aux_datatree::NodeRange<BasicDataTree<T>>;
        using const_descendant_range    = aux_datatree::ConstNodeRange<BasicDataTree<T>>;
        
        using ancestor_range        = aux_datatree::AncestorNodeRange<BasicDataTree<T>>;
        using const_ancestor_range  = aux_datatree::ConstAncestorNodeRange<BasicDataTree<T>>;
        
        using leaf_range            = aux_datatree::LeafRange<BasicDataTree<T>>;
        using const_leaf_range      = aux_datatree::ConstLeafRange<BasicDataTree<T>>;
        
        using branch_range          = aux_datatree::BranchRange<BasicDataTree<T>>;
        using const_branch_range    = aux_datatree::ConstBranchRange<BasicDataTree<T>>;
        
        Node(const Node&)               = delete;
        Node(Node&&)                    = delete;
        
        Node& operator=(const Node&)    = delete;
        Node& operator=(Node&&)         = delete;
        
        inline ~Node() noexcept
        {
            using namespace boost::adaptors;
            for (Node* child : reverse(_children))
                delete child;
        }
        
        inline Node* parent() const { return _parent; }
        inline std::size_t depth() const { return _depth; }
        inline std::size_t index() const { return _index; }
        
        inline aux_datatree::NodeAddress address() const
        {
            // TODO: efficient address cache
//             if (_treeData && _cacheVersion == _treeData->cacheVersion)
//                 return _address;
//             else
            return _address = ::Addle::aux_datatree::calculate_node_address(this); 
        }
        
        inline Node& root() { return _treeData->root; }
        inline const Node& root() const { return _treeData->root; }
        
        inline iterator begin() { return iterator(this); }
        inline const_iterator cbegin() const { return const_iterator(this); }
        inline const_iterator begin() const { return cbegin(); }
        
        inline iterator end() { return iterator(_end); }
        inline const_iterator cend() const { return const_iterator(_end); }
        inline const_iterator end() const { return cend(); }
        
        inline bool isLeaf() const { return _children.empty(); }
        inline bool isBranch() const { return !_children.empty(); }

        inline const T& value() const { return _value; }
        inline T& value() { return _value; }
        
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
        inline operator T& () && { return std::move(_value); }
        
        inline std::size_t countChildren() const { return _children.size(); }
        
        inline child_range children() 
        { 
            return child_range(_children.begin(), _children.end());
        }
        
        inline const_child_range children() const 
        { 
            return const_child_range(_children.begin(), _children.end());
        }
                
        inline Node& operator[](std::size_t index)
        {
            Node* child = _children[index];
            if (child)
            {
                return *child;
            }
            else
            {
                assert(false); // TODO: exception
                // NOTE this could either be the node is a leaf or the 
                // index is out of range
            }
        }
        
        inline const Node& operator[](std::size_t index) const
        {
            const Node* child = _children[index];
            if (child)
            {
                return *child;
            }
            else
            {
                assert(false); // TODO: exception
            }
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
        inline Node& addChild(U&& branchValue = {})
        {
            return *addChild_impl(std::forward<U>(branchValue));
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
        inline child_range addChildren(Range&& childValues)
        {
            return addChildren_impl(std::forward<Range>(childValues));
        }
        
        template<typename U>
        inline child_range addChildren(const std::initializer_list<U>& childValues)
        {
            return addChildren_impl(childValues);
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
        inline void reindexChildren(std::size_t startIndex, std::size_t newCount)
        {
            if (_children.empty()) return;
            
            {
                const std::size_t depth = _depth + 1; 
                
                std::size_t index = startIndex; 
                
                auto&& end = _children.begin() + startIndex + newCount;
                
                for (auto i = _children.begin() + startIndex; i != end; ++i)
                {
                    Node* child = *i;
                    
                    child->_parent = this;
                    child->_index = index;
                    child->_depth = depth;
                    child->_treeData = _treeData;
                    ++index;
                }
            }
            
            {
                auto i = startIndex > 0 ?
                    _children.begin() + startIndex - 1 :
                    _children.begin() + startIndex;
                auto&& end = _children.end();
                
                while (true)
                {
                    Node* child = *i;
                    
                    ++i;
                    if (i == end)
                        break;
                    
                    child->_end = *i;
                }
            }
            
            Node* lastChild = _children.back();
            lastChild->_end = _end;
        }
        
        template<typename Range>
        child_range insertChildren_impl(const_child_iterator pos, Range&& childValues)
        {
            using namespace boost::adaptors;
            using U = typename boost::range_value<boost::remove_cv_ref_t<Range>>::type;
            static_assert(std::is_constructible<T, U&&>::value);
            
            if (boost::empty(childValues)) return child_range();
            
            std::size_t startIndex = std::distance(_children.cbegin(), pos.base());
            
            if constexpr (
                    aux_range_utils::getting_size_is_cheap<boost::remove_cv_ref_t<Range>>::value &&
                    std::is_nothrow_constructible<T, U&&>::value &&
                    noexcept( *std::declval<typename boost::range_iterator<boost::remove_cv_ref_t<Range>>::type>() )
                )
            {
                const std::size_t count = boost::size(childValues);
                
                _children.insert(pos.base(), count, nullptr);
                
                Node** i = _children.data() + startIndex;
                for (auto&& v : childValues)
                {
                    *i = new Node(std::forward<decltype(v)>(v));
                    ++i;
                }
                
                reindexChildren(startIndex, count);
                
                return child_range(
                        _children.begin() + startIndex,
                        _children.begin() + startIndex + count
                    );
            }
            else
                // One of the following:
                // - Getting the size of `childValues` is not cheap
                // - The constructor `T( U&& )` is potentially throwing (where U 
                // is the range value type of `childValues`)
                // - The operator `*i` is potentially throwing (where i is an
                // iterator of `childValues`)
            {
                QVarLengthArray<Node*, 256> nodes;
                reserve_for_size_if_cheap(nodes, childValues);
                
                try 
                {
                    for (auto&& v : childValues)
                    {
                        nodes.append(new Node(std::forward<decltype(v)>(v)));
                    }
                }
                catch(...)
                {
                    for (Node* node : reverse(nodes))
                        delete node;
                    
                    throw;
                }
                
                _children.insert(pos.base(), nodes.size(), nullptr);
                
                std::memcpy(
                        _children.data() + startIndex, 
                        nodes.data(), 
                        nodes.size() * sizeof(Node*)
                    );
                
                reindexChildren(startIndex, nodes.size());
                
                return child_range(
                        _children.begin() + startIndex,
                        _children.begin() + startIndex + nodes.size()
                    );
            }
        }
        
        template<typename Range>
        child_range addChildren_impl( Range&& childValues )
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
        
        child_iterator addChild_impl( const T& v )
        {
            child_range result = addChildren_impl( make_inline_ref_range(v) );
            return result.begin();
        }
        
        child_iterator addChild_impl( T&& v )
        {
            child_range result = addChildren_impl( make_inline_ref_range(std::move(v)) );
            return result.begin();
        }
        
        child_iterator removeChildren_impl( child_range removals )
        {
            //assert(branch.children.size() >= startIndex + count); // TODO: exception
            assert(_treeData);
            
            auto begin = removals.begin().base();
            auto end = removals.end().base();
            
            std::size_t startIndex = std::distance(
                    _children.begin(),
                    begin
                );
            
            _treeData->onChildrenRemoving(removals);
            
            {
                auto&& rend = std::make_reverse_iterator(begin);
                for (auto i = std::make_reverse_iterator(end); i != rend; ++i)
                    delete *i;
            }
            
            _children.erase(begin, end);
            reindexChildren(startIndex, 0);
            
            return _children.begin() + startIndex;
        }
                
        T _value;
        child_container_t _children;
        
        Node* _parent = nullptr;
        
        std::size_t _index = 0;
        std::size_t _depth = 0;
        
        Node* _end = nullptr;
        
        // TODO: In addition to _end, we could quite reasonably have a _next and 
        // _prev maintained at the same pace, which would make iterator
        // traversal cheaper (and bidirectional)
        
        Data* _treeData = nullptr;
        
        mutable aux_datatree::NodeAddress _address;
        mutable unsigned _cacheVersion = 0;
        
        friend class BasicDataTree<T>;
        
        friend class aux_datatree::ChildNodeIterator<BasicDataTree<T>, true>;
        friend class aux_datatree::ChildNodeIterator<BasicDataTree<T>, false>;
        
        friend Node* datatree_node_parent(Node* node) { return Q_LIKELY(node) ? node->_parent : nullptr; }
        friend const Node* datatree_node_parent(const Node* node) { return Q_LIKELY(node) ? node->_parent : nullptr; }
        
        friend std::size_t datatree_node_index(const Node* node) { return Q_LIKELY(node) ? node->_index : 0; } 
        friend std::size_t datatree_node_depth(const Node* node) { return Q_LIKELY(node) ? node->_depth : 0; } 
        
        friend Node* datatree_node_root(Node* node) { return Q_LIKELY(node) ? &node->root() : nullptr; }
        friend const Node* datatree_node_root(const Node* node) { return Q_LIKELY(node) ? &node->root() : nullptr; }
        
        
        friend T datatree_node_value(const Node* node)
        {
            return node ? node->value() : T();
        }
        
        friend T& datatree_node_value_ref(Node* node)
        {
            assert(node); return node->value();
        }
        
        friend const T& datatree_node_value_ref(const Node* node)
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
        
        friend aux_datatree::ConstChildNodeIterator<BasicDataTree<T>> datatree_node_children_begin(const Node* node)
        {
            using iterator_t = aux_datatree::ConstChildNodeIterator<BasicDataTree<T>>;
            return Q_LIKELY(node) ? iterator_t ( node->_children.begin() ) : iterator_t {};
        }
        
        friend aux_datatree::ChildNodeIterator<BasicDataTree<T>> datatree_node_children_begin(Node* node)
        {
            using iterator_t = aux_datatree::ChildNodeIterator<BasicDataTree<T>>;
            return Q_LIKELY(node) ? iterator_t ( node->_children.begin() ) : iterator_t {};
        }
        
        friend aux_datatree::ConstChildNodeIterator<BasicDataTree<T>> datatree_node_children_end(const Node* node)
        {
            using iterator_t = aux_datatree::ConstChildNodeIterator<BasicDataTree<T>>;
            return Q_LIKELY(node) ? iterator_t ( node->_children.end() ) : iterator_t {};
        }
        
        friend aux_datatree::ChildNodeIterator<BasicDataTree<T>> datatree_node_children_end(Node* node)
        {
            using iterator_t = aux_datatree::ChildNodeIterator<BasicDataTree<T>>;
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
                return node->_children[index];
        }
        
        friend Node* datatree_node_child_at(Node* node, std::size_t index) { return node_child_at_impl(node, index); }
        friend const Node* datatree_node_child_at(const Node* node, std::size_t index) { return node_child_at_impl(node, index); }
        
        friend Node* datatree_node_dfs_end(Node* node)
        { 
            return Q_LIKELY(node) ? node->_end : nullptr;
        }
        
        friend const Node* datatree_node_dfs_end(const Node* node)
        { 
            return Q_LIKELY(node) ? node->_end : nullptr;
        }
        
        friend bool datatree_node_is_branch(const Node* node)
        {
            return node && node->isBranch();
        }
        
        friend bool datatree_node_is_leaf(const Node* node)
        {
            return node && node->isLeaf();
        }
        
        friend aux_datatree::NodeAddress datatree_node_address(const Node* node)
        {
            return node ? node->address() : aux_datatree::NodeAddress();
        }
        
        template<typename Range>
        friend child_range datatree_node_insert_children(
                Node* parent, 
                child_iterator pos, 
                Range&& childValues
            )
        {
            if (Q_LIKELY(parent))
                return parent->insertChildren(pos, std::forward<Range>(childValues));
            else
                return child_range {};
        }
        
        friend void datatree_node_remove_children(
                Node* parent, 
                child_iterator begin,
                child_iterator end 
            )
        {
            if (Q_LIKELY(parent))
                parent->removeChildren(child_range(begin, end));
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
        
    using leaf_range        = typename Node::leaf_range;
    using const_leaf_range  = typename Node::const_leaf_range;
    
    using branch_range        = typename Node::branch_range;
    using const_branch_range  = typename Node::const_branch_range;
    
    inline BasicDataTree()
        : _data(new Data)
    {
    }
    
    explicit inline BasicDataTree(const T& v)
        : _data(new Data(v))
    {
    }
    
    explicit inline BasicDataTree(T&& v)
        : _data(new Data(std::move(v)))
    {
    }
    
    ~BasicDataTree() = default;
    
    BasicDataTree(BasicDataTree&&) = default;
    BasicDataTree& operator=(BasicDataTree&&) = default;
    
    BasicDataTree(const BasicDataTree&) = delete;
    BasicDataTree& operator=(const BasicDataTree&) = delete;
    
    inline Node& root() { return _data->root; }
    inline const Node& root() const { return _data->root; }
    
    inline std::size_t size() const { return _data->size; }
    
    inline iterator begin() { return _data->root.begin(); }
    inline const_iterator begin() const { return _data->root.begin(); }
    inline const_iterator cbegin() const { return _data->root.cbegin(); }
    
    inline iterator end() { return _data->root.end(); }
    inline const_iterator end() const { return _data->root.end(); }
    inline const_iterator cend() const { return _data->root.cend(); }
    
//     inline leaf_range leaves() { return _data->root.leaves(); }
//     inline const_leaf_range leaves() const { return _data->root.leaves(); }
//     
//     inline branch_range branches() { return _data->root.branches(); }
//     inline const_branch_range branches() const { return _data->root.branches(); }
    
private:
   
    struct Data
    {
        inline Data()
        {
            root._treeData = this;
        }
        
        inline Data(T&& v)
            : root(std::move(v))
        {
            root._treeData = this;
        }

        inline Data(const T& v)
            : root(v, typename Node::child_container_t {})
        {
            root._treeData = this;
        }
        
        inline void onChildrenInserted(const const_child_range& children)
        {
            size += boost::size(children);
            ++cacheVersion;
        }
        
        inline void onChildrenRemoving(const const_child_range& children)
        {
            for (const Node& child : children)
                size -= boost::size(child);
            
            ++cacheVersion;
        }
        
        Node root;
        std::size_t size = 1;
        
        unsigned cacheVersion = 0;
    };
    
    friend const Node* datatree_root(const BasicDataTree& tree)
    {
        return &(tree.root());
    }
    
    friend Node* datatree_root(BasicDataTree& tree)
    {
        return &(tree.root());
    }
    
    std::unique_ptr<Data> _data;
};

template<typename T>
struct aux_datatree::datatree_traits<BasicDataTree<T>>
{
    using node_handle = typename BasicDataTree<T>::Node*;
    using const_node_handle = const typename BasicDataTree<T>::Node*;
    
    using node_value_type = T;
};

template<typename T, bool IsConst>
class aux_datatree::ChildNodeIterator<BasicDataTree<T>, IsConst>
    : public boost::indirect_iterator<
        boost::mp11::mp_if_c<
            IsConst,
            typename BasicDataTree<T>::Node::child_container_t::const_iterator,
            typename BasicDataTree<T>::Node::child_container_t::iterator
        >
    >
{
    using base_t = boost::mp11::mp_if_c<
            IsConst,
            typename BasicDataTree<T>::Node::child_container_t::const_iterator,
            typename BasicDataTree<T>::Node::child_container_t::iterator
        >;
        
    using adaptor_t = boost::indirect_iterator<base_t>;
        
    using handle_t = boost::mp11::mp_if_c<
            IsConst,
            const typename BasicDataTree<T>::Node*,
            typename BasicDataTree<T>::Node*
        >;
        
public:
    ChildNodeIterator() = default;
    ChildNodeIterator(const ChildNodeIterator&) = default;
    ChildNodeIterator(ChildNodeIterator&&) = default;
    
    ChildNodeIterator(handle_t node)
        : adaptor_t([node]() -> base_t {
            if (Q_UNLIKELY(!node))
                return base_t {};
            
            assert(node->_parent);
            // No situation should result in a child node iterator being 
            // made for a root node.
            
            return std::visit(
                    typename BasicDataTree<T>::Node::dataVisitor_getChildrenBegin {}, 
                    node->_parent->_nodeData
                ) + node->_index;
        }())
    {
    }
    
    ChildNodeIterator(const base_t& i)
        : adaptor_t(i)
    {
    }
    
    // not sure why this one is needed
    ChildNodeIterator(const adaptor_t& i)
        : adaptor_t(i)
    {
    }
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, ChildNodeIterator<BasicDataTree<T>, false>>::value, 
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
            && std::is_same<MutableIterator, ChildNodeIterator<BasicDataTree<T>, false>>::value,
            void*
        > = nullptr
    >
    ChildNodeIterator(MutableIterator&& i)
        : adaptor_t(std::move(i.base()))
    {
    }
    
    ChildNodeIterator& operator=(const ChildNodeIterator&) = default;
    ChildNodeIterator& operator=(ChildNodeIterator&&) = default;
    
    inline operator handle_t () const { return *(this->base()); }
    
    friend class ChildNodeIterator<BasicDataTree<T>, !IsConst>;
    friend class boost::iterator_core_access;
};

} // namespace Addle

