/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#pragma once

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>

#include <boost/range.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include <boost/type_traits.hpp>

#include "utilities/collections.hpp"

#include <algorithm> // lexicographical_compare
#include <variant>
#include <optional>
#include <iterator>
#include <vector>

#include <cstring> // memcpy

#include <QtDebug>

namespace Addle {
    
template<typename TLeaf, typename TBranch>
class DataTree;

namespace aux_datatree {
//     template<typename NodeHandle> NodeHandle datatree_node_parent(NodeHandle);
//     template<typename NodeHandle> std::size_t datatree_node_index(NodeHandle);
//     template<typename NodeHandle> std::size_t datatree_node_depth(NodeHandle);
//     template<typename NodeHandle> NodeHandle datatree_node_dfs_next(NodeHandle);
//     
//     template<typename NodeHandle> bool datatree_node_shallow_equivalent(NodeHandle, NodeHandle);
//     template<typename NodeHandle> bool datatree_node_deep_equivalent(NodeHandle, NodeHandle);
//     template<typename NodeHandle> bool datatree_node_is_branch(NodeHandle);
//     template<typename NodeHandle> bool datatree_node_is_leaf(NodeHandle);
    
    template<typename NodeHandle>
    struct node_handle_add_const;
    
    template<typename Node>
    struct node_handle_add_const<Node*> { using type = const Node*; };
    
    template<typename NodeHandle>
    using node_handle_add_const_t = typename node_handle_add_const<NodeHandle>::type;
    
    template<typename NodeHandle>
    struct node_handle_value_hash_proxy;
    
    template<typename NodeHandle>
    using node_handle_value_hash_proxy_t = typename node_handle_value_hash_proxy<NodeHandle>::type;
    
    // This is technically not a general solution, since a dereference operator 
    // does not have to return a true reference type. It is however probably 
    // adequate for our purposes. boost::pointee gives ideas of how to make this 
    // more generic, but is ancient and has its own shortcomings so we will not 
    // use it here
    template<typename T>
    using _dereferenced_val_t = std::remove_reference_t<decltype( *std::declval<T>() )>;
    
    template<typename NodeHandle, class Traversal = boost::forward_traversal_tag>
    class NodeIterator
        : public boost::iterator_facade<
            NodeIterator<NodeHandle, Traversal>,
            _dereferenced_val_t<NodeHandle>,
            Traversal
        >
    {
    public:
        NodeIterator() = default;
        NodeIterator(const NodeIterator&) = default;
        NodeIterator(NodeIterator&&) = default;
        
        NodeIterator(NodeHandle cursor)
            : _cursor(cursor)
        {
        }
        
        template<
            typename MutableNodeHandle, 
            std::enable_if_t<
                std::is_same<node_handle_add_const_t<MutableNodeHandle>, NodeHandle>::value
                && !std::is_same<MutableNodeHandle, NodeHandle>::value,
                void*
            > = nullptr
        >
        NodeIterator(const NodeIterator<MutableNodeHandle>& i)
            : _cursor(i._cursor)
        {
        }
        
        template<
            typename MutableNodeHandle, 
            std::enable_if_t<
                std::is_same<node_handle_add_const_t<MutableNodeHandle>, NodeHandle>::value
                && !std::is_same<MutableNodeHandle, NodeHandle>::value,
                void*
            > = nullptr
        >
        NodeIterator(NodeIterator<MutableNodeHandle>&& i)
            : _cursor(std::move(i._cursor))
        {
        }
        
        NodeIterator& operator=(const NodeIterator&) = default;
        NodeIterator& operator=(NodeIterator&&) = default;
        
        inline NodeHandle cursor() const { return _cursor; }
        
    private:
        inline _dereferenced_val_t<NodeHandle>& dereference() const { return *_cursor; }
        inline bool equal(const NodeIterator& x) const { return _cursor == x._cursor; }
        inline void increment() { _cursor = datatree_node_dfs_next(_cursor); }
        
        NodeHandle _cursor = {};
        
        friend class boost::iterator_core_access;
        template<typename, class> friend class NodeIterator;
    };
    
    template<typename NodeHandle>
    using ConstNodeIterator = NodeIterator<node_handle_add_const_t<NodeHandle>>;
    
    template<typename NodeHandle>
    using NodeRange = boost::iterator_range<NodeIterator<NodeHandle>>;
    
    template<typename NodeHandle>
    using ConstNodeRange = NodeRange<node_handle_add_const_t<NodeHandle>>;
    
    template<typename NodeHandle, class Traversal = boost::forward_traversal_tag>
    class AncestorIterator
        : public boost::iterator_facade<
            AncestorIterator<NodeHandle, Traversal>,
            _dereferenced_val_t<NodeHandle>,
            Traversal
        >
    {
    public:
        AncestorIterator() = default;
        AncestorIterator(const AncestorIterator&) = default;
        AncestorIterator(AncestorIterator&&) = default;
        
        AncestorIterator(NodeHandle cursor)
            : _cursor(cursor)
        {
        }
        
        template<
            typename MutableNodeHandle, 
            std::enable_if_t<
                std::is_same<node_handle_add_const_t<MutableNodeHandle>, NodeHandle>::value
                && !std::is_same<MutableNodeHandle, NodeHandle>::value,
                void*
            > = nullptr
        >
        AncestorIterator(const AncestorIterator<MutableNodeHandle>& i)
            : _cursor(i._cursor)
        {
        }
        
        template<
            typename MutableNodeHandle, 
            std::enable_if_t<
                std::is_same<node_handle_add_const_t<MutableNodeHandle>, NodeHandle>::value
                && !std::is_same<MutableNodeHandle, NodeHandle>::value,
                void*
            > = nullptr
        >
        AncestorIterator(AncestorIterator<MutableNodeHandle>&& i)
            : _cursor(std::move(i._cursor))
        {
        }
        
        AncestorIterator& operator=(const AncestorIterator&) = default;
        AncestorIterator& operator=(AncestorIterator&&) = default;
        
        inline NodeHandle cursor() const { return _cursor; }
        
    private:
        inline _dereferenced_val_t<NodeHandle>& dereference() const { return *_cursor; }
        inline bool equal(const AncestorIterator& x) const { return _cursor == x._cursor; }
        inline void increment() { _cursor = datatree_node_parent(_cursor); }
        
        NodeHandle _cursor = {};
        
        friend class boost::iterator_core_access;
        template<typename, class> friend class AncestorIterator;
    };
    
    template<typename NodeHandle>
    using ConstAncestorIterator = AncestorIterator<node_handle_add_const_t<NodeHandle>>;
    
    template<typename NodeHandle>
    using AncestorRange = boost::iterator_range<AncestorIterator<NodeHandle>>;
    
    template<typename NodeHandle>
    using ConstAncestorRange = AncestorRange<node_handle_add_const_t<NodeHandle>>;
    
    template<typename Predicate, typename NodeHandle, class Traversal = boost::forward_traversal_tag>
    class NodeFilterIterator
        : public boost::iterator_facade<
            NodeFilterIterator<Predicate, NodeHandle, Traversal>,
            _dereferenced_val_t<NodeHandle>,
            Traversal
        >
    {
    public:
        NodeFilterIterator() = default;
        NodeFilterIterator(const NodeFilterIterator&) = default;
        NodeFilterIterator(NodeFilterIterator&&) = default;
        
        NodeFilterIterator(Predicate p, NodeHandle cursor, NodeHandle end)
            : _p(p), _cursor(cursor), _end(end)
        {
            satisfyPredicate();
        }
        
        NodeFilterIterator(NodeHandle cursor, NodeHandle end)
            : _cursor(cursor), _end(end)
        {
            satisfyPredicate();
        }
                
        template<
            typename MutableNodeHandle, 
            std::enable_if_t<
                std::is_same<node_handle_add_const_t<MutableNodeHandle>, NodeHandle>::value
                && !std::is_same<MutableNodeHandle, NodeHandle>::value,
                void*
            > = nullptr
        >
        NodeFilterIterator(const NodeFilterIterator<Predicate, MutableNodeHandle>& i)
            : _p(i._p), _cursor(i._cursor), _end(i._end)
        {
        }
        
        template<
            typename MutableNodeHandle, 
            std::enable_if_t<
                std::is_same<node_handle_add_const_t<MutableNodeHandle>, NodeHandle>::value
                && !std::is_same<MutableNodeHandle, NodeHandle>::value,
                void*
            > = nullptr
        >
        NodeFilterIterator(NodeFilterIterator<Predicate, MutableNodeHandle>&& i)
            : _p(std::move(i._p)), _cursor(std::move(i._cursor)), _end(std::move(i._end))
        {
        }
        
        NodeFilterIterator& operator=(const NodeFilterIterator&) = default;
        NodeFilterIterator& operator=(NodeFilterIterator&&) = default;
        
        inline NodeHandle cursor() const { return _cursor; }
        inline NodeHandle end() const { return _end; }
        inline const Predicate & predicate() const { return _p; }
        
    private:
        inline _dereferenced_val_t<NodeHandle>& dereference() const { return *_cursor; }
        inline bool equal(const NodeFilterIterator& x) const { return _cursor == x._cursor; }
        inline void increment() 
        {
            _cursor = datatree_node_dfs_next(_cursor);
            satisfyPredicate();
        }
        
        inline void satisfyPredicate()
        {
            while (_cursor != _end && !_p(_cursor))
            {
                _cursor = datatree_node_dfs_next(_cursor);
            } 
        }
        
        Predicate _p = {};
        
        NodeHandle _cursor = {};
        NodeHandle _end = {};
        
        friend class boost::iterator_core_access;
        template<typename, typename, class> friend class NodeFilterIterator;
    };
    
    struct predicate_isLeaf
    {
        template<typename NodeHandle>
        inline bool operator()(NodeHandle node)
        {
            return datatree_node_is_leaf(node);
        }
    };
    
    template<typename NodeHandle>
    using LeafIterator = NodeFilterIterator<predicate_isLeaf, NodeHandle>;
    
    template<typename NodeHandle>
    using ConstLeafIterator = LeafIterator<node_handle_add_const_t<NodeHandle>>;
    
    template<typename NodeHandle>
    using LeafRange = boost::iterator_range<LeafIterator<NodeHandle>>;
    
    template<typename NodeHandle>
    using ConstLeafRange = LeafRange<node_handle_add_const_t<NodeHandle>>;
    
    struct predicate_isBranch
    {
        template<typename NodeHandle>
        inline bool operator()(NodeHandle node)
        {
            return datatree_node_is_branch(node);
        }
    };
    
    template<typename NodeHandle>
    using BranchIterator = NodeFilterIterator<predicate_isBranch, NodeHandle>;
    
    template<typename NodeHandle>
    using ConstBranchIterator = BranchIterator<node_handle_add_const_t<NodeHandle>>;
    
    template<typename NodeHandle>
    using BranchRange = boost::iterator_range<BranchIterator<NodeHandle>>;
    
    template<typename NodeHandle>
    using ConstBranchRange = BranchRange<node_handle_add_const_t<NodeHandle>>;
    
    
    // Generically represents the location of a node in a tree.
    class NodeAddress
    {
        static constexpr std::uint64_t INVALID_FASTADDRESS = UINT64_MAX;
    public:        
        using value_type        = std::size_t;
        using const_reference   = const std::size_t&;
        using reference         = const_reference;
        using const_iterator    = typename QList<std::size_t>::const_iterator;
        using iterator          = const_iterator;
        using difference_type   = std::ptrdiff_t;
        using size_type         = std::size_t;
        
        
        NodeAddress() = default;
        NodeAddress(const NodeAddress&) = default;
        NodeAddress(NodeAddress&&) = default;
        
        inline NodeAddress(const QList<std::size_t>& address)
            : _address(address)
        {
            updateFastAddress();
        }
        
        inline NodeAddress(QList<std::size_t>&& address)
            : _address(std::move(address))
        {
            updateFastAddress();
        }
        
        NodeAddress& operator=(const NodeAddress&) = default;
        NodeAddress& operator=(NodeAddress&&) = default;
        
        NodeAddress& operator=(const QList<std::size_t>& address)
        {
            _address = address;
            updateFastAddress();
            return *this;
        }
        
        NodeAddress& operator=(QList<std::size_t>&& address)
        {
            _address = std::move(address);
            updateFastAddress();
            return *this;
        }
        
        inline bool operator==(const NodeAddress& other) const
        {
            if (Q_LIKELY(
                _fastAddress != INVALID_FASTADDRESS 
                && other._fastAddress != INVALID_FASTADDRESS
            ))
                return _fastAddress == other._fastAddress;
            else
                return _address == other._address;
        }
        
        inline bool operator!=(const NodeAddress& other) const
        {
            return !(*this == other);
        }
        
        inline bool operator<(const NodeAddress& other) const
        {
            if (Q_LIKELY(
                _fastAddress != INVALID_FASTADDRESS 
                && other._fastAddress != INVALID_FASTADDRESS
            ))
                return _fastAddress < other._fastAddress;
            else
                return std::lexicographical_compare(
                        _address.begin(),           _address.end(),
                        other._address.begin(),     other._address.end()
                    );
        }
        
        inline bool operator<=(const NodeAddress& other) const
        {
            return !(*this > other);
        }
        
        inline bool operator>(const NodeAddress& other) const
        {
            if (Q_LIKELY(
                _fastAddress != INVALID_FASTADDRESS 
                && other._fastAddress != INVALID_FASTADDRESS
            ))
                return _fastAddress > other._fastAddress;
            else
                return std::lexicographical_compare(
                        other._address.begin(),     other._address.end(),
                        _address.begin(),           _address.end()
                    );
        }
        
        inline bool operator>=(const NodeAddress& other) const
        {
            return !(*this < other);
        }
        
        inline std::size_t size() const     { return _address.size(); }
        inline const_iterator begin() const { return _address.begin(); }
        inline const_iterator end() const   { return _address.end(); }
        
        inline QList<std::size_t> values() const    { return _address; }
        inline operator QList<std::size_t>() const  { return _address; }
        
        inline bool isRoot() const { return _address.isEmpty(); }
        
        inline NodeAddress parent() const
        {
            if (isRoot())
                return NodeAddress();
            else
                return NodeAddress( QList<std::size_t>(
                        _address.begin(),
                        _address.begin() + (_address.size() - 1)
                    ));
        }
        
        inline NodeAddress operator<<(std::size_t index) &&
        {
            _fastAddress = INVALID_FASTADDRESS;
            return NodeAddress(std::move(_address << index));
        }
        
        inline void swap(NodeAddress& other)
        {
            _address.swap(other._address);
            std::swap(_fastAddress, other._fastAddress);
        }
        
private:
        inline void updateFastAddress()
        {
            // First, portion a 64-bit uint into an "array" of 8 x 8-bit uints 
            // composed from the address indices with the first index filling
            // the most significant bits, and the 8th filling the least
            // significant bits. Add 1 to each index before inserting in order
            // to distinguish between a 0 and an empty cell.
            //
            // NOTE: This scheme has imperfect packing. Theoretically the
            // MAX_CELL_VALUE could be raised to UINT8_MAX by taking advantage 
            // of adder overflow. This is not a great concern.
            
            constexpr unsigned CELL_COUNT = 8;
            constexpr unsigned CELL_WIDTH = 8;
            constexpr unsigned MAX_CELL_VALUE = UINT8_MAX - 1;

            const std::size_t size = _address.size();
            
            if (size > CELL_COUNT)
            {
                _fastAddress = INVALID_FASTADDRESS;
                return;
            }
            
            _fastAddress = 0;
            for (std::size_t v : _address)
            {
                if (v > MAX_CELL_VALUE)
                {
                    _fastAddress = INVALID_FASTADDRESS;
                    return;
                }
                
                _fastAddress = _fastAddress << CELL_WIDTH | v + 1;
            }
            _fastAddress <<= qMax(CELL_COUNT - size, (std::size_t) 0) * CELL_WIDTH;
        }
        
        QList<std::size_t> _address;
        
        // While the total address space of a data tree is arbitrarily large and 
        // requires dynamic memory and iteration to represent fully, we can 
        // encode a reasonable portion of the address space (covering a majority 
        // of typical cases) into a 64-bit integer such that arithmetic
        // comparison of _fastAddress is equivalent to lexicographical 
        // comparison of _address (provided that both _fastAddress are valid)
        //
        // That "reasonable portion" is currently tuned to approximately max
        // index of 255 and max depth of 8. This can be adjusted if desired.
        std::uint64_t _fastAddress = 0;
        
        friend uint qHash(const NodeAddress& address, uint seed = 0)
        {
            if (Q_LIKELY(address._fastAddress != INVALID_FASTADDRESS))
                return ::qHash(address._fastAddress, seed);
            else
                return ::qHashRange(
                        address._address.begin(),
                        address._address.end(),
                        seed
                    );
        }
        
        friend void swap(NodeAddress& a1, NodeAddress& a2)
        {
            a1.swap(a2);
        }
        
#ifdef ADDLE_DEBUG
        friend QDebug operator<<(QDebug debug, const NodeAddress& nodeAddress)
        {
            using namespace boost::adaptors;
            
            if (nodeAddress.size() == 0)
            {
                debug << "{}";
            }
            else
            {
                debug 
                    << "{"
                    << qUtf8Printable(QStringList(
                        toQList(
                            nodeAddress | transformed([] (std::size_t i) { return QString::number(i); })
                        )
                    ).join(", "))
                    << "}";
            }
            
            if (nodeAddress._fastAddress != INVALID_FASTADDRESS)
            {
                debug
                    << "_fastAddress:"
                    << qUtf8Printable(
                        QString::asprintf("%0.16lx", nodeAddress._fastAddress)
                    );
            }
            
            return debug;
        }
#endif
    };
    
    template<typename NodeHandle>
    inline NodeAddress datatree_node_address(NodeHandle handle)
    {
        NodeHandle cursor = handle;
        NodeHandle parent = datatree_node_parent(cursor);
        
        QList<std::size_t> address;
        address.reserve(datatree_node_depth(handle));
            // does this actually help if we're prepending everything?
            // It's just as easy to us to build the list forward then
            // reverse it.
        
        while (parent)
        {
            address.push_front(datatree_node_index(cursor));
            cursor = parent;
            parent = datatree_node_parent(cursor);
        }
        
        return NodeAddress(std::move(address));
    }
    

    
    // 
    class EditSummary
    {
    public:
        struct MoveRecord
        {
            NodeAddress before;
            NodeAddress after;
        };
        
    };
    
//     template<typename>
//     class NodeValueHashProxy;
    
    template<typename TLeaf, typename TBranch, class TreeData>
    class Node 
    {
        template<typename TBranch_, class ChildContainer>
        struct _BranchData
        {
            _BranchData() = default;
            _BranchData(ChildContainer&& children_, TBranch_&& value_)
                : children(std::move(children_)), value(std::move(value_))
            {
            }
            
            _BranchData(const ChildContainer& children_, const TBranch_& value_)
                : children(children_), value(value_)
            {
            }
            
            ChildContainer children;
            TBranch_ value;
        };
        
        template<class ChildContainer>
        struct _BranchData<void, ChildContainer>
        {
            _BranchData() = default;
            _BranchData(ChildContainer&& children_)
                : children(std::move(children_))
            {
            }
            
            _BranchData(const ChildContainer& children_)
                : children(children_)
            {
            }
            
            ChildContainer children;
        };
        
        // Using std::vector instead of a Qt type mainly for the insert overload
        // accepting two iterators, which we use for inserting children. Node
        // should never be copied so implicit sharing is no advantage to us.
        //
        // I do not know how efficient the two-iterator insert overload actually
        // is for single-pass iterators, which we are likely to occasionally
        // need. The preferable solution may be to copy from the iterators into 
        // a buffer (e.g., QVarLengthArray to avoid dynamic memory usage if
        // possible) before moving them into the child container. At that point,
        // we'll know the exact count of them so any container will work just as
        // well and it might as well be QList where we're guaranteed it's moving
        // pointers with memcpy. 
        using child_container_t = std::vector<Node<TLeaf, TBranch, TreeData>*>;
        
        using branch_data_t = _BranchData<TBranch, child_container_t>;
        using node_data_t = std::variant<TLeaf, branch_data_t>;
        
    public:
        using value_type        = Node<TLeaf, TBranch, TreeData>;
        using reference         = Node<TLeaf, TBranch, TreeData>&;
        using const_reference   = const Node<TLeaf, TBranch, TreeData>&;
        using iterator          = NodeIterator<Node<TLeaf, TBranch, TreeData>*>;
        using const_iterator    = NodeIterator<const Node<TLeaf, TBranch, TreeData>*>;
        using difference_type   = std::ptrdiff_t;
        using size_type         = std::size_t;
        
        using child_range       = boost::iterator_range<
                boost::indirect_iterator<typename child_container_t::const_iterator>
            >;
        using const_child_range = boost::iterator_range<
                boost::indirect_iterator<
                    typename child_container_t::const_iterator,
                    const Node<TLeaf, TBranch, TreeData>
                >
            >;
        
        using descendant_range          = NodeRange<Node<TLeaf, TBranch, TreeData>*>;
        using const_descendant_range    = ConstNodeRange<Node<TLeaf, TBranch, TreeData>*>;
        
        using ancestor_range        = AncestorRange<Node<TLeaf, TBranch, TreeData>*>;
        using const_ancestor_range  = ConstAncestorRange<Node<TLeaf, TBranch, TreeData>*>;
        
        using leaf_range            = LeafRange<Node<TLeaf, TBranch, TreeData>*>;
        using const_leaf_range      = ConstLeafRange<Node<TLeaf, TBranch, TreeData>*>;
        
        using branch_range          = BranchRange<Node<TLeaf, TBranch, TreeData>*>;
        using const_branch_range    = ConstBranchRange<Node<TLeaf, TBranch, TreeData>*>;
        
        Node()                          = delete;
        Node(const Node&)               = delete;
        Node(Node&&)                    = delete;
        
        Node& operator=(const Node&)    = delete;
        Node& operator=(Node&&)         = delete;
        
        inline ~Node()
        {
            if (!_emancipated)
            {
                std::visit(dataVisitor_deleteChildren {}, _nodeData);
            }
        }
        
        inline Node* parent() const { return _parent; }
        inline std::size_t depth() const { return _depth; }
        inline std::size_t index() const { return _index; }
        
        inline NodeAddress address() const
        {
            if (_treeData && _cacheVersion == _treeData->cacheVersion)
                return _address;
            else
                return _address = ::Addle::aux_datatree::datatree_node_address(this); 
        }
        
        inline iterator begin() { return iterator(this); }
        inline const_iterator cbegin() const { return const_iterator(this); }
        inline const_iterator begin() const { return cbegin(); }
        
        inline iterator end() { return iterator(_end); }
        inline const_iterator cend() const { return const_iterator(_end); }
        inline const_iterator end() const { return cend(); }
        
        inline bool isLeaf() const { return std::holds_alternative<TLeaf>(_nodeData); }
        inline bool isBranch() const { return std::holds_alternative<branch_data_t>(_nodeData); }
        
        inline const auto& value() const { return value_impl<>(); }
        inline auto& value() { return value_impl<>(); }
        
        inline const auto& branchValue() const { return branchValue_impl<>(); }
        inline auto& branchValue() { return branchValue_impl<>(); }
        inline auto tryBranchValue() { return tryBranchValue_impl<>(); }
                
        inline const TLeaf& leafValue() const { return std::visit(dataVisitor_leafValue {}, _nodeData); }
        inline TLeaf& leafValue() { return std::visit(dataVisitor_leafValue {}, _nodeData); }
        inline std::optional<TLeaf> tryLeafValue() const { return std::visit(dataVisitor_tryLeafValue {}, _nodeData); }
        
        inline bool hasChildren() const {return std::visit(dataVisitor_countChildren {}, _nodeData) != 0; }
        
        inline child_range children() { return std::visit(dataVisitor_getChildren {}, _nodeData); }
        inline const_child_range children() const { return std::visit(dataVisitor_getChildren {}, _nodeData); }
        
        inline Node* firstChild() { return std::visit(dataVisitor_firstChildPtr {}, _nodeData); }
        inline const Node* firstChild() const { return std::visit(dataVisitor_firstChildPtr {}, _nodeData); }
        
        inline descendant_range descendants()
        {
            Node* firstChild = std::visit(dataVisitor_firstChildPtr {}, _nodeData);
            if (firstChild)
            {
                return descendant_range(firstChild, _end);
            }
            else
            {
                return descendant_range();
            }
        }
        
        inline const_descendant_range descendants() const
        {
            const Node* firstChild = std::visit(dataVisitor_firstChildPtr {}, _nodeData);
            if (firstChild)
            {
                return const_descendant_range(firstChild, _end);
            }
            else
            {
                return const_descendant_range();
            }
        }
        
        inline ancestor_range ancestors() { return ancestor_range( _parent, (Node*) nullptr ); }
        inline const_ancestor_range ancestors() const { return const_ancestor_range( (const Node*) _parent, (const Node*) nullptr ); }
        
        inline ancestor_range ancestorsAndSelf() { return ancestor_range( this, (Node*) nullptr ); }
        inline const_ancestor_range ancestorsAndSelf() const { return const_ancestor_range( this, (const Node*) nullptr ); }
        
        inline leaf_range leaves()
        { 
            using leaf_iterator_t = typename leaf_range::iterator;
            return leaf_range(
                    leaf_iterator_t ( this, _end ), leaf_iterator_t ( _end, _end )
                );
        }
        
        inline const_leaf_range leaves() const
        { 
            using leaf_iterator_t = typename const_leaf_range::iterator;
            return const_leaf_range(
                    leaf_iterator_t ( this, _end ), leaf_iterator_t ( _end, _end )
                );
        }
        
        
        inline branch_range branches()
        { 
            using branch_iterator_t = typename branch_range::iterator;
            return branch_range(
                    branch_iterator_t ( this, _end ), branch_iterator_t ( _end, _end )
                );
        }
        
        inline const_branch_range branches() const
        { 
            using branch_iterator_t = typename const_branch_range::iterator;
            return const_branch_range(
                    branch_iterator_t ( this, _end ), branch_iterator_t ( _end, _end )
                );
        }
        
        
        inline bool canAddChildren() const { return std::is_convertible<TLeaf, TBranch>::value || isBranch(); }

//         inline void addBranches(
//                 std::size_t count = 1,
//                 std::enable_if_t<std::is_void<TBranch>::value, void*> = nullptr
//             )
//         {
//             TODO
//         }
        
        template<typename Range>
        inline child_range addBranches(Range&& branchValues)
        {
            return insertBranches_impl(
                    std::visit(dataVisitor_countChildren {}, _nodeData),
                    std::forward<Range>(branchValues)
                );
        }
        
        template<typename T>
        inline child_range addBranches(const std::initializer_list<T>& branchValues)
        {
            return insertBranches_impl(
                    std::visit(dataVisitor_countChildren {}, _nodeData),
                    branchValues
                );
        }
        
        template<typename T, typename U = TBranch, std::enable_if_t<!std::is_void<U>::value, void*> = nullptr>
        inline Node& addBranch(T&& branchValue)
        {
            static_assert(std::is_constructible<TLeaf, T&&>::value);
            return addChild(
                    new Node(child_container_t {}, std::forward<T>(branchValue)) 
                );
        }
        
        inline Node& addBranch()
        {
            return addChild(
                    new Node(child_container_t {})
                );
        }
        
        inline child_range addBranchesCount(std::size_t count)
        {
            return insertBranchesCount_impl<>(
                    std::visit(dataVisitor_countChildren {}, _nodeData),
                    count
                );
        }
        
        template<typename Range>
        inline child_range addLeaves(Range&& leafValues)
        {
            return insertLeaves_impl(
                    std::visit(dataVisitor_countChildren {}, _nodeData),
                    std::forward<Range>(leafValues)
                );
        }
        
        template<typename T>
        inline child_range addLeaves(const std::initializer_list<T>& leafValues)
        {
            return insertLeaves_impl(
                    std::visit(dataVisitor_countChildren {}, _nodeData),
                    leafValues
                );
        }
        
        template<typename T = TLeaf>
        inline Node& addLeaf(T&& leafValue = {})
        {
            static_assert(std::is_constructible<TLeaf, T&&>::value);
            return addChild(
                    new Node(std::forward<T>(leafValue))
                );
        }
        
        template<
            typename Range,
            typename TLeaf_ = TLeaf,
            typename TBranch_ = TBranch,
            std::enable_if_t<
                std::is_convertible<TLeaf_, TBranch_>::value,
            void*> = nullptr
        >
        inline child_range addValues(Range&& values)
        {
            return insertLeaves_impl(
                    std::visit(dataVisitor_countChildren {}, _nodeData),
                    std::forward<Range>(values)
                );
        }
        
        template<
            typename T,
            typename TLeaf_ = TLeaf,
            typename TBranch_ = TBranch,
            std::enable_if_t<
                std::is_convertible<TLeaf_, TBranch_>::value,
            void*> = nullptr
        >
        inline child_range addValues(const std::initializer_list<T>& values)
        {
            return insertLeaves_impl(
                std::visit(dataVisitor_countChildren {}, _nodeData),
                values
            );
        }
        
        template<
            typename T = TLeaf,
            typename TLeaf_ = TLeaf,
            typename TBranch_ = TBranch,
            std::enable_if_t<
                std::is_convertible<TLeaf_, TBranch_>::value,
            void*> = nullptr
        >
        inline Node& addValue(T&& value = {})
        {
            return addLeaf(std::forward<T>(value));
        }
    
        // TODO: insert
        
    protected:
        explicit inline Node(const TLeaf& value)
            : _nodeData(value)
        {
        }
        
        explicit inline Node(TLeaf&& value)
            : _nodeData(std::move(value))
        {
        }
        
        template<typename U = TBranch, std::enable_if_t<!std::is_void<U>::value, void*> = nullptr>
        explicit inline Node(const child_container_t& children, const U& value = {})
            : _nodeData(branch_data_t { children, value })
        {
        }
        
        template<typename U = TBranch, std::enable_if_t<!std::is_void<U>::value, void*> = nullptr>
        explicit inline Node(child_container_t&& children, U&& value = {})
            : _nodeData(branch_data_t { std::move(children), std::move(value) })
        {
        }
        
        template<typename U = TBranch, std::enable_if_t<std::is_void<U>::value, void*> = nullptr>
        explicit inline Node(const child_container_t& children)
            : _nodeData(branch_data_t { children })
        {
        }
        
        
        template<typename U = TBranch, std::enable_if_t<std::is_void<U>::value, void*> = nullptr>
        explicit inline Node(child_container_t&& children)
            : _nodeData(branch_data_t { std::move(children) })
        {
        }
        
    private:
        struct dataVisitor_countChildren
        {
            std::size_t operator()(const branch_data_t& branchData) const
            {
                return branchData.children.size();
            }
            
            std::size_t operator()(const TLeaf&) const { return 0; }
        };
        
        struct dataVisitor_getChildren
        {
            template<
                    class BranchData,
                    std::enable_if_t<std::is_same<boost::remove_cv_ref_t<BranchData>, branch_data_t>::value,
                    void*> = nullptr
                >
            std::conditional_t<
                std::is_const<BranchData>::value, const_child_range, child_range
            > operator()(BranchData& branchData) const
            {
                return std::conditional_t<
                    std::is_const<BranchData>::value, const_child_range, child_range
                >(
                        branchData.children.begin(),
                        branchData.children.end()
                    );
            }

            template<
                    class T,
                    std::enable_if_t<std::is_same<boost::remove_cv_ref_t<T>, TLeaf>::value,
                    void*> = nullptr
                >
            std::conditional_t<
                std::is_const<T>::value, const_child_range, child_range
            > operator()(T&) const 
            { 
                return std::conditional_t<
                    std::is_const<T>::value, const_child_range, child_range
                >();
            }
        };
        
        struct insertChildrenHelper
        {
            inline insertChildrenHelper(
                    child_container_t& childContainer_,
                    std::size_t startIndex_,
                    std::size_t count_
                )
                : childContainer(childContainer_),
                startIndex(startIndex_),
                count(count_)
            {
                childContainer.insert(
                        childContainer.begin() + startIndex,
                        count,
                        nullptr
                    );
                
                begin = childContainer.data() + startIndex;
                end = childContainer.data() + startIndex + count;
            }
            
            inline void updateChildren(Node& parent) const
            {
                const std::size_t containerSize = childContainer.size();
                for (
                        std::size_t index = startIndex; 
                        index < containerSize;
                        ++index
                    )
                {
                    Node* child = childContainer[index];
                    
                    if (index < startIndex + count)
                    {
                        child->_parent = &parent;
                        child->_depth = parent._depth + 1;
                        child->_treeData = parent._treeData;
                    }
                    
                    if (index > 0)
                    {
                        childContainer[index - 1]->_end = child;
                    }
                    
                    child->_index = index;
                }
                
                Node* lastChild = childContainer.back();
                lastChild->_end = dfs_next_impl<false>(lastChild);
            }
            
            child_container_t& childContainer;
            
            std::size_t startIndex;
            std::size_t count;
            
            Node** begin;
            Node** end;
        };
        
        template<typename Range>
        struct dataVisitor_insertChildren
        {
            inline dataVisitor_insertChildren(
                    Node& owner_, 
                    std::size_t startIndex_,
                    Range&& range_
                )
                : owner(owner_), 
                startIndex(startIndex_),
                range(std::forward<Range>(range_))
            {
            }
            
            child_range operator()(branch_data_t& branchData) const
            {
                assert(startIndex <= branchData.children.size());
                
                auto&& begin = boost::begin(range);
                auto&& end = boost::end(range);
                
                if (begin == end)
                    return child_range();
                
                if constexpr (
                        aux_range_utils::getting_size_is_cheap<boost::remove_cv_ref_t<Range>>::value
                    )
                {
                    const std::size_t count = boost::size(range);
                    insertChildrenHelper helper(
                            branchData.children,
                            startIndex,
                            count
                        );
                    
                    if constexpr (
                            aux_range_utils::has_contiguous_storage<
                                boost::remove_cv_ref_t<Range>
                            >::value
                            && std::is_same<
                                typename boost::range_value<boost::remove_cv_ref_t<Range>>::type,
                                Node*
                            >::value
                        )
                    {
                        // e.g., if the TLeaf move constructor is potentially
                        // throwing, then insertLeaves will construct pointers
                        // into a QVarLengthArray<Node*, ...>.
                        std::memcpy( 
                                helper.begin, 
                                ::Addle::aux_range_utils
                                    ::get_contiguous_storage(range), 
                                count * sizeof(Node*)
                            );
                    }
                    else // range does not have contiguous storage
                    {
                        Node** j = helper.begin();
                        for (auto& i = begin; i != end; ++i)
                        {
                            *j = std::move(*i);
                            ++j;
                        }
                    }
                    
                    helper.updateChildren(owner);
                    
                    return child_range(
                            branchData.children.begin() + startIndex,
                            branchData.children.begin() + startIndex + count
                        );
                }
                else // getting size is not cheap
                {
                    QVarLengthArray<Node*, 256> children(
                            boost::begin(range),
                            boost::end(range)
                        );
                    
                    insertChildrenHelper helper(
                            branchData.children,
                            startIndex,
                            children.size()
                        );
                    
                    std::memcpy( 
                            helper.begin, 
                            children.data(), 
                            children.size() * sizeof(Node*)
                        );
                    
                    helper.updateChildren(owner);
                    
                    return child_range(
                            branchData.children.begin() + startIndex,
                            branchData.children.begin() + startIndex + children.size()
                        );
                }
            }
            
            child_range operator()(TLeaf& leafData) const
            {
                if constexpr (std::is_convertible<TLeaf, TBranch>::value)
                {
                    auto& branchData = owner._nodeData.template emplace<branch_data_t>(
                            child_container_t(),
                            std::move(leafData)
                        );
                    
                    return (*this)(branchData);
                }
                else // std::is_convertible<TLeaf, TBranch>::value
                {
                    assert(false); // TODO: exception
                }
            }
            
            Node& owner;
            std::size_t startIndex;
            Range&& range;
        };
        
        struct dataVisitor_insertBranchesCount
        {
            dataVisitor_insertBranchesCount(
                    Node& owner_, 
                    std::size_t startIndex_, 
                    std::size_t count_
            )
                : owner(owner_), 
                startIndex(startIndex_), 
                count(count_)
            {
            }
            
            child_range operator()(branch_data_t& branchData) const
            {
                assert(startIndex <= branchData.children.size());
                
                if (count == 0)
                    return child_range();
                
                insertChildrenHelper helper(
                        branchData.children,
                        startIndex,
                        count
                    );
                
                for (auto i = helper.begin; i < helper.end; ++i)
                    *i = new Node(child_container_t {});
                
                helper.updateChildren(owner);
                    
                return child_range(
                        branchData.children.begin() + startIndex,
                        branchData.children.begin() + startIndex + count
                    );
            }
            
            child_range operator()(TLeaf& leafData) const
            {
                if constexpr (std::is_convertible<TLeaf, TBranch>::value)
                {
                    auto& branchData = owner._nodeData.template emplace<branch_data_t>(
                            child_container_t {},
                            std::move(leafData)
                        );
                    
                    return (*this)(branchData);
                }
                else // std::is_convertible<TLeaf, TBranch>::value
                {
                    assert(false); // TODO: exception
                }
            }
            
            Node& owner;
            std::size_t startIndex;
            std::size_t count;
        };
        
        
        struct dataVisitor_deleteChildren
        {
            inline void operator()(branch_data_t& branchData) const
            {
                for (Node* child : boost::adaptors::reverse(noDetach(branchData.children)) )
                {
                    delete child;
                }
                
                branchData.children.clear();
            }
            
            inline void operator()(const TLeaf&) const {}
        };
        
        template<typename T>
        struct dataVisitor_value
        {
            template<
                    class BranchData,
                    std::enable_if_t<std::is_same<boost::remove_cv_ref_t<BranchData>, branch_data_t>::value,
                    void*> = nullptr
                >
            inline boost::copy_cv_t<T, BranchData>& operator()(BranchData& branchData) const
            {
                return branchData.value;
            }
            
            template<
                    class U,
                    std::enable_if_t<std::is_same<boost::remove_cv_ref_t<U>, TLeaf>::value,
                    void*> = nullptr
                >
            inline boost::copy_cv_t<T, U>& operator()(U& v) const
            {
                return v;
            }
        };
        
        template<typename T = TBranch>
        struct dataVisitor_branchValue
        {
            template<
                    class BranchData,
                    std::enable_if_t<std::is_same<boost::remove_cv_ref_t<BranchData>, branch_data_t>::value,
                    void*> = nullptr
                >
            inline boost::copy_cv_t<T, BranchData>& operator()(BranchData& branchData) const
            {
                return branchData;
            }
            
            template<
                    class U,
                    std::enable_if_t<std::is_same<boost::remove_cv_ref_t<U>, TLeaf>::value,
                    void*> = nullptr
                >
            [[noreturn]] inline boost::copy_cv_t<T, U>& operator()(U&) const
            {
                assert(false); // TODO: exception
            }
            
        };
        
        template<typename T = TBranch>
        struct dataVisitor_tryBranchValue
        {
            inline std::optional<T> operator()(const branch_data_t& branchData) const
            {
                return branchData.value;
            }
            
            inline std::optional<T> operator()(const TLeaf&) const
            {
                return std::optional<T>();
            }
        };
        
        struct dataVisitor_leafValue
        {
            template<
                    class BranchData,
                    std::enable_if_t<std::is_same<boost::remove_cv_ref_t<BranchData>, branch_data_t>::value,
                    void*> = nullptr
                >
            [[noreturn]] inline boost::copy_cv_t<TLeaf, BranchData>& operator()(BranchData&) const
            {
                assert(false); // TODO: exception
            }
            
            template<
                    class T,
                    std::enable_if_t<std::is_same<boost::remove_cv_ref_t<T>, TLeaf>::value,
                    void*> = nullptr
                >
            inline T& operator()(T& v) const
            {
                return v;
            }
        };
        
        struct dataVisitor_tryLeafValue
        {
            inline std::optional<TLeaf> operator()(const branch_data_t&) const
            {
                return std::optional<TLeaf>();
            }
            
            inline std::optional<TLeaf> operator()(const TLeaf& v) const
            {
                return v;
            }
        };
        
        struct dataVisitor_firstChildPtr
        {   
            template<
                    class BranchData,
                    std::enable_if_t<std::is_same<
                        boost::remove_cv_ref_t<BranchData>,
                        branch_data_t
                    >::value,
                    void*> = nullptr
                >
            inline boost::copy_cv_t<Node, BranchData>* operator()(BranchData& branchData) const
            {
                if (branchData.children.empty())
                    return nullptr;
                else
                    return branchData.children.front();
            }
            
            inline Node* operator()(TLeaf&) const { return nullptr; }
            inline const Node* operator()(const TLeaf&) const { return nullptr; }
        };
        
        struct dataVisitor_safeChildPtrAt
        {
            dataVisitor_safeChildPtrAt(std::size_t index_)
                : index(index_)
            {
            }
            
            template<
                    class BranchData,
                    std::enable_if_t<std::is_same<
                        boost::remove_cv_ref_t<BranchData>,
                        branch_data_t
                    >::value,
                    void*> = nullptr
                >
            inline boost::copy_cv_t<Node, BranchData>* operator()(BranchData& branchData) const
            {
                if (index < branchData.children.size())
                    return branchData.children[index];
                else
                    return nullptr;
            }
            
            inline Node* operator()(TLeaf&) const { return nullptr; }
            inline const Node* operator()(const TLeaf&) const { return nullptr; }
            
            std::size_t index;
        };
        
        struct dataVisitor_shallowEquivalent
        {
            inline bool operator()(const TLeaf& v1, const TLeaf& v2) const
            {
                return v1 == v2;
            }
            
            template<
                class T = TBranch,
                std::enable_if_t<
                    std::is_void<T>::value,
                void*> = nullptr
            >
            inline bool operator()(const branch_data_t&, const branch_data_t&) const
            {
                return true;
            }
                
            template<
                class BranchData,
                class T = TBranch,
                std::enable_if_t<
                    !std::is_void<T>::value
                    && std::is_same<
                        boost::remove_cv_ref_t<BranchData>,
                        branch_data_t
                    >::value,
                void*> = nullptr
            >
            inline bool operator()(const BranchData& d1, const BranchData& d2) const
            {
                return d1.value == d2.value;
            }
            
            template<
                class T = TBranch,
                std::enable_if_t<
                    std::is_void<T>::value,
                void*> = nullptr
            >
            inline bool operator()(const TLeaf&, const branch_data_t&) const
            {
                return false;
            }
            
            template<
                class T = TBranch,
                std::enable_if_t<
                    std::is_void<T>::value,
                void*> = nullptr
            >
            inline bool operator()(const branch_data_t&, const TLeaf&) const
            {
                return false;
            }
            
            template<
                class BranchData,
                class T = TBranch,
                std::enable_if_t<
                    !std::is_void<T>::value
                    && std::is_same<
                        boost::remove_cv_ref_t<BranchData>,
                        branch_data_t
                    >::value,
                void*> = nullptr
            >
            inline bool operator()(const TLeaf& leaf, const BranchData& branch) const
            {
                return leaf == branch.value;
            }
            
            template<
                class BranchData,
                class T = TBranch,
                std::enable_if_t<
                    !std::is_void<T>::value
                    && std::is_same<
                        boost::remove_cv_ref_t<BranchData>,
                        branch_data_t
                    >::value,
                void*> = nullptr
            >
            inline bool operator()(const BranchData& branch, const TLeaf& leaf) const
            {
                return branch.value == leaf;
            }
        };
        
        template<typename U = TLeaf, typename V = TBranch>
        inline const std::common_type_t<U, V>& value_impl() const
        {
            return std::visit( dataVisitor_value<std::common_type_t<U, V>> {}, _nodeData);
        }
        
        template<typename U = TLeaf, typename V = TBranch>
        inline std::common_type_t<U, V>& value_impl()
        {
            return std::visit( dataVisitor_value<std::common_type_t<U, V>> {}, _nodeData);
        }

        template<typename U = TBranch, std::enable_if_t<!std::is_void<U>::value, void*> = nullptr>
        inline const U& branchValue_impl() const
        {
            return std::visit(dataVisitor_branchValue<U> {}, _nodeData);
        }
        
        template<typename U = TBranch, std::enable_if_t<!std::is_void<U>::value, void*> = nullptr>
        inline U& branchValue_impl() 
        {
            return std::visit(dataVisitor_branchValue<U> {}, _nodeData);
        }
        
        template<typename U = TBranch, std::enable_if_t<!std::is_void<U>::value, void*> = nullptr>
        inline std::optional<U> tryBranchValue_impl() const
        {
            return std::visit(dataVisitor_tryBranchValue<U> {}, _nodeData);
        }
                
        template<typename Range>
        inline child_range insertChildren(std::size_t pos, Range&& children)
        {
            child_range result = std::visit(
                    dataVisitor_insertChildren<Range>(
                        *this, pos, std::forward<Range>(children)
                    ),
                    _nodeData
                );
            
            if (_treeData)
                _treeData->onChildrenInserted(result);
            
            return result;
        }
        
        inline Node& addChild(Node* child)
        {
            insertChild(std::visit(dataVisitor_countChildren {}, _nodeData), child);
            return *child;
        }
        
        inline Node& insertChild(std::size_t pos, Node* child)
        {
            using range_t = std::array<Node*, 1>;
            auto result = std::visit(
                    dataVisitor_insertChildren<range_t>(
                            *this, 
                            pos, 
                            range_t { child }
                        ), 
                    _nodeData
                );
            
            if (_treeData)
                _treeData->onChildrenInserted(result);
            
            return *child;
        }
        
        template<
            typename Range, 
            typename U = TBranch, 
            std::enable_if_t<!std::is_void<U>::value, void*> = nullptr
        >
        inline child_range insertBranches_impl(std::size_t pos, Range&& branchValues)
        {
            using namespace boost::adaptors;
            using T = typename boost::range_value<std::remove_reference_t<Range>>::type;
            static_assert(std::is_constructible<TLeaf, T&&>::value);
            
            assert(canAddChildren()); // exception?
            
            if constexpr(std::is_nothrow_constructible<U, T&&>::value)
            {
                return insertChildren(
                        pos,
                          branchValues
                        | transformed([] (auto&& v) -> Node* {
                                return new Node(
                                        child_container_t(),
                                        std::move(v)
                                    );
                            })
                    );
            }
            else // constructor may throw
            {
                QVarLengthArray<Node*, 256> nodes;
                reserve_for_size_if_cheap(nodes, branchValues);
                
                try 
                {
                    for (auto&& v : branchValues)
                    {
                        nodes.append(new Node(
                                child_container_t(),
                                std::move(v)
                            ));
                    }
                }
                catch(...)
                {
                    for (Node* node : boost::adaptors::reverse(nodes))
                        delete node;
                    
                    throw;
                }
                
                return insertChildren(pos, nodes);
            }
        }
        
        template<typename T = TBranch, 
            std::enable_if_t<
                std::is_void<T>::value
                || std::is_nothrow_default_constructible<TBranch>::value,
            void*> = nullptr>
        child_range insertBranchesCount_impl(std::size_t pos, std::size_t count)
        {
            child_range result =  std::visit(
                    dataVisitor_insertBranchesCount(*this, pos, count), 
                    _nodeData
                );
            
            if (_treeData)
                _treeData->onChildrenInserted(result);
            
            return result;
        }
        
        template<typename T = TBranch, 
            std::enable_if_t<
                !std::is_void<T>::value
                && !std::is_nothrow_default_constructible<T>::value,
            void*> = nullptr>
        child_range insertBranchesCount_impl(std::size_t pos, std::size_t count)
        {
            QVarLengthArray<Node*, 256> nodes;
            nodes.reserve(count);
            
            try 
            {
                for (std::size_t i = 0; i < count; ++i)
                {
                    nodes.append(new Node(child_container_t {}));
                }
            }
            catch (...)
            {
                for (Node* node : boost::adaptors::reverse(nodes))
                    delete node;
                
                throw;
            }
            insertChildren(pos, nodes);
        }
        
        template<typename Range>
        inline child_range insertLeaves_impl(std::size_t pos, Range&& leafValues)
        {
            using namespace boost::adaptors;
            using T = typename boost::range_value<std::remove_reference_t<Range>>::type;
            static_assert(std::is_constructible<TLeaf, T&&>::value);
            
            assert(canAddChildren()); // exception?
            
            if constexpr(std::is_nothrow_constructible<TLeaf, T&&>::value)
            {
                return insertChildren(
                        pos,
                          leafValues
                        | transformed([] (auto&& v) -> Node* {
                                return new Node(std::move(v));
                            })
                    );
            }
            else // constructor may throw
            {
                QVarLengthArray<Node*, 256> nodes;
                reserve_for_size_if_cheap(nodes, leafValues);
                
                try 
                {
                    for (auto&& v : leafValues)
                    {
                        nodes.append(new Node(
                                child_container_t {},
                                std::move(v)
                            ));
                    }
                }
                catch(...)
                {
                    for (Node* node : boost::adaptors::reverse(nodes))
                        delete node;
                    
                    throw;
                }
                
                return insertChildren(pos, nodes);
            }
        }
                
        Node* _parent = nullptr;
        
        std::size_t _index = 0;
        std::size_t _depth = 0;
        
        // the result of end(), i.e., equivalent to an iterator whose cursor is
        // the result of `dfs_next_impl<false>(this)`. This field is maintained
        // by this node's parent.
        Node* _end = nullptr;
        
        // TODO: In addition to _end, we could quite reasonably have a _next and 
        // _prev maintained at the same pace, which would make iterator
        // traversal cheaper (and bidirectional)
        
        mutable NodeAddress _address;
        
        TreeData* _treeData = nullptr;
        unsigned _cacheVersion = 0;
        
        bool _emancipated = false;
        
        node_data_t _nodeData;
        
        friend class DataTree<TLeaf, TBranch>;
        
        template<typename NodePointer>
        static inline auto parent_impl(NodePointer node)
        {
            return Q_LIKELY(node) ? node->_parent : nullptr;
        }
        
        friend Node* datatree_node_parent(Node* node) { return parent_impl(node); }
        friend const Node* datatree_node_parent(const Node* node) { return parent_impl(node); }
        
        friend std::size_t datatree_node_index(const Node* node) { return Q_LIKELY(node) ? node->_index : 0; } 
        friend std::size_t datatree_node_depth(const Node* node) { return Q_LIKELY(node) ? node->_depth : 0; } 
        
        friend std::size_t datatree_node_child_count(const Node* node)
        { 
            return Q_LIKELY(node) ? 
                std::visit( dataVisitor_countChildren {}, node->_nodeData ) 
                : 0;
        }
        
        template<typename NodePointer>
        static inline auto child_at_impl(NodePointer node, std::size_t index)
        {
            return Q_LIKELY(node) ?
                std::visit( dataVisitor_safeChildPtrAt(index), node->_nodeData )
                : nullptr;
        }
        
        friend Node* datatree_node_child_at(Node* node, std::size_t index) { return child_at_impl(node, index); }
        friend const Node* datatree_node_child_at(const Node* node, std::size_t index) { return child_at_impl(node, index); }
        
        template<bool Descend, typename NodePointer>
        static inline NodePointer dfs_next_impl(NodePointer node)
        {
            if (Q_UNLIKELY(!node))
                return nullptr;
            
            if constexpr (Descend)
            {
                NodePointer child = std::visit(
                        dataVisitor_firstChildPtr {},
                        node->_nodeData
                    );
                if (child)
                    return child;
            }
            
            NodePointer parent = const_cast<NodePointer>(node->_parent);
            if (parent)
            {
                NodePointer sibling = std::visit(
                        dataVisitor_safeChildPtrAt(node->_index + 1),
                        parent->_nodeData
                    );
                if (sibling)
                    return sibling;
            }
            
            return dfs_next_impl<false>(parent);
        }
        
        friend Node* datatree_node_dfs_next(Node* node) { return dfs_next_impl<true>(node); }
        friend const Node* datatree_node_dfs_next(const Node* node) { return dfs_next_impl<true>(node); }
        
        friend bool datatree_node_shallow_equivalent(const Node* node1, const Node* node2)
        {
            return node1 == node2
                || (
                        node1 && node2 
                        && std::visit(
                            dataVisitor_shallowEquivalent {},
                            node1->_nodeData,
                            node2->_nodeData
                        )
                    );
        }
        
        friend bool datatree_node_is_branch(const Node* node)
        {
            return node && node->isBranch();
        }
        
        friend bool datatree_node_is_leaf(const Node* node)
        {
            return node && node->isLeaf();
        }
        
        friend NodeAddress datatree_node_address(const Node* node)
        {
            return node ? node->address() : NodeAddress();
        }
    };
    
}

template<
        typename TLeaf,
        typename TBranch = TLeaf
    >
class DataTree
{
    struct Data;
public:
    using node_t = aux_datatree::Node<TLeaf, TBranch, Data>;
    
    using value_type        = typename node_t::value_type;
    using reference         = typename node_t::reference;
    using const_reference   = typename node_t::const_reference;
    using iterator          = typename node_t::iterator;
    using const_iterator    = typename node_t::const_iterator;
    using difference_type   = typename node_t::difference_type;
    using size_type         = typename node_t::size_type;
    
    using child_range       = typename node_t::child_range;
    using const_child_range = typename node_t::const_child_range;
    
    using descendant_range          = typename node_t::descendant_range;
    using const_descendant_range    = typename node_t::const_descendant_range;
    
    using ancestor_range        = typename node_t::ancestor_range;
    using const_ancestor_range  = typename node_t::const_ancestor_range;
        
    using leaf_range        = typename node_t::leaf_range;
    using const_leaf_range  = typename node_t::const_leaf_range;
    
    using branch_range        = typename node_t::branch_range;
    using const_branch_range  = typename node_t::const_branch_range;
    
    inline DataTree()
        : _data(new Data)
    {
    }
    
    ~DataTree() = default;
    
    DataTree(DataTree&&) = default;
    DataTree& operator=(DataTree&&) = default;
    
    DataTree(const DataTree&) = delete;
    DataTree& operator=(const DataTree&) = delete;
    
    inline node_t& root() { return _data->root; }
    inline const node_t& root() const { return _data->root; }
    
    inline std::size_t size() const { return _data->size; }
    
    inline iterator begin() { return _data->root.begin(); }
    inline const_iterator begin() const { return _data->root.begin(); }
    inline const_iterator cbegin() const { return _data->root.cbegin(); }
    
    inline iterator end() { return _data->root.end(); }
    inline const_iterator end() const { return _data->root.end(); }
    inline const_iterator cend() const { return _data->root.cend(); }
    
    inline leaf_range leaves() { return _data->root.leaves(); }
    inline const_leaf_range leaves() const { return _data->root.leaves(); }
    
    inline branch_range branches() { return _data->root.branches(); }
    inline const_branch_range branches() const { return _data->root.branches(); }
    
private:
    struct Data
    {
        inline Data()
            : root( typename node_t::child_container_t {} )
        {
            root._treeData = this;
        }
        
        inline void onChildrenInserted(const child_range& children)
        {
            size += boost::size(children);
            ++cacheVersion;
        }
        
        node_t root;
        std::size_t size = 1;
        
        unsigned cacheVersion = 0;
    };
    
    std::unique_ptr<Data> _data;
};

} // namespace Addle

