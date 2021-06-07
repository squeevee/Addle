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

#include "utilities/ranges.hpp"
#include "utilities/metaprogramming.hpp"

#include "./nodeaddress.hpp"

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
using _const_handle_t = typename datatree_traits<Tree>::const_node_handle;

template<typename Tree>
using _node_handle_t = typename datatree_traits<Tree>::node_handle;

template<typename Tree>
using node_handle_t = typename boost::mp11::mp_if<
        std::is_const<Tree>,
        boost::mp11::mp_defer<_const_handle_t, std::remove_const_t<Tree>>,
        boost::mp11::mp_defer<_node_handle_t, Tree>
    >::type;

template<typename Tree>
using const_node_handle_t = _const_handle_t<Tree>;

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
using _node_children_begin_t = decltype( datatree_node_children_begin( std::declval<NodeHandle>()) );

template<typename NodeHandle>
using _node_children_end_t = decltype( datatree_node_children_end( std::declval<NodeHandle>()) );

template<typename NodeHandle>
using is_node_handle = boost::mp11::mp_and<
        boost::mp11::mp_valid<_node_children_begin_t, NodeHandle>,
        boost::mp11::mp_valid<_node_children_end_t, NodeHandle>
    >;

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
        
template<typename NodeHandle, class Category>
using _handle_is_iterator_with_category = std::is_convertible<
        boost::mp11::mp_eval_or_q<
            void,
            boost::mp11::mp_bind<_iterator_category_t, boost::mp11::_1>,
            NodeHandle
        >,
        Category
    >;

template<typename NodeHandle>
std::size_t node_child_count_impl( const NodeHandle& node ) 
{
    if (!node) return 0;
    
    auto cursor = ::Addle::aux_datatree::node_children_begin(node);
    auto&& end = ::Addle::aux_datatree::node_children_end(node);
    
    if constexpr (
        _handle_is_iterator_with_category<
            decltype(cursor), 
            std::random_access_iterator_tag      
        >::value)
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
NodeHandle node_child_at_impl( const NodeHandle& node, std::size_t index ) 
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
inline auto node_child_at( NodeHandle&& node, std::size_t index ) { return datatree_node_child_at( std::forward<NodeHandle>(node), index ); }

template<typename NodeHandle, std::enable_if_t<!boost::is_detected<_node_child_at_t, NodeHandle>::value, void*> = nullptr>
inline NodeHandle node_child_at( const NodeHandle& node, std::size_t index ) { return ::Addle::aux_datatree::node_child_at_impl(node, index); }

template<typename NodeHandle>
using _node_last_child_t = decltype( datatree_node_last_child( std::declval<NodeHandle>() ) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_last_child_t, NodeHandle>::value, void*> = nullptr>
inline auto node_last_child( NodeHandle&& node ) { return datatree_node_last_child( std::forward<NodeHandle>(node)); }

template<typename NodeHandle,
    std::enable_if_t<
        boost::mp11::mp_and<
            boost::mp11::mp_not<boost::is_detected<_node_last_child_t, NodeHandle>>,
            _handle_is_iterator_with_category<
                decltype( ::Addle::aux_datatree::node_children_end(std::declval<NodeHandle>()) ),
                std::bidirectional_iterator_tag
            >
        >::value,
    void*> = nullptr>
inline child_node_handle_t<boost::remove_cv_ref_t<NodeHandle>> node_last_child( const NodeHandle& node)
{
}

template<typename NodeHandle>
std::size_t node_index_impl( const NodeHandle& node ) 
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
std::size_t node_depth_impl( const NodeHandle& node )
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
child_node_handle_t<NodeHandle> _node_handle_to_child_handle( const NodeHandle& node )
{
    if (!node) return {};
    
    auto parent = ::Addle::aux_datatree::node_parent(node);
    
    if (!parent) return {};
    
    auto cursor = ::Addle::aux_datatree::node_children_begin(parent);
    
    if constexpr (
            _handle_is_iterator_with_category<
                child_node_handle_t<NodeHandle>,
                std::random_access_iterator_tag
            >::value
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
std::decay_t<NodeHandle> node_dfs_next( const NodeHandle& node )
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


// template<typename NodeHandle>
// using _node_is_leaf_t = decltype( datatree_node_is_leaf(std::declval<NodeHandle>()) );
// 
// template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_is_leaf_t, NodeHandle>::value, void*> = nullptr>
// inline bool node_is_leaf( NodeHandle&& node ) { return datatree_node_is_leaf(std::forward<NodeHandle>(node)); }
// 
// template<typename NodeHandle, std::enable_if_t<!boost::is_detected<_node_is_leaf_t, NodeHandle>::value, void*> = nullptr>
// inline bool node_is_leaf( const NodeHandle& node )
// { 
//     return ::Addle::aux_datatree::node_children_begin(node) == ::Addle::aux_datatree::node_children_end(node);
// }
// 
// 
// template<typename NodeHandle>
// using _node_is_branch_t = decltype( datatree_node_is_branch(std::declval<NodeHandle>()) );
// 
// template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_is_branch_t, NodeHandle>::value, void*> = nullptr>
// inline bool node_is_branch( NodeHandle&& node ) { return datatree_node_is_branch(std::forward<NodeHandle>(node)); }
// 
// template<typename NodeHandle, std::enable_if_t<!boost::is_detected<_node_is_branch_t, NodeHandle>::value, void*> = nullptr>
// inline bool node_is_branch( const NodeHandle& node )
// { 
//     return ::Addle::aux_datatree::node_children_begin(node) != ::Addle::aux_datatree::node_children_end(node);
// }


template<typename NodeHandle>
inline decltype(auto) node_value( NodeHandle&& node ) { return datatree_node_value(std::forward<NodeHandle>(node)); }

template<typename NodeHandle, typename T>
inline void node_set_value( NodeHandle&& node, T&& v) { return datatree_node_set_value(std::forward<NodeHandle>(node), std::forward<T>(v)); }

template<typename NodeHandle>
using _node_has_value_t = decltype( datatree_node_has_value(std::declval<NodeHandle>()) );

template<typename NodeHandle, std::enable_if_t<boost::is_detected<_node_has_value_t, NodeHandle>::value, void*> = nullptr>
inline auto node_has_value( NodeHandle&& node ) { return datatree_node_has_value( std::forward<NodeHandle>(node) ); }

template<typename NodeHandle, std::enable_if_t<!boost::is_detected<_node_has_value_t, NodeHandle>::value, void*> = nullptr>
inline bool node_has_value( const NodeHandle& node ) { return static_cast<bool>(node); }

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
inline auto node_insert_child( NodeHandle&& parent, ChildHandle&& pos, T&& childValue )
{
    return datatree_node_insert_children(
            std::forward<NodeHandle>(parent), 
            std::forward<ChildHandle>(pos), 
            make_inline_ref_range(std::forward<T>(childValue))
        );
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
std::decay<ChildHandle> node_insert_children( NodeHandle&& parent, ChildHandle&& pos, const Range& childValues )
{
    std::decay<ChildHandle> next;
    
    for (auto&& v : childValues)
    {
        next = datatree_node_insert_child(
                std::forward<NodeHandle>(parent), 
                pos, 
                std::forward<decltype(v)>(v)
            );
    }
    
    return next;
}

template<typename NodeHandle>
NodeAddress calculate_node_address(NodeHandle node)
{
    if (!node) return NodeAddress();
    
    QVarLengthArray<std::size_t, 16> indices;
    indices.reserve(::Addle::aux_datatree::node_depth(node));
    
    while (true)
    {
        std::size_t index = ::Addle::aux_datatree::node_index(node);
        
        if ( !(node = ::Addle::aux_datatree::node_parent(node)) ) break;
        
        indices.append(index);
    }
    
    std::reverse(indices.begin(), indices.end());
    
    return NodeAddressBuilder(indices);
}

template<typename NodeHandle>
using _node_address_t = decltype( datatree_node_address(std::declval<NodeHandle>()) );

template<typename NodeHandle, std::enable_if_t<boost::mp11::mp_valid<_node_address_t, NodeHandle>::value, void*> = nullptr>
inline NodeAddress node_address( NodeHandle&& node ) { return datatree_node_address( std::forward<NodeHandle>(node) ); }

template<typename NodeHandle, std::enable_if_t<!boost::mp11::mp_valid<_node_address_t, NodeHandle>::value, void*> = nullptr>
inline NodeAddress node_address( const NodeHandle& node )
{
    return ::Addle::aux_datatree::calculate_node_address(node);
}

template<typename NodeHandle>
bool node_contains_address_impl(const NodeHandle& root, const NodeAddress& address)
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
std::decay_t<NodeHandle> node_lookup_address_impl(const NodeHandle& root, const NodeAddress& address)
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
 * and conducting such a search requires a degree of statefulness. 
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
    
    decltype(auto) operator*() const { return *currentNode(); }
    
    NodeAddress address() const
    {
        // cache until modified maybe?
        
        NodeAddressBuilder result;
        result.reserve(_descendants.size());
        
        NodeHandle parentCursor = _root;
        for (const auto& descendant : _descendants)
        {
            std::size_t index;
            
            descendant_handle_t firstSibling = 
                ::Addle::aux_datatree::node_children_begin(parentCursor);
            
            if constexpr (_handle_is_iterator_with_category<
                    descendant_handle_t,
                    std::random_access_iterator_tag
                >::value)
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
        
        return std::move(result);
    }
    
    // result type of `node_dfs_end`, == and != comparable to a DFSExtendedNode
    // `node` gives whether `node` is in its end state. A default-constructed
    // DFSExtendedNode would give the same behavior, but this is preferable for 
    // non-portable DFSExtendedNode as it avoids allocating array space that
    // will never be used.
    struct EndSentinel {};
    
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
                
            if constexpr (
                    _handle_is_iterator_with_category<
                        descendant_handle_t,
                        std::random_access_iterator_tag
                    >::value
                )
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
    
    
    friend bool operator==(const DFSExtendedNode& node, const EndSentinel&) 
    { 
        return !(node); 
    }
    
    friend bool operator==(const EndSentinel&, const DFSExtendedNode& node) 
    { 
        return !(node); 
    }
    
    friend bool operator!=(const DFSExtendedNode& node, const EndSentinel&) 
    { 
        return static_cast<bool>(node); 
    }
        
    friend bool operator!=(const EndSentinel&, const DFSExtendedNode& node) 
    { 
        return static_cast<bool>(node); 
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
inline typename DFSExtendedNode<boost::remove_cv_ref_t<NodeHandle>>::EndSentinel 
    node_dfs_end( NodeHandle&& ) 
{
    return typename DFSExtendedNode<boost::remove_cv_ref_t<NodeHandle>>::EndSentinel {}; 
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

// struct predicate_nodeIsLeaf
// {
//     template<typename NodeHandle>
//     inline bool operator()(NodeHandle node)
//     {
//         return datatree_node_is_leaf(node);
//     }
// };
// 
// template<typename NodeHandle>
// using LeafIterator = NodeFilterIterator<predicate_nodeIsLeaf, NodeHandle>;
// 
// template<typename NodeHandle>
// using ConstLeafIterator = NodeFilterIterator<predicate_nodeIsLeaf, NodeHandle, true>;
// 
// template<typename NodeHandle>
// using LeafRange = boost::iterator_range<LeafIterator<NodeHandle>>;
// 
// template<typename NodeHandle>
// using ConstLeafRange = boost::iterator_range<ConstLeafIterator<NodeHandle>>;
// 
// struct predicate_nodeIsBranch
// {
//     template<typename NodeHandle>
//     inline bool operator()(NodeHandle node)
//     {
//         return datatree_node_is_branch(node);
//     }
// };
// 
// template<typename NodeHandle>
// using BranchIterator = NodeFilterIterator<predicate_nodeIsBranch, NodeHandle>;
// 
// template<typename NodeHandle>
// using ConstBranchIterator = NodeFilterIterator<predicate_nodeIsBranch, NodeHandle, true>;
// 
// template<typename NodeHandle>
// using BranchRange = boost::iterator_range<BranchIterator<NodeHandle>>;
// 
// template<typename NodeHandle>
// using ConstBranchRange = boost::iterator_range<ConstBranchIterator<NodeHandle>>;


template<class Tree, class Range>
class NodeHandleRangeAdapter
{
public:
    using handle_t = typename boost::range_value<Range>::type;
    static_assert(
        boost::mp11::mp_or<
            std::is_same<handle_t, node_handle_t<Tree>>,
            std::is_same<handle_t, const_node_handle_t<Tree>>,
            std::is_same<handle_t, child_node_handle_t<node_handle_t<Tree>>>,
            std::is_same<handle_t, child_node_handle_t<const_node_handle_t<Tree>>>
        >::value
    );
    
    using const_handle_t = boost::mp11::mp_cond<
            std::is_same<handle_t, child_node_handle_t<node_handle_t<Tree>>>,
                child_node_handle_t<const_node_handle_t<Tree>>,
            std::is_same<handle_t, node_handle_t<Tree>>,
                const_node_handle_t<Tree>,
            boost::mp11::mp_true, // handle_t is already const
                handle_t
        >;
        
    using value_type = std::decay_t<_dereferenced_t<handle_t>>;
    using reference = _dereferenced_t<handle_t>;
    using const_reference = _dereferenced_t<const_handle_t>;
    
    template<bool Const>
    class iterator_impl 
        : public boost::iterator_adaptor<
            iterator_impl<Const>,
            typename boost::range_iterator<
                boost::mp11::mp_if_c<Const, const Range, Range>
            >::type,
            value_type,
            typename boost::range_category<Range>::type,
            boost::mp11::mp_if_c<Const, const_reference, reference>
        >
    {
        using _base_t = boost::iterator_adaptor<
                iterator_impl<Const>,
                typename boost::range_iterator<
                    boost::mp11::mp_if_c<Const, const Range, Range>
                >::type,
                value_type,
                typename boost::range_category<Range>::type,
                boost::mp11::mp_if_c<Const, const_reference, reference>
            >;
        using _inner_t = typename boost::range_iterator<
                boost::mp11::mp_if_c<Const, const Range, Range>
            >::type;
            
    public:
        iterator_impl();
        iterator_impl(const iterator_impl&) = default;
        iterator_impl(iterator_impl&&) = default;
        
        template<
            typename MutableIterator,
            typename std::enable_if_t<
                Const && std::is_same_v<MutableIterator, iterator_impl<false>>,
            void*> = nullptr
        >
        iterator_impl(MutableIterator other)
            : _base_t(other.base())
        {
        }
        
        iterator_impl& operator=(const iterator_impl&) = default;
        iterator_impl& operator=(iterator_impl&&) = default;
        
        operator handle_t () const { return *(this->base()); }
        
    private:
        iterator_impl(_inner_t inner)
            : _base_t(inner)
        {
        }
        
        boost::mp11::mp_if_c<Const, const_reference, reference> dereference() const
        {
            return **(this->base());
        }
        
        friend class boost::iterator_core_access;
        friend class NodeHandleRangeAdapter<Tree, Range>;
    };
    
    using iterator          = iterator_impl<false>;
    using const_iterator    = iterator_impl<true>;
    
    using size_type         = std::size_t;
    using difference_type   = std::ptrdiff_t;
    
    iterator begin() { return iterator(boost::begin(_range)); }
    const_iterator begin() const { return const_iterator(boost::begin(_range)); }
    
    iterator end() { return iterator(boost::end(_range)); }
    const_iterator end() const { return const_iterator(boost::end(_range)); }
    
    const Range& base() const { return _range; }
    
private:
    const Range& _range;
};

template<typename T, typename Discrim>
struct _storage_util_basic
{
    using discriminator = Discrim;
    std::decay_t<T> accessor;
    
    inline T& get() { return static_cast<T&>(accessor); }
    inline const T& get() const { return static_cast<const T&>(accessor); }
};

template<typename T, typename Discrim>
struct _storage_util_ebo : T
{
    using discriminator = Discrim;
    
    inline T& get() { return *static_cast<T*>(this); }
    inline const T& get() const { return *static_cast<const T*>(this); }
};

struct _nil_storage_util_discrim {};

// Utility base class type for storing user-supplied objects (especially 
// callables) that might benefit from empty base optimization.
template<typename T, typename Discrim = _nil_storage_util_discrim>
using storage_util = typename boost::mp11::mp_if<
        boost::mp11::mp_and<
            std::is_class<T>,
#ifdef __cpp_lib_is_final
            boost::mp11::mp_not< std::is_final<T> >,
#endif
            std::is_empty<T>
        >,
        boost::mp11::mp_defer<_storage_util_ebo, T, Discrim>,
        boost::mp11::mp_defer<_storage_util_basic, T, Discrim>
    >::type;

} // namespace aux_datatree 

template<class Tree>
inline bool data_tree_contains_address(Tree&& tree, const DataTreeNodeAddress& address)
{
    return aux_datatree::node_contains_address(::Addle::aux_datatree::tree_root(tree), address);
}

template<class Tree>
inline aux_datatree::node_handle_t<Tree> 
    data_tree_lookup_address(Tree&& tree, const DataTreeNodeAddress& address)
{
    return aux_datatree::node_lookup_address(::Addle::aux_datatree::tree_root(tree), address);
}

} // namespace Addle
