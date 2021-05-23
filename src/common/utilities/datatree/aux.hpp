#pragma once

#include <exception>
#include <iterator>

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>

#include <boost/range.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <boost/mp11.hpp>
#include <boost/type_traits.hpp>
#include <boost/type_traits/is_detected.hpp>
#include <boost/type_traits/is_detected_exact.hpp>
#include <boost/type_traits/is_detected_convertible.hpp>

#include "utilities/collections.hpp"
#include "utilities/metaprogramming.hpp"

#include <QtDebug>

namespace Addle {
namespace aux_datatree {

template<typename Tree>
struct datatree_traits
{
    // using node_handle = ...;
    // using const_node_handle = ...;
    
    // using value_type = ...;
};

template<typename Tree>
using node_handle_t = boost::mp11::mp_if<
        std::is_const<Tree>,
        typename datatree_traits<std::remove_const_t<Tree>>::const_node_handle,
        typename datatree_traits<Tree>::node_handle
    >;

template<typename Tree>
using const_node_handle_t = typename datatree_traits<Tree>::const_node_handle;

template<typename Tree>
using node_value_t = typename datatree_traits<Tree>::node_value_type;

// # Datatree API functions for tree traversal

template<typename Tree>
using _tree_root_t = decltype( datatree_root(std::declval<Tree&>()) );

template<typename Tree, std::enable_if_t<boost::is_detected<_tree_root_t, Tree>::value, void*> = nullptr>
inline node_handle_t<Tree> tree_root( Tree& tree ) { return datatree_root(tree); }
    
template<typename NodeHandle>
using _node_parent_t = decltype( datatree_node_parent(std::declval<NodeHandle>()) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_parent_t, NodeHandle>::value, void*> = nullptr>
inline std::decay_t<NodeHandle> node_parent( NodeHandle&& node ) { return datatree_node_parent( std::forward<NodeHandle>(node) ); }

template<typename NodeHandle>
inline auto node_children_begin( NodeHandle&& node ) { return datatree_node_children_begin( std::forward<NodeHandle>(node) ); }

template<typename NodeHandle>
using child_node_handle_t = std::decay_t<decltype( ::Addle::aux_datatree::node_children_begin( std::declval<NodeHandle>() ))>;

template<typename NodeHandle>
inline auto node_children_end( NodeHandle&& node ) { return datatree_node_children_end( std::forward<NodeHandle>(node) ); }

template<typename NodeHandle>
using _node_sibling_next_t = decltype( datatree_node_sibling_next(std::declval<NodeHandle>()) );

template<typename NodeHandle>
using _node_sibling_increment_t = decltype( datatree_node_sibling_increment(std::declval<NodeHandle&>()) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_sibling_next_t, NodeHandle>::value, void*> = nullptr>
inline std::decay_t<NodeHandle> node_sibling_next( NodeHandle&& node ) { return datatree_node_sibling_next( std::forward<NodeHandle>(node) ); }

template<
    typename NodeHandle, 
    std::enable_if_t<
        !std::is_const<NodeHandle>::value
        && boost::is_detected<_node_sibling_increment_t, NodeHandle>::value,
    void*> = nullptr>
inline void node_sibling_increment( NodeHandle& node ) { datatree_node_sibling_increment( node ); }

template<
    typename NodeHandle, 
    std::enable_if_t<
        !boost::is_detected<_node_sibling_next_t, NodeHandle>::value, void*> = nullptr>
inline std::decay_t<NodeHandle> node_sibling_next( NodeHandle&& node )
{ 
    std::decay_t<NodeHandle> result(std::forward<NodeHandle>(node));
    ::Addle::aux_datatree::node_sibling_increment( std::forward<NodeHandle>(node) ); 
    return result;
}

template<
    typename NodeHandle, 
    std::enable_if_t<
        !std::is_const<NodeHandle>::value
        && !boost::is_detected<_node_sibling_increment_t, NodeHandle>::value,
    void*> = nullptr>
inline void node_sibling_increment( NodeHandle& node ) { node = ::Addle::aux_datatree::node_sibling_next( std::move(node) ); }

template<typename NodeHandle>
using _node_sibling_increment_t_ = decltype( ::Addle::aux_datatree::node_sibling_increment(std::declval<NodeHandle&>()) );

template<typename NodeHandle>
using handle_is_sibling_incrementable = boost::is_detected<_node_sibling_increment_t_, NodeHandle>;

template<typename NodeHandle>
using _iterator_category_t = typename std::iterator_traits<
        boost::remove_cv_ref_t<NodeHandle>
    >::iterator_category;
        
template<typename NodeHandle>
using _handle_is_random_access_iterator = std::is_convertible<
        boost::mp11::mp_eval_or_q<
            void,
            boost::mp11::mp_bind<_iterator_category_t, boost::mp11::_1>,
            NodeHandle
        >,
        boost::random_access_traversal_tag
    >;

template<typename NodeHandle>
inline std::size_t node_child_count_impl( const NodeHandle& node ) 
{
    if (!node) return 0;
    
    auto cursor = ::Addle::aux_datatree::node_children_begin(node);
    auto&& end = ::Addle::aux_datatree::node_children_end(node);
    
    if constexpr (_handle_is_random_access_iterator<decltype(cursor)>::value)
    {
        return std::distance(cursor, end);
    }
    else
    {
        std::size_t result = 0;
        for (; cursor != end; ::Addle::aux_datatree::node_sibling_increment(cursor))
            ++result;
    
        return result;
    }
}

template<typename NodeHandle>
using _node_child_count_t = decltype( datatree_node_child_count(std::declval<NodeHandle>()) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_child_count_t, NodeHandle>::value, void*> = nullptr>
inline auto node_child_count( NodeHandle&& node ) { return datatree_node_child_count( std::forward<NodeHandle>(node) ); }

template<typename NodeHandle, std::enable_if_t<!boost::is_detected<_node_child_count_t, NodeHandle>::value, void*> = nullptr>
inline std::size_t node_child_count( const NodeHandle& node ) 
{
    return ::Addle::aux_datatree::node_child_count_impl(node);
}

template<typename NodeHandle>
inline NodeHandle node_child_at_impl( const NodeHandle& node, std::size_t index ) 
{
    auto cursor = ::Addle::aux_datatree::node_children_begin(node);
    auto&& end = ::Addle::aux_datatree::node_children_end(node);
    
    for (std::size_t i = 0; i < index; ++i)
    {
        ::Addle::aux_datatree::node_sibling_next(cursor);
        if (cursor == end) break;
    }
    
    return cursor;
}

template<typename NodeHandle>
using _node_child_at_t = decltype( datatree_node_child_at(std::declval<NodeHandle>(), std::declval<std::size_t>()) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_child_at_t, NodeHandle>::value, void*> = nullptr>
inline std::decay_t<NodeHandle> node_child_at( NodeHandle&& node, std::size_t index ) { return datatree_node_child_at( std::forward<NodeHandle>(node), index ); }

template<typename NodeHandle, std::enable_if_t<!boost::is_detected<_node_child_at_t, NodeHandle>::value, void*> = nullptr>
inline NodeHandle node_child_at( const NodeHandle& node, std::size_t index ) { return ::Addle::aux_datatree::node_child_at_impl(node, index); }


template<typename NodeHandle>
inline std::size_t node_index_impl( const NodeHandle& node ) 
{ 
    if (!node) return 0;
    
    NodeHandle parent = ::Addle::aux_datatree::node_parent(node);
    
    if (!parent) return 0;
    
    std::size_t result = 0;
    
    auto cursor = ::Addle::aux_datatree::node_children_begin(parent);
    auto&& end = ::Addle::aux_datatree::node_children_end(parent);
    
    while (cursor != node)
    {
        assert(cursor != end);
        ::Addle::aux_datatree::node_sibling_increment(cursor);
        ++result;
    }
    
    return result;
}

template<typename NodeHandle>
using _node_index_t = decltype( datatree_node_index(std::declval<NodeHandle>()) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_index_t, NodeHandle>::value, void*> = nullptr>
inline std::size_t node_index( NodeHandle&& node ) { return datatree_node_index( std::forward<NodeHandle>(node) ); }

template<typename NodeHandle, std::enable_if_t<!boost::is_detected<_node_index_t, NodeHandle>::value, void*> = nullptr>
inline std::size_t node_index( const NodeHandle& node ) { return ::Addle::aux_datatree::node_index_impl(node); }


template<typename NodeHandle>
inline std::size_t node_depth_impl( const NodeHandle& node )
{ 
    if (!node) return 0;
    
    std::size_t result = 0;
    NodeHandle cursor = ::Addle::aux_datatree::node_parent(node);
    
    while (cursor)
    {
        cursor = ::Addle::aux_datatree::node_parent(cursor);
        ++result;
    }
    
    return result;
}

template<typename NodeHandle>
using _node_depth_t = decltype( datatree_node_depth(std::declval<NodeHandle>()) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_depth_t, NodeHandle>::value, void*> = nullptr>
inline std::size_t node_depth( NodeHandle&& node ) { return datatree_node_depth( std::forward<NodeHandle>(node) ); }

template<typename NodeHandle, std::enable_if_t<!boost::is_detected<_node_depth_t, NodeHandle>::value, void*> = nullptr>
inline std::size_t node_depth( const NodeHandle& node ) { return ::Addle::aux_datatree::node_depth_impl( node ); }


template<typename NodeHandle>
inline NodeHandle node_root_impl( const NodeHandle& node )
{ 
    if (!node) return {};
    
    NodeHandle cursor(node);
    NodeHandle parent = ::Addle::aux_datatree::node_parent(node);
    
    while (parent)
    {
        cursor = parent;
        parent = ::Addle::aux_datatree::node_parent(parent);
    }
    
    return cursor;
}

template<typename NodeHandle>
using _node_root_t = decltype( datatree_node_root(std::declval<NodeHandle>()) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_root_t, NodeHandle>::value, void*> = nullptr>
inline std::decay_t<NodeHandle> node_root( NodeHandle&& node ) { return datatree_node_root( std::forward<NodeHandle>(node) ); }

template<typename NodeHandle, std::enable_if_t<!boost::is_detected<_node_root_t, NodeHandle>::value, void*> = nullptr>
inline NodeHandle node_root( const NodeHandle& node ) { return ::Addle::aux_datatree::node_root_impl( node ); }


template<typename NodeHandle>
using _node_dfs_begin_t = decltype( datatree_node_dfs_begin(std::declval<NodeHandle>()) );

template<typename NodeHandle>
using _node_parent_t_ = decltype( ::Addle::aux_datatree::node_parent(std::declval<NodeHandle>()) );

template<
    typename NodeHandle, 
    std::enable_if_t<
        !boost::is_detected<_node_dfs_begin_t, NodeHandle>::value
        && boost::is_detected<_node_parent_t_, NodeHandle>::value,
        void*
    > = nullptr>
inline NodeHandle node_dfs_begin( NodeHandle&& node ) { return node; }

template<typename NodeHandle>
using _node_dfs_end_t = decltype( datatree_node_dfs_end(std::declval<NodeHandle>()) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_dfs_end_t, NodeHandle>::value, void*> = nullptr>
inline auto node_dfs_end( NodeHandle&& node ) { return datatree_node_dfs_end( std::forward<NodeHandle>(node) ); }

template<
    typename NodeHandle,
    std::enable_if_t<std::is_convertible_v<child_node_handle_t<boost::remove_cv_ref_t<NodeHandle>>, NodeHandle&&>, void*> = nullptr
>
inline child_node_handle_t<boost::remove_cv_ref_t<NodeHandle>> _node_handle_to_child_handle( NodeHandle&& node ) { return std::forward<NodeHandle>(node); }

template<
    typename NodeHandle,
    std::enable_if_t<!std::is_convertible_v<child_node_handle_t<NodeHandle>, NodeHandle&&>, void*> = nullptr
>
inline child_node_handle_t<NodeHandle> _node_handle_to_child_handle( const NodeHandle& node )
{
    if (!node) return {};
    
    auto parent = ::Addle::aux_datatree::node_parent(node);
    
    if (!parent) return {};
    
    auto cursor = ::Addle::aux_datatree::node_children_begin(parent);
    
    if constexpr (
            _handle_is_random_access_iterator<child_node_handle_t<NodeHandle>>::value
            && boost::is_detected<_node_index_t, NodeHandle>::value
        )
    {
        // This code path is not guaranteed to be more efficient, but it is not 
        // likely to be worse.
        std::size_t index = ::Addle::aux_datatree::node_index(node);
        return std::advance(cursor, index);
    }
    else
    {
        auto&& siblingsEnd = ::Addle::aux_datatree::node_children_end(parent);
        while (cursor != node)
        {
            assert(cursor != siblingsEnd);
            ::Addle::aux_datatree::node_sibling_increment(cursor);
        }
        
        return cursor;
    }
}

template<
    typename NodeHandle, 
    std::enable_if_t<
        !boost::is_detected<_node_dfs_end_t, NodeHandle>::value
        && boost::is_detected<_node_parent_t_, NodeHandle>::value,
        void*
    > = nullptr>
inline NodeHandle node_dfs_end( const NodeHandle& node )
{
    auto cursor = _node_handle_to_child_handle(node);
    
    while (cursor)
    {
        NodeHandle parent = ::Addle::aux_datatree::node_parent(cursor);
        
        if (!parent) return {};
        
        auto&& siblingsEnd = ::Addle::aux_datatree::node_children_end(parent);
        
        ::Addle::aux_datatree::node_sibling_increment(cursor);
        
        if (cursor != siblingsEnd)
            return cursor;
        
        cursor = _node_handle_to_child_handle(parent);
    }
    
    return {};
}

template<typename NodeHandle>
using _node_dfs_next_t = decltype( datatree_node_dfs_next(std::declval<NodeHandle>()) );

template<typename NodeHandle>
using _node_dfs_increment_t = decltype( datatree_node_dfs_increment(std::declval<NodeHandle&>()) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_dfs_next_t, NodeHandle>::value, void*> = nullptr>
inline std::decay_t<NodeHandle> node_dfs_next( NodeHandle&& node ) { return datatree_node_dfs_next( std::forward<NodeHandle>(node) ); }

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_dfs_increment_t, NodeHandle>::value, void*> = nullptr>
inline void node_dfs_increment( NodeHandle& handle ) { datatree_node_dfs_increment( handle ); }

template<
    typename NodeHandle, 
    std::enable_if_t<
        !boost::is_detected<_node_dfs_next_t, NodeHandle>::value
        && !boost::is_detected<_node_dfs_increment_t, NodeHandle>::value, 
    void*> = nullptr>
inline std::decay_t<NodeHandle> node_dfs_next( const NodeHandle& node )
{ 
    if (!node) return {};
    
    auto&& childrenBegin = ::Addle::aux_datatree::node_children_begin(node);
    auto&& childrenEnd = ::Addle::aux_datatree::node_children_end(node);
    
    if (childrenBegin != childrenEnd)
        return childrenBegin;
    else
        return ::Addle::aux_datatree::node_dfs_end(node);
}

template<
    typename NodeHandle, 
    std::enable_if_t<
        !boost::is_detected<_node_dfs_next_t, NodeHandle>::value
        && boost::is_detected<_node_dfs_increment_t, NodeHandle>::value, 
    void*> = nullptr>
inline std::decay_t<NodeHandle> node_dfs_next( NodeHandle&& node )
{ 
    std::decay_t<NodeHandle> result(std::forward<NodeHandle>(node));
    ::Addle::aux_datatree::node_dfs_increment(result);
    
    return result;
}

template<
    typename NodeHandle, 
    std::enable_if_t<
        !boost::is_detected<_node_dfs_increment_t, NodeHandle>::value,
    void*> = nullptr>
inline void node_dfs_increment( NodeHandle& handle ) { handle = ::Addle::aux_datatree::node_dfs_next(std::move(handle)); }

template<typename NodeHandle>
using _node_dfs_increment_t_ = decltype( ::Addle::aux_datatree::node_dfs_increment(std::declval<NodeHandle&>()) );

template<typename NodeHandle>
using handle_is_dfs_incrementable = boost::is_detected<_node_dfs_increment_t_, NodeHandle>;


template<typename NodeHandle>
using _node_is_leaf_t = decltype( datatree_node_is_leaf(std::declval<NodeHandle>()) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_is_leaf_t, NodeHandle>::value, void*> = nullptr>
inline bool node_is_leaf( NodeHandle&& node ) { return datatree_node_is_leaf(std::forward<NodeHandle>(node)); }

template<typename NodeHandle, std::enable_if_t<!boost::is_detected<_node_is_leaf_t, NodeHandle>::value, void*> = nullptr>
inline bool node_is_leaf( const NodeHandle& node )
{ 
    return ::Addle::aux_datatree::node_children_begin(node) == ::Addle::aux_datatree::node_children_end(node);
}


template<typename NodeHandle>
using _node_is_branch_t = decltype( datatree_node_is_branch(std::declval<NodeHandle>()) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_is_branch_t, NodeHandle>::value, void*> = nullptr>
inline bool node_is_branch( NodeHandle&& node ) { return datatree_node_is_branch(std::forward<NodeHandle>(node)); }

template<typename NodeHandle, std::enable_if_t<!boost::is_detected<_node_is_branch_t, NodeHandle>::value, void*> = nullptr>
inline bool node_is_branch( const NodeHandle& node )
{ 
    return ::Addle::aux_datatree::node_children_begin(node) != ::Addle::aux_datatree::node_children_end(node);
}


template<typename NodeHandle>
inline auto node_value( NodeHandle&& node ) { return datatree_node_value(std::forward<NodeHandle>(node)); }

template<typename NodeHandle, typename T>
inline void node_set_value( NodeHandle&& node, T&& v) { return datatree_node_set_value(std::forward<NodeHandle>(node), std::forward<T>(v)); }

template<typename NodeHandle>
inline auto&& node_value_ref( NodeHandle&& node ) { return datatree_node_value_ref(std::forward<NodeHandle>(node)); }


template<
    typename NodeHandle,
    typename ChildHandle,
    typename T
>
using _datatree_node_insert_child_t = decltype( 
        datatree_node_insert_child(std::declval<NodeHandle>(), std::declval<ChildHandle>(), std::declval<T>()) 
    );

template<
    typename NodeHandle,
    typename ChildHandle,
    typename T
>
using _datatree_node_insert_children_t = decltype( 
        datatree_node_insert_children(std::declval<NodeHandle>(), std::declval<ChildHandle>(), std::declval<T>()) 
    );

template<
    typename NodeHandle,
    typename ChildHandle,
    typename T, 
    std::enable_if_t<boost::is_detected<_datatree_node_insert_child_t, NodeHandle, ChildHandle, T>::value, void*> = nullptr
>
inline auto node_insert_child( NodeHandle&& parent, ChildHandle&& pos, T&& childValue )
{
    return datatree_node_insert_child(
            std::forward<NodeHandle>(parent), 
            std::forward<ChildHandle>(pos), 
            std::forward<T>(childValue)
        );
}

template<
    typename NodeHandle,
    typename ChildHandle,
    typename T, 
    std::enable_if_t<!boost::is_detected<_datatree_node_insert_child_t, NodeHandle, ChildHandle, T>::value, void*> = nullptr
>
inline std::decay_t<ChildHandle> node_insert_child( NodeHandle&& parent, ChildHandle&& pos, T&& childValue )
{
    auto&& range = datatree_node_insert_children(
            std::forward<NodeHandle>(parent), 
            std::forward<ChildHandle>(pos), 
            make_inline_ref_range(std::forward<T>(childValue))
        );
    
    return boost::begin(range);
}

template<
    typename NodeHandle, 
    typename Range, 
    typename ChildHandle,
    std::enable_if_t<boost::is_detected<_datatree_node_insert_children_t, NodeHandle, ChildHandle, Range>::value, void*> = nullptr
>
inline auto node_insert_children( NodeHandle&& parent, ChildHandle&& pos, Range&& childValues )
{
    return datatree_node_insert_children(
            std::forward<NodeHandle>(parent), 
            std::forward<ChildHandle>(pos), 
            std::forward<Range>(childValues)
        );
}

template<
    typename NodeHandle, 
    typename Range, 
    typename ChildHandle,
    std::enable_if_t<!boost::is_detected<_datatree_node_insert_children_t, NodeHandle, ChildHandle, Range>::value, void*> = nullptr
>
inline std::pair<ChildHandle, ChildHandle> node_insert_children( NodeHandle&& parent, ChildHandle pos, const Range& childValues )
{
    ChildHandle begin = {};
    ChildHandle end = {};
    
    bool isFirst = true;
    
    for (auto&& v : childValues)
    {
        pos = datatree_node_insert_child(
                std::forward<NodeHandle>(parent), 
                pos, 
                std::forward<decltype(v)>(v)
            );
        
        if (isFirst)
        {
            begin = pos;
            isFirst = false;
        }
        
        end = pos;
    }
    
    if (end) 
        ::Addle::aux_datatree::node_sibling_increment(end);
    
    return std::make_pair(begin, end);
}


// Generically represents the location of a node in a tree as a sequence of
// 0-based integer indices.
//
// TODO: potential additional optimization would be to have the fast address not 
// only serve as a comparator proxy, but also *encode* the address instead of
// the QList where applicable. This could be a bit tricky to implement, but
// would mean the vast majority of practical NodeAddress instances would never
// need to allocate dynamic memory, which could make a lot of operations faster 
// and cheaper.
//
// A fairly simple approach would use a `std::variant` holding either a QList or
// a 7x8-bit array + 8-bit size hint. The total size would be ~9 bytes and there
// would be fairly tidy semantics for the various array vs. list case juggling.
// This would not have the guaranteed near-instant comparison speeds enjoyed by
// the uint64 fastaddress, but trusting modern compilers/processors to support
// vectorizing small integer operations, it will surely be good enough for us.
// 
// But the *real* white whale is getting the whole thing down to 8 bytes. While
// it's fun to imagine some kind of steganography reversibly hiding an extra bit
// of information on top of QList, an 8-byte NodeAddress is probably not
// possible without either meddling with Qt's private API or fully
// reimplementing an implicitly shared list container ourselves.
// 
// Ultimately an 8-byte NodeAddress is not terribly important, especially in
// Qt 6

class NodeAddress
{
    static constexpr std::uint64_t INVALID_FASTADDRESS = UINT64_MAX;
    
    using list_t = QList<std::size_t>;
public:        
    using value_type        = std::size_t;
    using const_reference   = const std::size_t&;
    using reference         = const_reference;
    using const_iterator    = typename list_t::const_iterator;
    using iterator          = const_iterator;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    
    
    NodeAddress() = default;
    NodeAddress(const NodeAddress&) = default;
    NodeAddress(NodeAddress&&) = default;
    
    inline NodeAddress(const list_t& address)
        : _address(address)
    {
        updateFastAddress();
    }
    
    inline NodeAddress(list_t&& address)
        : _address(std::move(address))
    {
        updateFastAddress();
    }
    
    inline NodeAddress(const std::initializer_list<std::size_t>& init)
        : _address(init)
    {
        updateFastAddress();
    }
    
    NodeAddress& operator=(const NodeAddress&) = default;
    NodeAddress& operator=(NodeAddress&&) = default;
    
    NodeAddress& operator=(const list_t& address)
    {
        _address = address;
        updateFastAddress();
        return *this;
    }
    
    NodeAddress& operator=(list_t&& address)
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
    
    inline list_t values() const    { return _address; }
    inline operator list_t() const  { return _address; }
    inline std::size_t operator[](std::size_t depth) const { return _address[depth]; }
    
    inline std::size_t firstIndex() const { assert(!_address.isEmpty()); return _address.first(); }
    inline std::size_t lastIndex() const { assert(!_address.isEmpty()); return _address.last(); }
    
    inline std::size_t size() const     { return _address.size(); }
    inline const_iterator begin() const { return _address.begin(); }
    inline const_iterator end() const   { return _address.end(); }
    
    inline bool isRoot() const { return _address.isEmpty(); }
    
    inline bool isAncestorOf(const NodeAddress& other) const
    {
        if (isRoot() && !other.isRoot())
            return true;
        else if (other.isRoot())
            return false;
        
        if (other.size() <= size())
            return false;
        
        for (std::size_t i = 0; i < size(); ++i)
        {
            if (other[i] != (*this)[i])
                return false;
        }
        
        return true;
    }
    
    inline bool isDescendantOf(const NodeAddress& other) const
    {
        return other.isAncestorOf(*this);
    }
    
    inline bool isParentOf(const NodeAddress& other) const
    { 
        return other.size() == size() + 1 && isAncestorOf(other);
    }
    
    inline bool isChildOf(const NodeAddress& other) const
    {
        return other.isParentOf(*this);
    }
    
    inline bool isSiblingOf(const NodeAddress& other) const
    {
        if (isRoot() && other.isRoot())
            return true;
        else if (isRoot() || other.isRoot())
            return false;
        
        if (other.size() != size())
            return false;
        
        for (std::size_t i = 0; i < size() - 1; ++i)
        {
            if (other[i] != (*this)[i])
                return false;
        }
        
        return true;
    }
    
    // useful for some dfs-based operations
    inline bool exceedsDescendantsOf(const NodeAddress& other) const
    {
        
        if (isRoot() || other.isRoot())
            return false;
        
        auto i = begin();
        auto end_ = end();
        
        auto j = other.begin();
        auto otherEnd = other.end();
        
        // find first index that differs between the addresses
        while (i != end_ && j != otherEnd)
        {
            if ((*i) != (*j)) break;
            ++i;
            ++j;
        }
        
        if (i == end_ || j == otherEnd) return false;
        
        return (*i) > (*j);
    }
    
    inline NodeAddress parent() const &
    {
        if (isRoot())
            return NodeAddress();
        else
            return NodeAddress( list_t(
                    _address.begin(),
                    _address.begin() + (_address.size() - 1)
                ));
    }
    
    inline NodeAddress parent() &&
    {
        if (isRoot())
        {
            return NodeAddress();
        }
        else
        {
            _address.removeLast();
            _fastAddress = 0;
            return NodeAddress(std::move(_address));
        }
    }
    
    inline NodeAddress nextSibling() const &
    {
        if (isRoot())
        {
            return NodeAddress();
        }
        else
        {
            list_t newAddress(_address);
            ++(newAddress.last());
            return NodeAddress(std::move(newAddress));
        }
    }
    
    inline NodeAddress nextSibling() &&
    {
        if (isRoot())
        {
            return NodeAddress();
        }
        else
        {
            ++(_address.last());
            _fastAddress = 0;
            return NodeAddress(std::move(_address));
        }
    }
    
    inline NodeAddress deltaSibling(int d) const &
    {
        if (isRoot())
        {
            return NodeAddress();
        }
        else
        {
            list_t newAddress(_address);
            (newAddress.last()) += d;
            return NodeAddress(std::move(newAddress));
        }
    }
    
    inline NodeAddress deltaSibling(int d) &&
    {
        if (isRoot())
        {
            return NodeAddress();
        }
        else
        {
            (_address.last()) += d;
            _fastAddress = 0;
            return NodeAddress(std::move(_address));
        }
    }
    
    inline NodeAddress prev() const &
    {
        if (isRoot())
        {
            return NodeAddress();
        }
        else
        {
            if (_address.last() == 0)
            {
                return parent();
            }
            else
            {
                list_t newAddress(_address);
                --(newAddress.last());
                return NodeAddress(std::move(newAddress));
            }
        }
    }
    
    inline NodeAddress prev() &&
    {
        if (isRoot())
        {
            return NodeAddress();
        }
        else
        {
            if (noDetach(_address).last() == 0)
            {
                return parent();
            }
            else
            {
                --(_address.last());
                _fastAddress = 0;
                return NodeAddress(std::move(_address));
            }
        }
    }
    
    inline NodeAddress operator<<(std::size_t index) const &
    {
        list_t newAddress(_address);
        newAddress.append(index);
        return NodeAddress(newAddress);
    }
            
    inline NodeAddress operator<<(const NodeAddress& other) const &
    {
        return NodeAddress(_address + other._address);
    }
            
    inline NodeAddress operator<<(const list_t& other) const &
    {
        return NodeAddress(_address + other);
    }
    
    inline NodeAddress operator<<(std::size_t index) &&
    {
        _fastAddress = 0;
        return NodeAddress(std::move(_address << index));
    }

    inline NodeAddress operator<<(const NodeAddress& other) &&
    {
        _fastAddress = 0;
        return NodeAddress(std::move(_address << other._address));
    }
    
    inline NodeAddress operator<<(const list_t& other) &&
    {
        _fastAddress = 0;
        return NodeAddress(std::move(_address << other));
    }
    
    inline void swap(NodeAddress& other)
    {
        _address.swap(other._address);
        std::swap(_fastAddress, other._fastAddress);
    }
    
    template<typename NodeHandle>
    static inline NodeAddress calculate(const NodeHandle& node)
    {
        NodeHandle cursor = node;
        NodeHandle parent = ::Addle::aux_datatree::node_parent(cursor);
        
        QList<std::size_t> address;
        address.reserve(::Addle::aux_datatree::node_depth(node));
            // TODO does this actually help if we're prepending everything?
            // It's just as easy to us to build the list forward then
            // reverse it.
        
        while (parent)
        {
            address.push_front(::Addle::aux_datatree::node_index(cursor));
            cursor = parent;
            parent = ::Addle::aux_datatree::node_parent(cursor);
        }
        
        return NodeAddress(std::move(address));
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
    
    list_t _address;
    
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
        
//         if (nodeAddress._fastAddress != INVALID_FASTADDRESS)
//         {
//             debug
//                 << "_fastAddress:"
//                 << qUtf8Printable(
//                     QString::asprintf("%0.16lx", nodeAddress._fastAddress)
//                 );
//         }
        
        return debug;
    }
#endif
};

template<typename NodeHandle>
using _node_address_t = decltype( datatree_node_address(std::declval<NodeHandle>()) );

template<typename NodeHandle, std::enable_if_t<boost::mp11::mp_valid<_node_address_t, NodeHandle>::value, void*> = nullptr>
inline NodeAddress node_address( NodeHandle&& node ) { return datatree_node_address( std::forward<NodeHandle>(node) ); }

template<typename NodeHandle, std::enable_if_t<!boost::mp11::mp_valid<_node_address_t, NodeHandle>::value, void*> = nullptr>
inline NodeAddress node_address( const NodeHandle& node )
{
    return NodeAddress::calculate(node);
}

template<typename NodeHandle>
inline bool node_contains_address_impl(const NodeHandle& root, const NodeAddress& address)
{
    NodeHandle cursor = root;
    for (std::size_t index : address)
    {
        if (::Addle::aux_datatree::node_child_count(cursor) <= index)
            return false;
    }
    return true;
}

template<typename NodeHandle>
using _node_contains_address_t = decltype( datatree_node_contains_address(std::declval<NodeHandle>(), std::declval<const NodeAddress&>()) );

template<typename NodeHandle, std::enable_if_t<boost::mp11::mp_valid<_node_contains_address_t, NodeHandle>::value, void*> = nullptr>
inline bool node_contains_address( NodeHandle&& node, const NodeAddress& address ) 
{ 
    return datatree_node_contains_address( std::forward<NodeHandle>(node), address); 
}

template<typename NodeHandle, std::enable_if_t<!boost::mp11::mp_valid<_node_contains_address_t, NodeHandle>::value, void*> = nullptr>
inline bool node_contains_address( const NodeHandle& node, const NodeAddress& address )
{
    return node_contains_address_impl(node, address);
}

template<typename NodeHandle>
inline std::decay_t<NodeHandle> node_lookup_address_impl(const NodeHandle& root, const NodeAddress& address)
{
    std::decay_t<NodeHandle> cursor(root);
    for (std::size_t index : address)
    {
        if ( ::Addle::aux_datatree::node_child_count(cursor) <= index)
            return NodeHandle {};
        
        cursor = ::Addle::aux_datatree::node_child_at(cursor, index);
    }
    
    return cursor;
}

template<typename NodeHandle>
using _node_lookup_address_t = decltype( datatree_node_lookup_address(std::declval<NodeHandle>(), std::declval<const NodeAddress&>()) );

template<typename NodeHandle, std::enable_if_t<boost::mp11::mp_valid<_node_lookup_address_t, NodeHandle>::value, void*> = nullptr>
inline std::decay_t<NodeHandle> node_lookup_address( NodeHandle&& node, const NodeAddress& address )
{ 
    return datatree_node_lookup_address( std::forward<NodeHandle>(node), address ); 
}

template<typename NodeHandle, std::enable_if_t<!boost::mp11::mp_valid<_node_lookup_address_t, NodeHandle>::value, void*> = nullptr>
inline std::decay_t<NodeHandle> node_lookup_address( const NodeHandle& node, const NodeAddress& address )
{
    return node_lookup_address_impl(node, address);
}

template<typename ExtendedNode>
struct _dfs_extended_node_index
{
    ExtendedNode root = {};
    QHash<NodeAddress, ExtendedNode> nodes;
    QHash<NodeAddress, ExtendedNode> ends;
    
//     struct _address_comp
//     {
//         inline bool operator()(
//                 const QSharedPointer<NodeAddress>& lhs, 
//                 const QSharedPointer<NodeAddress>& rhs
//             ) const
//         {
//             if (lhs && !rhs)
//                 return false;
//             else if (!lhs && rhs)
//                 return true;
//             else 
//                 return (*lhs < *rhs);
//         }
//     };
//     
//     std::set<QSharedPointer<NodeAddress>, _address_comp> addresses;
};

template<typename ExtendedNode>
class _indexed_dfs_extended_node_base
{
public:
    inline void clearIndex() 
    { 
        if (!_index) return;
        
        _index->nodes.clear();
        _index->ends.clear();
//         _index->addresses.clear();
    }
    
protected:    
    mutable QSharedPointer<_dfs_extended_node_index<ExtendedNode>> _index;
    //mutable QWeakPointer<NodeAddress> _address;
    
    inline void lazyInitIndex() const
    {
        if (!_index)
            _index = QSharedPointer<_dfs_extended_node_index<ExtendedNode>>(
                    new _dfs_extended_node_index<ExtendedNode>
                );
        
        if (!_index->root && static_cast<const ExtendedNode*>(this)->_root)
            _index->root = static_cast<const ExtendedNode*>(this)->_root;
    }
};

class _unindexed_dfs_extended_node_base {};

/**
 * For nodes that implement `datatree_node_parent`, it is possible to determine
 * the next step of a global depth-first search from any point in the tree.
 * 
 * For nodes that don't implement `datatree_node_parent`, e.g., nodes that may 
 * be shared between different parents in different trees, every depth-first
 * search is relative (or, at least, the global search cannot be determined),
 * and conducting such a search requires some state. 
 * 
 * DFSExtendedNode stores the state of a depth-first search relative to an
 * initial root node. This effectively builds parent linkage metadata on the
 * fly. In addition to being a state helper, this is also a node handle that
 * uses that metadata to extend the functionality of the node it wraps.
 * 
 * If Portable is false, the state data is stored using QVarLengthArray (which
 * avoids heap allocation except for very deep trees) and if true, the state
 * data is stored using QVector (which is generally much cheaper to copy and
 * move).
 * 
 * If Indexed is true (only allowed if Portable is true), the state for each
 * node is generated exactly once as it is encountered, and stored until all
 * DFSExtendedNode from the same tree are destroyed (this can be more efficient
 * for repeat random access of the tree, but much less efficient for the simple 
 * case).
 * 
 * DFSExtendedNode is not thread safe.
 */
template<typename NodeHandle, bool Portable = false, bool Indexed = false>
class DFSExtendedNode
    : public boost::mp11::mp_if_c<
            Indexed,
            _indexed_dfs_extended_node_base<DFSExtendedNode<NodeHandle, Portable, Indexed>>,
            _unindexed_dfs_extended_node_base
        >
{
    static_assert(Portable || !Indexed, "Only portable DFSExtendedNode may be indexed.");
    
    // Likely either a node handle or a ChildNodeIterator.
    using descendant_handle_t = child_node_handle_t<NodeHandle>;
    
    using descendant_stack_t = boost::mp11::mp_if_c<
            Portable,
            QVector<descendant_handle_t>,
            QVarLengthArray<descendant_handle_t, 64>
        >;
        
public:
    DFSExtendedNode() = default;
    DFSExtendedNode(const DFSExtendedNode&) = default;
    DFSExtendedNode(DFSExtendedNode&&) = default;
    
    DFSExtendedNode(const DFSExtendedNode<NodeHandle, !Portable, false>& other)
        : _root(other._root), 
        _descendants(other._descendants.begin(), other._descendants.end())
    {
    }
    
    DFSExtendedNode(DFSExtendedNode<NodeHandle, !Portable, false>&& other)
        : _root(std::move(other._root)), 
        _descendants(
            std::make_move_iterator(other._descendants.begin()), 
            std::make_move_iterator(other._descendants.end())
        )
    {
        other._descendants.clear();
    }
    
    DFSExtendedNode& operator=(const DFSExtendedNode&) = default;
    DFSExtendedNode& operator=(DFSExtendedNode&&) = default;
    
    DFSExtendedNode(const NodeHandle& node)
        : _root(node)
    {
    }
    
    DFSExtendedNode(NodeHandle&& node)
        : _root(std::move(node))
    {
    }
    
    DFSExtendedNode& operator=(const NodeHandle& node)
    {
        _descendants.clear();
        _root = node;
        return *this;
    }
    
    DFSExtendedNode& operator=(NodeHandle&& node)
    {
        _descendants.clear();
        _root = std::move(node);
        return *this;
    }
    
    operator NodeHandle () const { return currentNode(); }
    
    explicit operator bool () const { return static_cast<bool>(currentNode()); }
    bool operator!() const { return !currentNode(); }
    
    bool operator==(const DFSExtendedNode& other) const
    { 
        return _root == other._root && _descendants == other._descendants;
    }
    
    bool operator!=(const DFSExtendedNode& other) const { return !(*this == other); }
    
    auto&& operator*() const { return *currentNode(); }
    
    NodeAddress address() const
    {
        // It's necessary to calculate the address internally
        // - to avoid the implicit deep copy involved in making a parent cursor
        //   of type DFSExtendedNode, as NodeAddress::calculate would do
        // - for indexed DFSExtendedNode, to avoid the circular dependency of
        //   looking up a parent in the index and getting a node's parent to
        //   build its address.
        
        QList<std::size_t> result;
        result.reserve(_descendants.size());
        
        NodeHandle parentCursor = _root;
        for (const auto& descendant : _descendants)
        {
            std::size_t index;
            
            descendant_handle_t firstSibling = 
                ::Addle::aux_datatree::node_children_begin(parentCursor);
            
            if constexpr (_handle_is_random_access_iterator<descendant_handle_t>::value)
            {
                index = std::distance(
                        firstSibling, 
                        descendant
                    );
            }
            else
            {
                index = 0;
                
                while (firstSibling != descendant)
                {
                    ::Addle::aux_datatree::node_sibling_increment(firstSibling);
                    ++index;
                }
            }
            
            result.append(index);
            
            parentCursor = static_cast<NodeHandle>(descendant);
        }
        
        return result;
    }
    
private:
    NodeHandle _root = {};
    descendant_stack_t _descendants;
    
    NodeHandle currentNode() const
    {
        if (Q_UNLIKELY(_descendants.isEmpty()))
            return _root;
        else
            return static_cast<NodeHandle>(_descendants.last());
    }
    
    void moveToEnd_impl()
    {
        while (!_descendants.isEmpty())
        {
            auto ri = std::make_reverse_iterator(_descendants.end());
            auto rend = std::make_reverse_iterator(_descendants.begin());
            
            descendant_handle_t& cursor = *ri;
            ++ri;
            
            descendant_handle_t siblingsEnd = Q_UNLIKELY(ri == rend) ?
                ::Addle::aux_datatree::node_children_end(_root) :
                ::Addle::aux_datatree::node_children_end(
                        static_cast<const NodeHandle&>(*ri)
                    );
                
            ::Addle::aux_datatree::node_sibling_increment(cursor);
                
            if (cursor != siblingsEnd)
                return;
            
            _descendants.removeLast();
        }
        
        _root = {};
    }
    
    void moveToEnd()
    {
        if constexpr (Indexed)
        {
            auto beginAddress = address();
            
            if (this->_index && this->_index->ends.contains(beginAddress))
            {
                (*this) = qAsConst(this->_index->ends)[beginAddress];
            }
            else
            {
                moveToEnd_impl();
                
                this->lazyInitIndex();
                this->_index->ends[beginAddress] = *this;
                
                if (_root)
                    this->_index->nodes[address()] = *this;
            }
        }
        else
        {
            moveToEnd_impl();
        }
    }
    
    void moveToParent()
    {
        if (_descendants.isEmpty())
        {
            _root = {};
        }
        else
        {
            if constexpr (Indexed)
            {
                auto parentAddress = address().parent();
                
                if (this->_index && this->_index->nodes.contains(parentAddress))
                {
                    (*this) = qAsConst(this->_index->nodes)[parentAddress];
                }
                else
                {
                    _descendants.removeLast();
                    this->_index->nodes[parentAddress] = (*this);
                }
            }
            else
            {
                _descendants.removeLast();
            }
        }
    }
    
    void moveToChildrenBegin()
    {
        if (!_root)
            return;
        
        if constexpr (Indexed)
        {
            auto childAddress = address() << 0;
            if (this->_index && this->_index->nodes.contains(childAddress))
            {
                (*this) = qAsConst(this->_index->nodes)[childAddress];
            }
            else
            {
                _descendants.append(
                        ::Addle::aux_datatree::node_children_begin(currentNode())
                    );
                
                this->lazyInitIndex();
                this->_index->nodes[childAddress] = (*this);
            }
        }
        else
        {
            _descendants.append(
                    ::Addle::aux_datatree::node_children_begin(currentNode())
                );
        }
    }
    
    void moveToChildrenEnd()
    {
        if (!_root)
            return;
        
        _descendants.append(
                ::Addle::aux_datatree::node_children_end(currentNode())
            );
        
    }
    
    friend bool operator==(const DFSExtendedNode& lhs, const NodeHandle& rhs) { return lhs.currentNode() == rhs; }
    friend bool operator!=(const DFSExtendedNode& lhs, const NodeHandle& rhs) { return lhs.currentNode() != rhs; }
    
    friend bool operator==(const NodeHandle& lhs, const DFSExtendedNode& rhs) { return rhs == lhs.currentNode(); }
    friend bool operator!=(const NodeHandle& lhs, const DFSExtendedNode& rhs) { return rhs != lhs.currentNode(); }
    
    friend DFSExtendedNode datatree_node_children_begin(DFSExtendedNode&& node)
    {
        if (!node._root)
            return DFSExtendedNode {};
        
        node.moveToChildrenBegin();
        return std::move(node);
    }
    
    friend DFSExtendedNode datatree_node_children_begin(const DFSExtendedNode& node)
    {
        if (!node._root)
            return DFSExtendedNode {};
            
        if constexpr (Indexed) node.lazyInitIndex();
        
        DFSExtendedNode result(node);
        result.moveToChildrenBegin();
        return result;
    }
    
    friend DFSExtendedNode datatree_node_children_end(DFSExtendedNode&& node)
    {
        if (!node._root)
            return DFSExtendedNode {};
        
        node.moveToChildrenEnd();
        return std::move(node);
    }
    
    friend DFSExtendedNode datatree_node_children_end(const DFSExtendedNode& node)
    {
        if (!node._root)
            return DFSExtendedNode {};
            
        if constexpr (Indexed) node.lazyInitIndex();
        
        DFSExtendedNode result(node);
        result.moveToChildrenEnd();
        return result;
    }
    
    friend void datatree_node_sibling_increment(DFSExtendedNode& node)
    {
        if (!node._root)
            return;
        
        if (node._descendants.isEmpty())
        {
            node._root = {};
            return;
        }
    
        if constexpr (Indexed)
        {
            auto siblingAddress = node.address().nextSibling();
            
            if (node._index && node._index->nodes.contains(siblingAddress))
            {
                node = qAsConst(node._index->nodes)[siblingAddress];
            }
            else
            {
                auto ri = std::make_reverse_iterator(node._descendants.end());
                auto rend = std::make_reverse_iterator(node._descendants.begin());
                
                descendant_handle_t& cursor = *ri;
                ++ri;
                
                descendant_handle_t siblingsEnd = Q_UNLIKELY(ri == rend) ?
                    ::Addle::aux_datatree::node_children_end(node._root) :
                    ::Addle::aux_datatree::node_children_end(
                            static_cast<const NodeHandle&>(*ri)
                        );
                    
                ::Addle::aux_datatree::node_sibling_increment(cursor);
                    
                if (siblingsEnd != cursor)
                {
                    node.lazyInitIndex();
                    node._index->nodes[siblingAddress] = node;
                }
            }
        }
        else
        {
            ::Addle::aux_datatree::node_sibling_increment(
                    node._descendants.last()
                );
        }
    }
    
    friend void datatree_node_dfs_increment(DFSExtendedNode& node)
    {
        if (!node._root)
            return;
        
        if constexpr (Indexed) node.lazyInitIndex();
        
        auto&& childrenBegin = node_children_begin(node.currentNode());
        auto&& childrenEnd = node_children_end(node.currentNode());
        
        if (childrenBegin != childrenEnd)
        {
            node._descendants.append(std::move(childrenBegin));
            if constexpr (Indexed)
                node._index->nodes[node.address()] = node;
        }
        else
        {
            node.moveToEnd();
        }
    }
    
    friend DFSExtendedNode datatree_node_dfs_end(DFSExtendedNode&& node)
    {
        if (!node._root)
            return DFSExtendedNode {};
            
        node.moveToEnd();
        return std::move(node);
    }
    
    friend DFSExtendedNode datatree_node_dfs_end(const DFSExtendedNode& node)
    {
        if (!node._root)
            return DFSExtendedNode {};
            
        if constexpr (Indexed) node.lazyInitIndex();
        
        DFSExtendedNode result(node);
        result.moveToEnd();
        return result;
    }
    
    friend DFSExtendedNode datatree_node_parent(DFSExtendedNode&& node)
    {
        if (!node._root)
            return DFSExtendedNode {};
            
        node.moveToParent();
        return std::move(node);
    }

    friend DFSExtendedNode datatree_node_parent(const DFSExtendedNode& node)
    {
        if (!node._root)
            return DFSExtendedNode {};
            
        if constexpr (Indexed) node.lazyInitIndex();
        
        DFSExtendedNode result(node);
        result.moveToParent();
        return result;
    }
    
    friend std::size_t datatree_node_depth(const DFSExtendedNode& node)
    {
        if (!node._root) 
            return 0;
        else
            return node._descendants.size() + 1;
    }
    
    friend std::size_t datatree_node_index(const DFSExtendedNode& node)
    {
        if (!node._root || node._descendants.isEmpty())
        {
            return 0;
        }
        else
        {
            auto ri = std::make_reverse_iterator(node._descendants.end());
            auto rend = std::make_reverse_iterator(node._descendants.begin());
            
            ++ri;
            
            descendant_handle_t firstSibling = Q_UNLIKELY(ri == rend) ?
                ::Addle::aux_datatree::node_children_begin(node._root) :
                ::Addle::aux_datatree::node_children_begin(
                        static_cast<const NodeHandle&>(*ri)
                    );
                
            const descendant_handle_t& currentChildHandle = node._descendants.last();
                
            if constexpr (_handle_is_random_access_iterator<descendant_handle_t>::value)
            {
                return std::distance(
                        firstSibling, 
                        currentChildHandle
                    );
            }
            else
            {
                std::size_t result = 0;
                
                while (firstSibling != currentChildHandle)
                {
                    ::Addle::aux_datatree::node_sibling_increment(firstSibling);
                    ++result;
                }
                
                return result;
            }
        }
    }
    
    friend bool datatree_node_contains_address(const DFSExtendedNode& node, const NodeAddress& address)
    {
        if (!node._root) 
            return false;
        
        if constexpr (Indexed)
        {
            if (node._index && node._index->nodes.contains(address))
            {
                return true;
            }
            else
            {
                DFSExtendedNode result = node_lookup_address_impl(node, address);
                
                if (result)
                {
                    node.lazyInitIndex();
                    node._index->nodes[address] = result;
                }
                
                return static_cast<bool>(result);
            }
        }
        else
        {
            return node_contains_address_impl(node, address);
        }
    }
    
    friend DFSExtendedNode datatree_node_lookup_address(const DFSExtendedNode& node, const NodeAddress& address)
    {
        if (!node._root) 
            return DFSExtendedNode {};
            
        if constexpr (Indexed)
        {
            if (node._index && node._index->nodes.contains(address))
            {
                return qAsConst(node._index->nodes)[address];
            }
            else
            {
                DFSExtendedNode result =::Addle::aux_datatree::node_lookup_address_impl(node, address);
                
                if (result)
                {
                    node.lazyInitIndex();
                    node._index->nodes[address] = result;
                }
                
                return result;
            }
        }
        else
        {
            return ::Addle::aux_datatree::node_lookup_address_impl(node, address);
        }
    }
    
    friend NodeAddress datatree_node_address(const DFSExtendedNode& node)
    {
        return node.address();
    }
    
    friend class DFSExtendedNode<NodeHandle, !Portable>;
    friend class _indexed_dfs_extended_node_base<DFSExtendedNode<NodeHandle, Portable, Indexed>>;
};

template<
    typename NodeHandle, 
    std::enable_if_t<
        !boost::is_detected<_node_dfs_begin_t, NodeHandle>::value
        && !boost::is_detected<_node_parent_t_, NodeHandle>::value,
        void*
    > = nullptr>
inline DFSExtendedNode<boost::remove_cv_ref_t<NodeHandle>> 
    node_dfs_begin( NodeHandle&& node ) 
{
    return DFSExtendedNode<boost::remove_cv_ref_t<NodeHandle>>( 
            std::forward<NodeHandle>(node) 
        ); 
}

template<
    typename NodeHandle, 
    std::enable_if_t<
        !boost::is_detected<_node_dfs_end_t, NodeHandle>::value
        && !boost::is_detected<_node_parent_t_, NodeHandle>::value,
        void*
    > = nullptr>
inline DFSExtendedNode<boost::remove_cv_ref_t<NodeHandle>> 
    node_dfs_end( NodeHandle&& ) 
{
    return DFSExtendedNode<boost::remove_cv_ref_t<NodeHandle>>(); 
}


template<typename NodeHandle, typename ChildHandle>
using _node_remove_children_t = decltype( 
        datatree_node_remove_children( std::declval<NodeHandle>(), std::declval<ChildHandle>(), std::declval<ChildHandle>())
    );

template<
    typename NodeHandle, 
    typename ChildHandle,
    std::enable_if_t<
        boost::is_detected<_node_remove_children_t, NodeHandle, ChildHandle>::value,
    void*> = nullptr
>
inline void node_remove_children( NodeHandle node, ChildHandle begin, ChildHandle end)
{
    datatree_node_remove_children(
            std::forward<NodeHandle>(node), 
            std::forward<ChildHandle>(begin),
            std::forward<ChildHandle>(end)
        );
}

// TODO: possibly move echo into its own header? or have an algorithms header?

struct echo_default_make_node_tag {};
struct echo_default_after_make_node_tag {};

// Generic one-stop implementation for algorithms that involve traversing one
// tree and modifying another accordingly. Behavior is extended by providing
// callables for various events and conditionals.
// 
template<
        class SourceTree,
        class DestTree,
        typename MakeNode,
        typename AfterMakeNode
    >
class echo_impl
{
public:
    const_node_handle_t<SourceTree> sourceRoot;
    node_handle_t<DestTree> destRoot;
    
    MakeNode&& makeNode;
    AfterMakeNode&& afterMakeNode;
    
    
    void operator()() const
    {
        try
        {
            populateBranch_impl(sourceRoot, destRoot);
        }
        catch(...)
        {
            // TODO this algorithm should be able to work on trees with existing
            // nodes
            ::Addle::aux_datatree::node_remove_children(
                    destRoot, 
                    ::Addle::aux_datatree::node_children_begin(destRoot),
                    ::Addle::aux_datatree::node_children_end(destRoot)
                );  
            throw;
        }
    }
    
private:
    template<typename F, typename... Args>
    using _f_t = decltype( std::declval<F>()( std::declval<Args>()... ) );
    
    void make_node_impl(
            const_node_handle_t<SourceTree> sourceNode, 
            node_handle_t<DestTree> destParent,
            std::size_t pos
        ) const
    {
        node_handle_t<DestTree> destNode;
        
        if constexpr (std::is_same<boost::remove_cv_ref_t<MakeNode>, echo_default_make_node_tag>::value)
        {
            destNode = ::Addle::aux_datatree::node_insert_child(
                    destParent,
                    pos,
                    node_value_t<DestTree> {}
                );
        }
        else
        {
            destNode = ::Addle::aux_datatree::node_insert_child(
                    destParent,
                    pos,
                    this->makeNode( ::Addle::aux_datatree::node_value(sourceNode) )
                );
        }
        
        if (::Addle::aux_datatree::node_is_branch(sourceNode))
            populateBranch_impl(sourceNode, destNode);
        
        if constexpr (!std::is_same<boost::remove_cv_ref_t<AfterMakeNode>, echo_default_after_make_node_tag>::value)
        {
            if constexpr (boost::is_detected<
                    _f_t, AfterMakeNode, node_handle_t<DestTree>, const_node_handle_t<SourceTree>
                >::value)
            {
                this->afterMakeNode(destNode, sourceNode);
            }
            else
            {
                this->afterMakeNode(destNode);
            }
        }
    }
    
    void populateBranch_impl(
            const_node_handle_t<SourceTree> sourceBranch, 
            node_handle_t<DestTree> destBranch
        ) const
    {
        std::size_t pos = 0;
        
        auto&& sourceEnd = ::Addle::aux_datatree::node_children_end(sourceBranch);
        for (
                auto sourceChild = ::Addle::aux_datatree::node_children_begin(sourceBranch);
                sourceChild != sourceEnd; 
                ::Addle::aux_datatree::node_sibling_increment(sourceChild)
            )
        {
            this->make_node_impl(sourceChild, destBranch, pos);
            ++pos;
        }
    }
};



template<typename T>
using _dereferenced_t = decltype( *std::declval<T>() );

template<typename Tree, bool IsConst = false>
class NodeIterator
    : public boost::iterator_facade<
        NodeIterator<Tree, IsConst>,
        std::remove_reference_t<
            _dereferenced_t<
                boost::mp11::mp_if_c<
                    IsConst,
                    const_node_handle_t<Tree>,
                    node_handle_t<Tree>
                >
            >
        >,
        boost::forward_traversal_tag,
        _dereferenced_t<
            boost::mp11::mp_if_c<
                IsConst,
                const_node_handle_t<Tree>,
                node_handle_t<Tree>
            >
        >
    >
{
    using _pure_handle_t = boost::mp11::mp_if_c<
            IsConst,
            const_node_handle_t<Tree>,
            node_handle_t<Tree>
        >;
    
    using handle_t = boost::mp11::mp_if<
            boost::mp11::mp_valid<_node_parent_t, _pure_handle_t>,
            _pure_handle_t,
            DFSExtendedNode<_pure_handle_t, true>
        >;
        
public:
    NodeIterator() = default;
    NodeIterator(const NodeIterator&) = default;
    NodeIterator(NodeIterator&&) = default;
    
    NodeIterator(handle_t cursor)
        : _cursor(cursor)
    {
    }
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, NodeIterator<Tree, false>>::value, 
            void*
        > = nullptr
    >
    NodeIterator(const MutableIterator& i)
        : _cursor(i._cursor)
    {
    }
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, NodeIterator<Tree, false>>::value,
            void*
        > = nullptr
    >
    NodeIterator(MutableIterator&& i)
        : _cursor(std::move(i._cursor))
    {
    }
    
    NodeIterator& operator=(const NodeIterator&) = default;
    NodeIterator& operator=(NodeIterator&&) = default;
    
    inline operator const handle_t& () const { return _cursor; }
    
private:
    inline _dereferenced_t<handle_t> dereference() const { return *_cursor; }
    inline bool equal(const NodeIterator& x) const { return _cursor == x._cursor; }
    inline bool equal(const handle_t& x) const { return _cursor == x; }
    inline void increment() { ::Addle::aux_datatree::node_dfs_increment(_cursor); }
    
    handle_t _cursor = {};
    
    friend class NodeIterator<Tree, true>;
    friend class boost::iterator_core_access;
};

template<typename Tree, bool IsConst = false>
inline NodeIterator<Tree, IsConst> node_dfs_next(const NodeIterator<Tree, IsConst>& iter)
{ 
    return ++NodeIterator<Tree, IsConst>(iter); 
}

template<typename Tree, bool IsConst = false>
inline NodeIterator<Tree, IsConst> node_dfs_next(NodeIterator<Tree, IsConst>&& iter)
{ 
    return ++iter; 
}

template<typename Tree, bool IsConst = false>
inline void node_dfs_increment(NodeIterator<Tree, IsConst>& iter) { ++iter; }

template<typename Tree>
using ConstNodeIterator = NodeIterator<Tree, true>;

template<typename Tree>
using NodeRange = boost::iterator_range<NodeIterator<Tree>>;

template<typename Tree>
using ConstNodeRange = boost::iterator_range<ConstNodeIterator<Tree>>;

template<typename Tree, bool IsConst = false>
class ChildNodeIterator
    : public boost::iterator_facade<
        ChildNodeIterator<Tree, IsConst>,
        std::remove_reference_t<
            _dereferenced_t<
                boost::mp11::mp_if_c<
                    IsConst,
                    const_node_handle_t<Tree>,
                    node_handle_t<Tree>
                >
            >
        >,
        boost::random_access_traversal_tag,
        _dereferenced_t<
            boost::mp11::mp_if_c<
                IsConst,
                const_node_handle_t<Tree>,
                node_handle_t<Tree>
            >
        >
    >
{
    using handle_t = boost::mp11::mp_if_c<
            IsConst,
            const_node_handle_t<Tree>,
            node_handle_t<Tree>
        >;
public:
    ChildNodeIterator() = default;
    ChildNodeIterator(const ChildNodeIterator&) = default;
    ChildNodeIterator(ChildNodeIterator&&) = default;
    
    ChildNodeIterator(handle_t cursor)
        : _cursor(cursor)
    {
    }
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, ChildNodeIterator<Tree, false>>::value, 
            void*
        > = nullptr
    >
    ChildNodeIterator(const MutableIterator& i)
        : _cursor(i._cursor)
    {
    }
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, ChildNodeIterator<Tree, false>>::value,
            void*
        > = nullptr
    >
    ChildNodeIterator(MutableIterator&& i)
        : _cursor(std::move(i._cursor))
    {
    }
    
    ChildNodeIterator& operator=(const ChildNodeIterator&) = default;
    ChildNodeIterator& operator=(ChildNodeIterator&&) = default;
    
    inline operator const handle_t& () const { return _cursor; }
    
private:
    inline _dereferenced_t<handle_t> dereference() const { return *_cursor; }
    inline bool equal(const ChildNodeIterator& x) const { return _cursor == x._cursor; }
    inline bool equal(const handle_t& x) const { return _cursor == x; }
    inline void increment() { ::Addle::aux_datatree::node_sibling_increment(_cursor); }
    
//     inline void advance(std::size_t n)
//     {
//         _index += n;
//     }
//     
//     inline std::ptrdiff_t distance_to(const ChildNodeIterator& x) const
//     {
//         return x._index - _index;
//     }
    
    handle_t _cursor = {};
    
    friend class ChildNodeIterator<Tree, true>;
    friend class boost::iterator_core_access;
};

// template<typename Tree, bool IsConst>
// inline ChildNodeIterator<Tree, IsConst> datatree_node_sibling_next(const ChildNodeIterator<Tree, IsConst>& iter) 
// { 
//     return ++ChildNodeIterator<Tree, IsConst>(iter); 
// }
// 
// template<typename Tree, bool IsConst>
// inline ChildNodeIterator<Tree, IsConst> datatree_node_sibling_next(ChildNodeIterator<Tree, IsConst>&& iter) 
// { 
//     return ++iter; 
// }

template<typename Tree, bool IsConst>
inline void datatree_node_sibling_increment(ChildNodeIterator<Tree, IsConst>& iter) { ++iter; }

// template<typename Tree, bool IsConst>
// inline auto node_children_begin(const ChildNodeIterator<Tree, IsConst>& iter)
// {
//     using handle_t = boost::mp11::mp_if_c<
//             IsConst,
//             const_node_handle_t<Tree>,
//             node_handle_t<Tree>
//         >;
//     return ::Addle::aux_datatree::node_children_begin( static_cast<const handle_t&>(iter) );
// }
// 
// template<typename Tree, bool IsConst>
// inline auto node_children_end(const ChildNodeIterator<Tree, IsConst>& iter)
// {
//     using handle_t = boost::mp11::mp_if_c<
//             IsConst,
//             const_node_handle_t<Tree>,
//             node_handle_t<Tree>
//         >;
//     return ::Addle::aux_datatree::node_children_end( static_cast<const handle_t&>(iter) );
// }

template<typename Tree>
using ConstChildNodeIterator = ChildNodeIterator<Tree, true>;

template<typename Tree>
using ChildNodeRange = boost::iterator_range<ChildNodeIterator<Tree>>;

template<typename Tree>
using ConstChildNodeRange = boost::iterator_range<ConstChildNodeIterator<Tree>>;

template<typename Tree, bool IsConst = false>
class AncestorNodeIterator
    : public boost::iterator_facade<
        AncestorNodeIterator<Tree, IsConst>,
        std::remove_reference_t<
            _dereferenced_t<
                boost::mp11::mp_if_c<
                    IsConst,
                    const_node_handle_t<Tree>,
                    node_handle_t<Tree>
                >
            >
        >,
        boost::forward_traversal_tag,
        _dereferenced_t<
            boost::mp11::mp_if_c<
                IsConst,
                const_node_handle_t<Tree>,
                node_handle_t<Tree>
            >
        >
    >
{        
    using handle_t = boost::mp11::mp_if_c<
            IsConst,
            const_node_handle_t<Tree>,
            node_handle_t<Tree>
        >;
public:
    AncestorNodeIterator() = default;
    AncestorNodeIterator(const AncestorNodeIterator&) = default;
    AncestorNodeIterator(AncestorNodeIterator&&) = default;
    
    AncestorNodeIterator(handle_t cursor)
        : _cursor(cursor)
    {
    }
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, AncestorNodeIterator<Tree, false>>::value, 
            void*
        > = nullptr
    >
    AncestorNodeIterator(const MutableIterator& i)
        : _cursor(i._cursor)
    {
    }
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, AncestorNodeIterator<Tree, false>>::value,
            void*
        > = nullptr
    >
    AncestorNodeIterator(MutableIterator&& i)
        : _cursor(std::move(i._cursor))
    {
    }
    
    AncestorNodeIterator& operator=(const AncestorNodeIterator&) = default;
    AncestorNodeIterator& operator=(AncestorNodeIterator&&) = default;
    
    inline operator const handle_t& () const { return _cursor; }
    
private:
    inline _dereferenced_t<handle_t>& dereference() const { return *_cursor; }
    inline bool equal(const AncestorNodeIterator& x) const { return _cursor == x._cursor; }
    inline bool equal(const handle_t& x) const { return _cursor == x; }
    inline void increment() { _cursor = ::Addle::aux_datatree::node_parent(_cursor); }
    
    handle_t _cursor = {};
    
    friend class AncestorNodeIterator<Tree, true>;
    friend class boost::iterator_core_access;
};

template<typename Tree>
using ConstAncestorNodeIterator = AncestorNodeIterator<Tree, true>;

template<typename Tree>
using AncestorNodeRange = boost::iterator_range<AncestorNodeIterator<Tree>>;

template<typename Tree>
using ConstAncestorNodeRange = boost::iterator_range<ConstAncestorNodeIterator<Tree>>;

// TODO: just use Boost, really. These aren't important for algorithms, so they
// may as well be a little gnostic

template<typename Predicate, typename Tree, bool IsConst = false>
class NodeFilterIterator
    : public boost::iterator_facade<
        NodeFilterIterator<Predicate, Tree, IsConst>,
        std::remove_reference_t<
            _dereferenced_t<
                boost::mp11::mp_if_c<
                    IsConst,
                    const_node_handle_t<Tree>,
                    node_handle_t<Tree>
                >
            >
        >,
        boost::forward_traversal_tag,
        _dereferenced_t<
            boost::mp11::mp_if_c<
                IsConst,
                const_node_handle_t<Tree>,
                node_handle_t<Tree>
            >
        >
    >
{
    using handle_t = boost::mp11::mp_if_c<
            IsConst,
            const_node_handle_t<Tree>,
            node_handle_t<Tree>
        >;
public:
    NodeFilterIterator() = default;
    NodeFilterIterator(const NodeFilterIterator&) = default;
    NodeFilterIterator(NodeFilterIterator&&) = default;
    
    NodeFilterIterator(Predicate p, handle_t cursor, handle_t end)
        : _p(p), _cursor(cursor), _end(end)
    {
        satisfyPredicate();
    }
    
    NodeFilterIterator(handle_t cursor, handle_t end)
        : _cursor(cursor), _end(end)
    {
        satisfyPredicate();
    }
            
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, NodeFilterIterator<Predicate, Tree, false>>::value, 
            void*
        > = nullptr
    >
    NodeFilterIterator(const MutableIterator& i)
        : _p(i._p), _cursor(i._cursor), _end(i._end)
    {
    }
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, NodeFilterIterator<Predicate, Tree, false>>::value,
            void*
        > = nullptr
    >
    NodeFilterIterator(MutableIterator&& i)
        : _p(std::move(i._p)), _cursor(std::move(i._cursor)), _end(std::move(i._end))
    {
    }
    
    NodeFilterIterator& operator=(const NodeFilterIterator&) = default;
    NodeFilterIterator& operator=(NodeFilterIterator&&) = default;
    
    inline handle_t cursor() const { return _cursor; }
    inline handle_t end() const { return _end; }
    inline const Predicate & predicate() const { return _p; }
    
private:
    inline _dereferenced_t<handle_t> dereference() const { return *_cursor; }
    inline bool equal(const NodeFilterIterator& x) const { return _cursor == x._cursor; }
    inline void increment() 
    {
        ::Addle::aux_datatree::node_dfs_next(_cursor);
        satisfyPredicate();
    }
    
    inline void satisfyPredicate()
    {
        while (_cursor != _end && !_p(_cursor))
        {
            ::Addle::aux_datatree::node_dfs_next(_cursor);
        } 
    }
    
    Predicate _p = {};
    
    handle_t _cursor = {};
    handle_t _end = {};
    
    friend class NodeFilterIterator<Predicate, Tree, true>;
    friend class boost::iterator_core_access;
};

struct predicate_nodeIsLeaf
{
    template<typename NodeHandle>
    inline bool operator()(NodeHandle node)
    {
        return datatree_node_is_leaf(node);
    }
};

template<typename NodeHandle>
using LeafIterator = NodeFilterIterator<predicate_nodeIsLeaf, NodeHandle>;

template<typename NodeHandle>
using ConstLeafIterator = NodeFilterIterator<predicate_nodeIsLeaf, NodeHandle, true>;

template<typename NodeHandle>
using LeafRange = boost::iterator_range<LeafIterator<NodeHandle>>;

template<typename NodeHandle>
using ConstLeafRange = boost::iterator_range<ConstLeafIterator<NodeHandle>>;

struct predicate_nodeIsBranch
{
    template<typename NodeHandle>
    inline bool operator()(NodeHandle node)
    {
        return datatree_node_is_branch(node);
    }
};

template<typename NodeHandle>
using BranchIterator = NodeFilterIterator<predicate_nodeIsBranch, NodeHandle>;

template<typename NodeHandle>
using ConstBranchIterator = NodeFilterIterator<predicate_nodeIsBranch, NodeHandle, true>;

template<typename NodeHandle>
using BranchRange = boost::iterator_range<BranchIterator<NodeHandle>>;

template<typename NodeHandle>
using ConstBranchRange = boost::iterator_range<ConstBranchIterator<NodeHandle>>;


} // namespace aux_datatree 

using DataTreeNodeAddress = aux_datatree::NodeAddress;

template<class Tree>
inline aux_datatree::node_handle_t<Tree> 
    data_tree_contains_address(Tree&& tree, const DataTreeNodeAddress& address)
{
    return aux_datatree::node_contains_address(datatree_root(tree), address);
}

template<class Tree>
inline aux_datatree::node_handle_t<Tree> 
    data_tree_lookup_address(Tree&& tree, const DataTreeNodeAddress& address)
{
    return aux_datatree::node_lookup_address(datatree_root(tree), address);
}

template<
        class SourceTree,
        class DestTree,
        typename MakeNode = aux_datatree::echo_default_make_node_tag,
        typename AfterMakeNode = aux_datatree::echo_default_after_make_node_tag
    >
inline void datatree_echo(
        SourceTree&& sourceTree,
        DestTree&& destTree,
        MakeNode&& makeNode = {},
        AfterMakeNode&& afterMakeNode = {}
    )
{
    aux_datatree::echo_impl<
            boost::remove_cv_ref_t<SourceTree>,
            boost::remove_cv_ref_t<DestTree>,
            MakeNode&&,
            AfterMakeNode&&
        > {
            datatree_root(sourceTree),
            datatree_root(destTree),
            std::forward<MakeNode>(makeNode),
            std::forward<AfterMakeNode>(afterMakeNode)
        } ();
}

} // namespace Addle
