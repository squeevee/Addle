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

#include "./aux.hpp"
#include "./observer.hpp"
#include "utilities/metaprogramming.hpp"
#include "utilities/ranges.hpp"

#ifdef ADDLE_TEST
class DataTree_UTest;
#endif

namespace Addle {
namespace aux_datatree {

class AddleDataTree_NodeBase;
class AddleDataTree_TreeDataBase;
    
struct AddleDataTree_NodeRefData
{
    QAtomicPointer<AddleDataTree_NodeBase> node;
    QAtomicInteger<unsigned> refCount;
    
    QSharedPointer<TreeObserverData> treeObserverData;
    
    friend class AddleDataTree_NodeBase;
    template<class, bool> friend class NodeRef;
};

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
    void removeChildren_impl(inner_iterator_t begin, inner_iterator_t end);
    
    const AddleDataTree_NodeBase* findEnd() const { return findEnd_impl<true>(this); }
    AddleDataTree_NodeBase* findEnd() { return findEnd_impl<false>(this); }
    
    template<bool IsConst>
    using _ptr_t = boost::mp11::mp_if_c<
            IsConst, 
            const AddleDataTree_NodeBase*, 
            AddleDataTree_NodeBase*
        >;
    
    template<bool IsConst> 
    static _ptr_t<IsConst> findEnd_impl(_ptr_t<IsConst>);
    
    [[noreturn]] void throw_childOutOfRange(std::size_t index) const;
    [[noreturn]] void throw_insertPosOutOfRange(std::size_t pos) const;
    [[noreturn]] void throw_removeIndicesOutOfRange(std::size_t pos, std::size_t count) const;
    
    inner_child_container_t _children;
    
    AddleDataTree_NodeBase* _parent = nullptr;
    
    std::size_t _index = 0;
    std::size_t _depth = 0;
    std::size_t _descendantCount = 0;
    
    AddleDataTree_NodeBase* _prev = nullptr;
    AddleDataTree_NodeBase* _end = nullptr;
    
    AddleDataTree_TreeDataBase* _treeData = nullptr;
    
    AddleDataTree_NodeRefData* _nodeRefData = nullptr;
    
    ValencyHint _valencyHint = ValencyHint_Unset;
    
    friend class AddleDataTree_TreeDataBase;
#ifdef ADDLE_TEST
    friend class DataTree_UTest;
#endif
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
        _endSentinel._nodeRefData = nullptr;
    }
    
    using inner_node_iterator_t = AddleDataTree_NodeBase::inner_iterator_t;
    
    template<bool Throws>
    const AddleDataTree_NodeBase* nodeAt(const NodeAddress& address) const noexcept(!Throws);
    
    void invalidateAddressCache(const AddleDataTree_NodeBase* from) const;
        
    void updateMaxCacheAddressSize()
    {
        // ???
        const QMutexLocker lock(&_cacheMutex);
        _maxAddressCacheSize = std::max(
                (unsigned) (2 * std::sqrt(_root->_descendantCount)), 
                16U
            );
        pruneAddressCache();
    }
    
    void pruneAddressCache() const
    {
        assert(!_cacheMutex.tryLock());
        
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
    
    mutable QMutex _cacheMutex;
    
    friend class AddleDataTree_NodeBase;
#ifdef ADDLE_TEST
    friend class DataTree_UTest;
#endif
};

extern template const AddleDataTree_NodeBase* AddleDataTree_TreeDataBase::nodeAt<true>(const NodeAddress&) const;
extern template const AddleDataTree_NodeBase* AddleDataTree_TreeDataBase::nodeAt<false>(const NodeAddress&) const noexcept;

template<class AddleDataTree_>
class AddleDataTree_TreeDataWithObserverBase
{
    using observer_t = TreeObserver<AddleDataTree_>;
    
protected:
    // observer must be initialized with a reference to the tree, so this must
    // be called from inside the *tree* constructor
    inline void initializeObserver(AddleDataTree_& tree)
    {
        new (&_observerStorage) observer_t(tree);
    }
    
    AddleDataTree_TreeDataWithObserverBase() = default;
    
    inline ~AddleDataTree_TreeDataWithObserverBase()
    {
        observer().~observer_t();
    }
    
    observer_t& observer() { return *reinterpret_cast<observer_t*>(&_observerStorage); }
    const observer_t& observer() const { return *reinterpret_cast<const observer_t*>(&_observerStorage); }
    
private:
    std::aligned_storage_t<sizeof(observer_t), alignof(observer_t)> _observerStorage;
    
    template<class> friend class AddleDataTree_WithObserverBase;
#ifdef ADDLE_TEST
    friend class DataTree_UTest;
#endif
};

template<class AddleDataTree_>
class AddleDataTree_WithObserverBase
{
    using data_t = AddleDataTree_TreeDataWithObserverBase<AddleDataTree_>;
public:
    TreeObserver<AddleDataTree_>& observer()
    {
        return static_cast<data_t&>(*static_cast<AddleDataTree_*>(this)->_data).observer();
    }
    
    const TreeObserver<AddleDataTree_>& observer() const
    {
        return static_cast<const data_t&>(*static_cast<const AddleDataTree_*>(this)->_data).observer();
    }
    
    std::unique_ptr<QReadLocker> lockForRead() const { return observer().lockForRead(); }
    std::unique_ptr<QWriteLocker> lockForWrite() const { return observer().lockForWrite(); }
};

class ValencyHintViolationError : std::logic_error
{
public:
    enum Why
    {
        // Indicates an inappropriate attempt to add children to a node with the
        // leaf valency hint, or to set a node that already had children to the
        // leaf valency hint.
        ChildrenInLeaf,
        
        // Indicates an inappropriate value for a node's valency hint. E.g., a 
        // tree's value type may be a pointer to a polymorphic base type, for
        // which branches and leaves are expected to dynamically be particular
        // derived classes.
        // This option is available for users when verifying trees, it is not
        // enforced by AddleDataTree.
        InappropriateValue
    };
    
    ValencyHintViolationError(
            Why why,
            ValencyHint valencyHint = ValencyHint_Unset,
            NodeAddress address = {}
        );
    
    virtual ~ValencyHintViolationError() = default;
    
    Why why() const { return _why; }
    ValencyHint valencyHint() const { return _valencyHint; }
    NodeAddress address() const { return _address; }
    
private:
    Why _why;
    ValencyHint _valencyHint;
    NodeAddress _address;
};

enum AddleDataTreeOptions : unsigned
{
    AddleDataTreeOptions_Null         = 0x00,
    AddleDataTreeOptions_Observer     = 0x01
};

Q_DECLARE_FLAGS(AddleDataTreeOptionFlags, AddleDataTreeOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(AddleDataTreeOptionFlags)

} // namespace aux_datatree
 
template<typename T, unsigned OptionFlags = 0>
class AddleDataTree 
    : public boost::mp11::mp_apply<
        boost::mp11::mp_inherit,
        mp_build_list<
            boost::mp11::mp_bool<static_cast<bool>(OptionFlags & aux_datatree::AddleDataTreeOptions_Observer)>,
                aux_datatree::AddleDataTree_WithObserverBase<AddleDataTree<T, OptionFlags>>
        >
    >
{
    struct Data;
public:
    static constexpr bool HasObserver = static_cast<bool>(OptionFlags & aux_datatree::AddleDataTreeOptions_Observer);
    
    class Node : private aux_datatree::AddleDataTree_NodeBase
    {
    public:
        using value_type        = Node;
        using reference         = Node&;
        using const_reference   = const Node&;
        using iterator          = aux_datatree::NodeIterator<AddleDataTree<T, OptionFlags>>;
        using const_iterator    = aux_datatree::ConstNodeIterator<AddleDataTree<T, OptionFlags>>;
        using difference_type   = std::ptrdiff_t;
        using size_type         = std::size_t;
        
        using child_iterator        = aux_datatree::ChildNodeIterator<AddleDataTree<T, OptionFlags>>;
        using const_child_iterator  = aux_datatree::ConstChildNodeIterator<AddleDataTree<T, OptionFlags>>;
        
        using child_range       = aux_datatree::ChildNodeRange<AddleDataTree<T, OptionFlags>>;
        using const_child_range = aux_datatree::ConstChildNodeRange<AddleDataTree<T, OptionFlags>>;
        
        using descendant_range          = aux_datatree::NodeRange<AddleDataTree<T, OptionFlags>>;
        using const_descendant_range    = aux_datatree::ConstNodeRange<AddleDataTree<T, OptionFlags>>;
        
        using ancestor_range        = aux_datatree::AncestorNodeRange<AddleDataTree<T, OptionFlags>>;
        using const_ancestor_range  = aux_datatree::ConstAncestorNodeRange<AddleDataTree<T, OptionFlags>>;
        
        using node_ref_t        = aux_datatree::NodeRef<AddleDataTree<T, OptionFlags>, false>;
        using const_node_ref_t  = aux_datatree::NodeRef<AddleDataTree<T, OptionFlags>, true>;
        
        Node(const Node&)               = delete;
        Node(Node&&)                    = delete;
        
        Node& operator=(const Node&)    = delete;
        Node& operator=(Node&&)         = delete;
        
        virtual ~Node() noexcept = default;
        
        inline const Node* parent() const { return static_cast<const Node*>(_parent); }
        inline Node* parent() { return static_cast<Node*>(_parent); }
        
        inline std::size_t depth() const { return _depth; }
        inline std::size_t index() const { return _index; }
        DataTreeNodeAddress address() const { return this->address_impl(); }
        
        Node& root() { return static_cast<Node&>(*static_cast<Data*>(_treeData)->_root); }
        
        const Node& root() const { return static_cast<const Node&>(*static_cast<Data*>(_treeData)->_root); }
        
        inline iterator begin() { return iterator(this); }
        inline const_iterator begin() const { return const_iterator(this);  }
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
        
        inline bool isRoot() const { return static_cast<const Data*>(_treeData)->_root.get() == this; }
        
        inline const T& value() const & { return _value; }
        inline T& value() & { return _value; }
        inline T&& value() && { return std::move(_value); }
        
        template<typename U = T>
        inline void setValue(U&& value)
        {
            _value = std::forward<U>(value);
        }
        
        template<typename U = T>
        inline Node& operator= (U&& value)
        {
            setValue(std::forward<U>(value));
            return *this;
        }
        
        inline operator const T& () const { return _value; }
        inline operator T& () & { return _value; }
        inline operator T&& () && { return std::move(_value); }
        
        inline bool hasChildren() const { return !_children.empty(); }
        inline std::size_t childCount() const { return _children.size(); }
        
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
            if (Q_UNLIKELY(index >= _children.size()))
                throw_childOutOfRange(index);
                
            return *(static_cast<Node*>(_children[index]));
        }
        
        inline const Node& childAt(std::size_t index) const
        {
            if (Q_UNLIKELY(index >= _children.size()))
                throw_childOutOfRange(index);
                
            
            return *(static_cast<const Node*>(_children[index]));
        }
        
        inline Node& descendantAt(DataTreeNodeAddress address)
        {
            if (isRoot())
            {
                auto node = static_cast<Data*>(_treeData)
                    ->template nodeAt<true>(address);
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
            address = this->address() + std::move(address);
            
            return *static_cast<const Node*>(
                    static_cast<Data*>(_treeData)->nodeAt(address)                    
                );
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
        
        inline ancestor_range ancestors()
        { 
            return ancestor_range( _parent, (Node*) nullptr ); 
        }
        
        inline const_ancestor_range ancestors() const 
        { 
            return const_ancestor_range( (const Node*) _parent, (const Node*) nullptr ); 
        }
        
        inline ancestor_range ancestorsAndSelf() 
        { 
            return ancestor_range( this, (Node*) nullptr );
        }
        
        inline const_ancestor_range ancestorsAndSelf() const
        { 
            return const_ancestor_range( this, (const Node*) nullptr ); 
        }

        template<typename U = T>
        inline Node& appendChild(U&& branchValue = {})
        {
            return *appendChild_impl(std::forward<U>(branchValue));
        }

        template<typename U = T>
        inline Node& insertChild(const_child_iterator pos, U&& branchValue = {})
        {
            return *insertChild_impl(pos, std::forward<U>(branchValue));
        }
    
        template<typename U = T>
        inline Node& insertChild(std::size_t pos, U&& branchValue = {})
        {
            if (Q_UNLIKELY(pos > _children.size()))
                throw_insertPosOutOfRange(pos);
            
            return *insertChild_impl(const_child_iterator(_children.begin() + pos), std::forward<U>(branchValue));
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
            if (Q_UNLIKELY(pos > _children.size()))
                throw_insertPosOutOfRange(pos);
            
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
            if (Q_UNLIKELY(pos > _children.size()))
                throw_insertPosOutOfRange(pos);
            
            return insertChildren_impl(
                    _children.begin() + pos,
                    childValues
                );
        }
        
        // it's an uncommon case, probably better to use a repeat range
//         inline child_range insertChildrenCount(std::size_t pos, std::size_t count)
//         {
//             return insertChildrenCount_impl(pos, count);
//         }

        child_iterator removeChildren(child_range children)
        {
            return removeChildren_p(children);
        }
        
        void removeChildren(std::size_t pos, std::size_t count)
        {
            if (Q_UNLIKELY(pos + count > _children.size()))
                throw_removeIndicesOutOfRange(pos, count);
            
            auto&& begin    = _children.begin() + pos;
            auto&& end      = _children.begin() + pos + count;
            
            // TODO error check?
            removeChildren_p(
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
        
        node_ref_t nodeRef() { return node_ref_t(this); }
        const_node_ref_t nodeRef() const { return const_node_ref_t(this); }
        
        inline aux_datatree::ValencyHint valencyHint() const { return this->_valencyHint; }

        void setValencyHint(aux_datatree::ValencyHint hint)
        {
            if (Q_UNLIKELY(hint == aux_datatree::ValencyHint_Leaf && !_children.empty()))
            {
                throw aux_datatree::ValencyHintViolationError(
                        aux_datatree::ValencyHintViolationError::ChildrenInLeaf,
                        hint,
                        address()
                    );
            }
            
            this->_valencyHint = hint;
        }
        
        bool branchHint() const { return this->_valencyHint == aux_datatree::ValencyHint_Branch; }
        bool leafHint() const { return this->_valencyHint == aux_datatree::ValencyHint_Leaf; }
        
    private:
        explicit inline Node(T&& value = {})
            : _value(std::move(value))
        {
            this->_nodeRefData 
                = new aux_datatree::AddleDataTree_NodeRefData { this, 1 };
        }
        
        explicit inline Node(const T& value)
            : _value(value)
        {
            this->_nodeRefData 
                = new aux_datatree::AddleDataTree_NodeRefData { this, 1 };
        }
                
        template<typename Range>
        child_range insertChildren_impl(const_child_iterator pos, Range&& childValues);
        
//         child_range insertChildrenCount_impl(std::size_t pos, std::size_t count);
        
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
        
        child_iterator removeChildren_p( child_range removals );
                
        T _value;
        
        friend class AddleDataTree<T, OptionFlags>;
        friend struct AddleDataTree<T, OptionFlags>::Data;
        
        friend class aux_datatree::ChildNodeIterator<AddleDataTree<T, OptionFlags>, true>;
        friend class aux_datatree::ChildNodeIterator<AddleDataTree<T, OptionFlags>, false>;
        
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
            return Q_LIKELY(node) ? 
                static_cast<Node*>(static_cast<Data*>(node->_treeData)->_root.get())
                : nullptr; 
        }
        
        friend const Node* datatree_node_root(const Node* node) 
        { 
            return Q_LIKELY(node) ? 
                static_cast<const Node*>(static_cast<Data*>(node->_treeData)->_root.get())
                : nullptr; 
        }
        
        friend bool datatree_node_is_root(const Node* node)
        {
            return Q_LIKELY(node) && (
                    static_cast<const Node*>(
                        static_cast<Data*>(node->_treeData)->_root.get()
                    ) == node
                );
        }
        
        friend T& datatree_node_value(Node* node)
        {
            assert(node); return node->_value;
        }
        
        friend const T& datatree_node_value(const Node* node)
        {
            assert(node); return node->_value;
        }
        
        template<typename U>
        friend void datatree_node_set_value(Node* node, U&& v)
        {
            if (Q_LIKELY(node)) node->_value = std::forward<U>(v);
        }
    
        friend std::size_t datatree_node_child_count(const Node* node)
        { 
            return Q_LIKELY(node) ? node->_children.size() : 0;
        }
        
        friend const_child_iterator datatree_node_children_begin(const Node* node)
        {
            return Q_LIKELY(node) ? const_child_iterator ( node->_children.begin() ) : const_child_iterator {};
        }
        
        friend child_iterator datatree_node_children_begin(Node* node)
        {
            return Q_LIKELY(node) ? child_iterator ( node->_children.begin() ) : child_iterator {};
        }
        
        friend const_child_iterator datatree_node_children_end(const Node* node)
        {
            return Q_LIKELY(node) ? const_child_iterator ( node->_children.end() ) : const_child_iterator {};
        }
        
        friend child_iterator datatree_node_children_end(Node* node)
        {
            return Q_LIKELY(node) ? child_iterator ( node->_children.end() ) : child_iterator {};
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
            return Q_LIKELY(node) ? 
                static_cast<Node*>(
                    Q_LIKELY(node->_end) ? node->_end : node->findEnd()
                ) : nullptr;
        }
        
        friend const Node* datatree_node_dfs_end(const Node* node)
        { 
            return Q_LIKELY(node) ? 
                static_cast<const Node*>(
                    Q_LIKELY(node->_end) ? node->_end : node->findEnd()
                ) : nullptr;
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
        
        friend aux_datatree::NodeAddress datatree_node_address(const Node* node)
        {
            return node ? node->address_impl() : aux_datatree::NodeAddress();
        }
        
        Node* lookup_address_p(const aux_datatree::NodeAddress& address)
        {
            return const_cast<Node*>(static_cast<const Node*>(
                static_cast<const Data*>(_treeData)
                    ->template nodeAt<false>(address_impl() + address)
            ));
        }
        
        const Node* lookup_address_p(const aux_datatree::NodeAddress& address) const
        {
            return static_cast<const Node*>(
                static_cast<const Data*>(_treeData)
                    ->template nodeAt<false>(address_impl() + address)
            );
        }
        
        friend Node* datatree_node_lookup_address(Node* node, const aux_datatree::NodeAddress& address)
        {
            return Q_LIKELY(node) ? node->lookup_address_p(address) : nullptr;
        }
        
        friend const Node* datatree_node_lookup_address(const Node* node, const aux_datatree::NodeAddress& address)
        {
            return Q_LIKELY(node) ? node->lookup_address_p(address) : nullptr;
        }
        
        template<typename Range>
        friend child_iterator datatree_node_insert_children(
                Node* parent, 
                child_iterator pos, 
                Range&& childValues
            )
        {            
            if (Q_LIKELY(parent))
            {
                return parent->insertChildren_impl(
                        pos,
                        std::forward<Range>(childValues)
                    ).begin();
            }
            else
            {
                return child_iterator {};
            }
        }
        
        friend child_iterator datatree_node_remove_children(
                Node* parent, 
                child_iterator begin,
                child_iterator end 
            )
        {
            if (Q_LIKELY(parent))
                return parent->removeChildren_p(child_range(begin, end));
            else
                return child_iterator {};
        }
        
        friend bool datatree_node_branch_hint(const Node* node)
        {
            return Q_LIKELY(node) && node->branchHint();
        }
        
        friend bool datatree_node_leaf_hint(const Node* node)
        {
            return Q_LIKELY(node) && node->leafHint();
        }
        
        friend class aux_datatree::NodeRef<AddleDataTree<T, OptionFlags>, true>;
        friend class aux_datatree::NodeRef<AddleDataTree<T, OptionFlags>, false>;
        
#ifdef ADDLE_TEST
        friend class DataTree_UTest;
#endif
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
        if constexpr (HasObserver)
        {
            _data->initializeObserver(*this);
            aux_datatree::NodeRef<AddleDataTree<T, OptionFlags>, true>::setTreeObserverData(
                    static_cast<Node&>(*_data->_root)._nodeRefData,
                    _data->observer()
                );
        }
    }
    
    explicit inline AddleDataTree(const T& v)
        : _data(new Data(v))
    {
        if constexpr (HasObserver)
        {
            _data->initializeObserver(*this);
            aux_datatree::NodeRef<AddleDataTree<T, OptionFlags>, true>::setTreeObserverData(
                    static_cast<Node&>(*_data->_root)._nodeRefData,
                    _data->observer()
                );
        }
    }
    
    explicit inline AddleDataTree(T&& v)
        : _data(new Data(std::move(v)))
    {
        if constexpr (HasObserver)
        {
            _data->initializeObserver(*this);
            aux_datatree::NodeRef<AddleDataTree<T, OptionFlags>, true>::setTreeObserverData(
                    static_cast<Node&>(*_data->_root)._nodeRefData,
                    _data->observer()
                );
        }
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
            = _data->template nodeAt<true>(address);
        return const_cast<Node&>(static_cast<const Node&>(*node));
    }
    
    inline const Node& nodeAt(const DataTreeNodeAddress& address) const
    {
        return static_cast<const Node&>(*_data->template nodeAt<true>(address));
    }
    
//     inline leaf_range leaves() { return _data->root.leaves(); }
//     inline const_leaf_range leaves() const { return _data->root.leaves(); }
//     
//     inline branch_range branches() { return _data->root.branches(); }
//     inline const_branch_range branches() const { return _data->root.branches(); }
    
private:
    struct Data 
        : private aux_datatree::AddleDataTree_TreeDataBase,
        private boost::mp11::mp_if_c<
            HasObserver,
            aux_datatree::AddleDataTree_TreeDataWithObserverBase<AddleDataTree<T, OptionFlags>>,
            boost::mp11::mp_inherit<>
        >
    {
        inline Data();
        inline Data(const T&);
        inline Data(T&&);
        
        // friend access to protected base members
        friend class AddleDataTree<T, OptionFlags>;
        friend class AddleDataTree<T, OptionFlags>::Node;
        friend class aux_datatree::AddleDataTree_WithObserverBase<AddleDataTree<T, OptionFlags>>;
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
    
    friend class aux_datatree::AddleDataTree_WithObserverBase<AddleDataTree<T, OptionFlags>>;
#ifdef ADDLE_TEST
    friend class DataTree_UTest;
#endif
};

template<typename T, unsigned OptionFlags>
template<typename Range>
typename AddleDataTree<T, OptionFlags>::Node::child_range 
AddleDataTree<T, OptionFlags>::Node::insertChildren_impl(const_child_iterator pos, Range&& childValues)
{
    using namespace boost::adaptors;
    using U = typename boost::range_value<boost::remove_cv_ref_t<Range>>::type;
    static_assert(std::is_constructible_v<T, U&&>);
    
    if (Q_UNLIKELY(_valencyHint == aux_datatree::ValencyHint_Leaf))
    {
        throw aux_datatree::ValencyHintViolationError(
                aux_datatree::ValencyHintViolationError::ChildrenInLeaf,
                _valencyHint,
                address()
            );
    }
    
    if (boost::empty(childValues)) 
        return child_range();
    
    std::size_t startIndex = std::distance(_children.cbegin(), pos.base());
    std::size_t count;
    
    if constexpr (
        std::is_convertible_v<
            typename boost::range_category<boost::remove_cv_ref_t<Range>>::type,
            std::forward_iterator_tag
        >
        && std::is_nothrow_constructible_v<T, U&&>
        && noexcept( 
            *std::declval<
                typename boost::range_iterator<boost::remove_cv_ref_t<Range>>
                ::type
            >() 
        ))
    {   
        // This route is preferable but is only safe if `childValues` is a
        // forward range (i.e., it can be pre-counted) and initializing nodes
        // is guaranteed not to throw exceptions.
        count = boost::size(childValues);
        
        _children.insert(pos.base(), count, nullptr);
        
        aux_datatree::AddleDataTree_NodeBase** i = _children.data() + startIndex;
        for (auto&& v : childValues)
        {
            *i = new Node(std::forward<decltype(v)>(v));
            ++i;
        }
    }
    else // constexpr
    {
        // This route is less efficient but works for single-pass ranges and can 
        // clean up after itself in the event of an exception initializing a
        // node.
        
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
    
    if constexpr (HasObserver)
    {
        auto& observer = static_cast<Data*>(_treeData)->observer();
        
        auto i = _children.begin() + startIndex;
        auto end = _children.begin() + startIndex + count;
        for (; i != end; ++i)
        {
            aux_datatree::NodeRef<AddleDataTree<T, OptionFlags>, true>::setTreeObserverData(
                    static_cast<Node*>(*i)->_nodeRefData,
                    observer
                );
        }
        
        if (observer.isRecording())
        {
            observer.passiveAddNodes(
                    this,
                    startIndex,
                    count
                );
        }
    }
        
    return child_range(
            _children.begin() + startIndex,
            _children.begin() + startIndex + count
        );
}

// template<typename T, unsigned OptionFlags>
// typename AddleDataTree<T, OptionFlags>::Node::child_range 
// AddleDataTree<T, OptionFlags>::Node::insertChildrenCount_impl(std::size_t pos, std::size_t count)
// {
//     if (!count) 
//         return child_range();
//     
//     assert(pos <= _children.size()); // TODO: exception
// 
//     _children.insert(_children.begin() + pos, count, nullptr);
//     
//     aux_datatree::AddleDataTree_NodeBase** i = _children.data() + pos;
//     for (; i < _children.data() + pos + count; ++i)
//     {
//         *i = new Node();
//     }
//     
//     this->reindexChildren(pos);
//     static_cast<Data*>(_treeData)->invalidateAddressCache(this);
//         
//     {
//         Node* cursor = this;
//         do
//         {
//             cursor->_descendantCount += count;
//             cursor = static_cast<Node*>(cursor->_parent);
//         } while (cursor);
//     }
//     
//     if constexpr (HasObserver)
//     {
//         auto& observer = static_cast<Data*>(_treeData)->observer();
//         
//         auto i = _children.begin() + pos;
//         auto end = _children.begin() + pos + count;
//         for (; i != end; ++i)
//         {
//             aux_datatree::NodeRef<AddleDataTree<T, OptionFlags>, true>::setTreeObserverData(
//                     static_cast<Node*>(*i)->_nodeRefData,
//                     observer
//                 );
//         }
//         
//         if (observer.isRecording())
//         {
//             observer.passiveAddNodes(
//                     this,
//                     pos,
//                     count
//                 );
//         }
//     }
//     
//     return child_range(
//             _children.begin() + pos,
//             _children.begin() + pos + count
//         );
// }

template<typename T, unsigned OptionFlags>
typename AddleDataTree<T, OptionFlags>::Node::child_iterator
AddleDataTree<T, OptionFlags>::Node::removeChildren_p(child_range removals)
{
    if (removals.empty()) return child_iterator();
    
    auto begin = removals.begin().base();
    auto end = removals.end().base();
    
    std::size_t startIndex = std::distance(_children.cbegin(), begin);
    
    if constexpr (HasObserver)
    {
        assert(_treeData);
        auto& observer = static_cast<Data*>(_treeData)->observer();
        if (observer.isRecording())
        {
            observer.passiveRemoveNodes(
                    this,
                    startIndex,
                    removals.size()
                );
        }
    }
    
    this->removeChildren_impl(begin, end);
    
    return child_iterator(_children.begin() + startIndex);
}

template<typename T, unsigned OptionFlags>
AddleDataTree<T, OptionFlags>::Data::Data()
    : aux_datatree::AddleDataTree_TreeDataBase(new Node)
{
}

template<typename T, unsigned OptionFlags>
AddleDataTree<T, OptionFlags>::Data::Data(const T& value)
    : aux_datatree::AddleDataTree_TreeDataBase(new Node(value))
{
}

template<typename T, unsigned OptionFlags>
AddleDataTree<T, OptionFlags>::Data::Data(T&& value)
    : aux_datatree::AddleDataTree_TreeDataBase(new Node(std::move(value)))
{
}

template<typename T, unsigned OptionFlags>
struct aux_datatree::datatree_traits<AddleDataTree<T, OptionFlags>>
{
    using node_handle = typename AddleDataTree<T, OptionFlags>::Node*;
    using const_node_handle = const typename AddleDataTree<T, OptionFlags>::Node*;
    
    using node_value_type = T;
};

template<typename T, unsigned OptionFlags, bool IsConst>
class aux_datatree::ChildNodeIterator<AddleDataTree<T, OptionFlags>, IsConst>
    : public boost::iterator_adaptor<
        aux_datatree::ChildNodeIterator<AddleDataTree<T, OptionFlags>, IsConst>,
        aux_datatree::AddleDataTree_NodeBase::inner_iterator_t,
        boost::mp11::mp_if_c<
            IsConst,
            const typename AddleDataTree<T, OptionFlags>::Node,
            typename AddleDataTree<T, OptionFlags>::Node
        >
    >
{
    using base_t = aux_datatree::AddleDataTree_NodeBase::inner_iterator_t;
    using adaptor_t = boost::iterator_adaptor<
            aux_datatree::ChildNodeIterator<AddleDataTree<T, OptionFlags>, IsConst>,
            aux_datatree::AddleDataTree_NodeBase::inner_iterator_t,
            boost::mp11::mp_if_c<
                IsConst,
                const typename AddleDataTree<T, OptionFlags>::Node,
                typename AddleDataTree<T, OptionFlags>::Node
            >
        >;
    using node_t = boost::mp11::mp_if_c<
            IsConst,
            const typename AddleDataTree<T, OptionFlags>::Node,
            typename AddleDataTree<T, OptionFlags>::Node
        >;
    using handle_t = boost::mp11::mp_if_c<
            IsConst,
            const typename AddleDataTree<T, OptionFlags>::Node*,
            typename AddleDataTree<T, OptionFlags>::Node*
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
            && std::is_same<MutableIterator, ChildNodeIterator<AddleDataTree<T, OptionFlags>, false>>::value, 
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
            && std::is_same<MutableIterator, ChildNodeIterator<AddleDataTree<T, OptionFlags>, false>>::value,
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
    
    friend class ChildNodeIterator<AddleDataTree<T, OptionFlags>, !IsConst>;
    friend class boost::iterator_core_access;
    
#ifdef ADDLE_TEST
    friend class DataTree_UTest;
#endif
};

template<typename T, unsigned OptionFlags, bool IsConst>
class aux_datatree::NodeRef<AddleDataTree<T, OptionFlags>, IsConst>
{
    using tree_t = AddleDataTree<T, OptionFlags>;
public:
    using handle_t = boost::mp11::mp_if_c<
            IsConst,
            const typename tree_t::Node*,
            typename tree_t::Node*
        >;
    
    NodeRef() : _data(nullptr) {}
    
    template<class Observer,
        std::enable_if_t<
            AddleDataTree<T, OptionFlags>::HasObserver && ((
                IsConst && std::is_same_v<Observer, const TreeObserver<tree_t>>
            ) || (
                std::is_same_v<std::remove_const_t<Observer>, TreeObserver<tree_t>>
            )), void*> = nullptr>
    NodeRef(Observer& observer, handle_t node)
        : NodeRef(node)
    {
        assert(!node || node->_nodeRefData->treeObserverData == observer._data);
    }
    
    NodeRef(handle_t node)
        : _data(node ? node->_nodeRefData : nullptr)
    {
        if (_data)
            _data->refCount.ref();
    }
    
    NodeRef(const NodeRef& other)
        : _data(other._data)
    {
        if (_data)
            _data->refCount.ref();
    }
    
    NodeRef(NodeRef&& other)
        : _data(other._data)
    {
        other._data = nullptr;
    }
    
    template<class MutableNodeRef,
        std::enable_if_t<
            IsConst && std::is_same_v<
                MutableNodeRef,
                NodeRef<AddleDataTree<T, OptionFlags>, false>
            >,
        void*> = nullptr>
    NodeRef(const MutableNodeRef& other)
        : _data(other._data)
    {
        if (_data)
            _data->refCount.ref();
    }
    
    ~NodeRef()
    {         
        if (_data && !_data->refCount.deref())
            delete _data; 
    }
    
    NodeRef& operator=(const NodeRef& other)
    {
        if (_data && !_data->refCount.deref())
            delete _data;
        
        if ((_data = other._data))
            _data->refCount.ref();
        
        return *this;
    }
        
    NodeRef& operator=(NodeRef&& other)
    {
        if (_data && !_data->refCount.deref())
            delete _data;
        
        _data = other._data;
        other._data = nullptr;
        
        return *this;
    }
    
    bool operator==(const NodeRef& other) const { return _data == other._data; }
    bool operator!=(const NodeRef& other) const { return _data != other._data; }
    
    friend bool operator==(const handle_t& handle, const NodeRef& ref) { return handle == ref.get(); }
    friend bool operator!=(const handle_t& handle, const NodeRef& ref) { return handle != ref.get(); }
    friend bool operator==(const NodeRef& ref, const handle_t& handle) { return ref.get() == handle; }
    friend bool operator!=(const NodeRef& ref, const handle_t& handle) { return ref.get() != handle; }
    
    explicit operator bool () const { return isValid(); }
    bool operator! () const { return !isValid(); }
    
    std::unique_ptr<QReadLocker> lockTreeObserverForRead() const
    { 
        if (_data && _data->treeObserverData)
            return _data->treeObserverData->lockForRead();
        else
            return {};
    }
    
    std::unique_ptr<QWriteLocker> lockTreeObserverForWrite() const
    { 
        if (_data && _data->treeObserverData)
            return _data->treeObserverData->lockForWrite();
        else
            return {};
    }
    
    bool isValid() const
    {
        if (!_data) return false;
        
        return static_cast<bool>(_data->node.loadRelaxed());
    }
    
    handle_t get() const
    {
        if (!_data) return {};
        
        return static_cast<handle_t>(_data->node.loadRelaxed());
    }
    
    const typename AddleDataTree<T, true>::Node* getConst() const 
    { 
        return get(); 
    }
    
    decltype(auto) operator*() const
    {
        assert(isValid());
        
        handle_t node = get();
        assert(node);
        return *node;
    }
    
private:
    static inline void setTreeObserverData(
            AddleDataTree_NodeRefData* data, 
            const TreeObserver<tree_t>& observer
        )
    {
        data->treeObserverData = observer._data;
    }
    
    std::size_t hash_p() const { return std::hash<AddleDataTree_NodeRefData*>() (_data); }
    
    friend uint qHash(const NodeRef& ref, uint seed = 0)
    {
        return ref.hash_p() ^ seed;
    }
    
    AddleDataTree_NodeRefData* _data;
    
    friend class AddleDataTree<T, OptionFlags>;
    friend class NodeRef<AddleDataTree<T, OptionFlags>, !IsConst>;
    
    friend class std::hash<NodeRef<AddleDataTree<T, OptionFlags>, IsConst>>;
};

} // namespace Addle

namespace std {

template<typename T, unsigned OptionFlags, bool IsConst>
struct hash<Addle::aux_datatree::NodeRef<Addle::AddleDataTree<T, OptionFlags>, IsConst>>
{
    std::size_t operator() (
        const Addle::aux_datatree::NodeRef<Addle::AddleDataTree<T, OptionFlags>, IsConst>& ref
    ) const
    {
        return ref.hash_p();
    }
};

}
