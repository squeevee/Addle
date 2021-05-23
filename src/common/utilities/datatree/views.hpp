#pragma once

#include <optional>

#include <boost/mp11.hpp>
#include <boost/type_traits.hpp>

#include "./aux.hpp"

namespace Addle {
namespace aux_datatree {

/**
 * Exposes an API similar to DataTree::Node for any handle -- a custom handle_t
 * class can dereference itself to this
 */
template<class Tree, bool IsConst = false>
class NodeView
{
public:
    using handle_t = boost::mp11::mp_if_c<
            IsConst,
            const_node_handle_t<Tree>,
            node_handle_t<Tree>
        >;
    
    using value_type        = NodeView<Tree, IsConst>;
    using reference         = NodeView<Tree, IsConst>;
    using const_reference   = NodeView<Tree, true>;
    using iterator          = NodeIterator<Tree, IsConst>;
    using const_iterator    = ConstNodeIterator<Tree>;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    
    using child_range = boost::mp11::mp_if_c<
            IsConst,
            ConstChildNodeRange<Tree>,
            ChildNodeRange<Tree>
        >;
    using const_child_range = ConstChildNodeRange<Tree>;

    using descendant_range = boost::mp11::mp_if_c<
            IsConst,
            ConstNodeRange<Tree>,
            NodeRange<Tree>
        >;
    using const_descendant_range = ConstNodeRange<Tree>;
    
    using ancestor_range = boost::mp11::mp_if_c<
            IsConst,
            ConstAncestorNodeRange<Tree>,
            AncestorNodeRange<Tree>
        >;
    using const_ancestor_range = ConstAncestorNodeRange<Tree>;
   
    // TODO: gonna implement these using the more generic Boost filter ranges.
//     using leaf_range            = aux_datatree::ConstLeafRange<TransformTree<Tree, TransformOp>>;
//     using const_leaf_range      = aux_datatree::ConstLeafRange<TransformTree<Tree, TransformOp>>;
//     
//     using branch_range          = aux_datatree::ConstBranchRange<TransformTree<Tree, TransformOp>>;
//     using const_branch_range    = aux_datatree::ConstBranchRange<TransformTree<Tree, TransformOp>>;
        
    explicit inline NodeView( const handle_t& node )
        : _node(node)
    {
    }
       
    explicit inline NodeView( handle_t&& node )
        : _node(std::move(node))
    {
    }
    
    NodeView(const NodeView&) = default;
    NodeView(NodeView&&) = default;
    
    template<
        typename MutableNodeView, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableNodeView, NodeView<Tree, false>>::value, 
            void*
        > = nullptr
    >
    inline NodeView(const MutableNodeView& other)
        : _node(other._node)
    {
    }
    
    NodeView() = delete;
    
    NodeView& operator=(const NodeView&) = delete;
    NodeView& operator=(NodeView&&) = delete;

    const handle_t& node() const { return _node; }
    
    inline handle_t parent() const { return ::Addle::aux_datatree::node_parent(_node); }
    inline std::size_t depth() const { return ::Addle::aux_datatree::node_depth(_node); }
    inline std::size_t index() const { return ::Addle::aux_datatree::node_index(_node); }
    inline NodeAddress address() const { return ::Addle::aux_datatree::node_address(_node); }
    
    inline NodeView root() const { return NodeView(::Addle::aux_datatree::node_root(_node)); }
    
    inline const_iterator cbegin() const { return const_iterator(_node); }
    inline const_iterator begin() const { return cbegin(); }
    
    inline const_iterator cend() const
    { 
        return const_iterator(::Addle::aux_datatree::node_dfs_end(_node)); 
    }
    inline const_iterator end() const { return cend(); }
    
    inline bool isLeaf() const { return ::Addle::aux_datatree::node_is_leaf(_node); }
    inline bool isBranch() const { return ::Addle::aux_datatree::node_is_branch(_node); }
        
    inline node_value_t<Tree> value() const { return ::Addle::aux_datatree::node_value(_node); }
    
    inline std::size_t countChildren() const { return ::Addle::aux_datatree::node_child_count(_node); }
    
    inline const_child_range children() const
    { 
        return const_child_range(
                ::Addle::aux_datatree::node_children_begin(_node),
                ::Addle::aux_datatree::node_children_end(_node)
            );
    }
    
    inline NodeView operator[](std::size_t index) const
    {
        return NodeView(::Addle::aux_datatree::node_child_at(_node, index));
    }
    
//     inline const_descendant_range descendants() const
//     {
//         if (hasChildren())
//         {
//             Node firstChild = datatree_node_children_begin(_node).cursor();
//             return const_descendant_range(firstChild, end());
//         }
//         else
//         {
//             return const_descendant_range(); 
//         }
//     }
    
    inline const_ancestor_range ancestors() const
    { 
        return const_ancestor_range( ::Addle::aux_datatree::node_parent(_node), handle_t {}); 
    }
    
    inline const_ancestor_range ancestorsAndSelf() const 
    { 
        return const_ancestor_range( _node, handle_t {}); 
    }
    
//     inline const_leaf_range leaves() const
//     { 
//         using leaf_iterator_t = typename const_leaf_range::iterator;
//         return const_leaf_range(
//                 leaf_iterator_t ( *this, end() ), leaf_iterator_t ( end(), end() )
//             );
//     }
//     
//     inline const_branch_range branches() const
//     { 
//         using branch_iterator_t = typename const_branch_range::iterator;
//         return const_branch_range(
//                 branch_iterator_t ( *this, end() ), branch_iterator_t ( end(), end() )
//             );
//     }
    
private:
    handle_t _node;
};

template<class Tree, class TransformOp>
class TransformTree
{
    using base_handle_t = const_node_handle_t<Tree>;
public:
    using node_value_type = decltype(
            std::declval<TransformOp>() (
                std::declval<node_value_t<Tree>>()
            ) 
        );
    
    class NodeHandle
    {
    public:
        using node_view_t = NodeView<TransformTree<Tree, TransformOp>, true>;
        
        NodeHandle() = default;
        NodeHandle(const NodeHandle&) = default;
        NodeHandle(NodeHandle&&) = default;
        
        NodeHandle& operator=(const NodeHandle&) = default;
        NodeHandle& operator=(NodeHandle&&) = default;
        
        inline NodeHandle(base_handle_t baseNode, TransformOp transform)
            : _baseNode(std::move(baseNode)), _transform(std::move(transform))
        {
        }
        
        explicit inline operator bool () const
        { 
            if constexpr (std::is_convertible<const TransformOp&, bool>::value)
            {
                return static_cast<bool>(_baseNode)
                    && static_cast<bool>(_transform);
            }
            else
            {
                return static_cast<bool>(_baseNode); 
            }
        }
        
        inline bool operator! () const
        {
            return !static_cast<bool>(*this);
        }
        
        inline bool operator==(const NodeHandle& x) const
        {
            if constexpr (boost::has_equal_to<const TransformOp&, const TransformOp&, bool>::value)
            {
                return static_cast<bool>(_baseNode == x._baseNode)
                    && static_cast<bool>(_transform == x._transform);
            }
            else
            {
                return static_cast<bool>(_baseNode == x._baseNode);
            }
        }
        
        inline bool operator!=(const NodeHandle& x) const
        {
            return !(*this == x);
        }
        
        inline node_view_t operator*() const
        {
            return node_view_t(*this);
        }
        
    private:
        
        base_handle_t _baseNode = {};
        TransformOp _transform = {}; // EBO maybe?
        
        friend NodeHandle datatree_node_parent(const NodeHandle& node)
        { 
            return NodeHandle(aux_datatree::node_parent(node._baseNode), node._transform);
        }
        
        friend std::size_t datatree_node_index(const NodeHandle& node)
        { 
            return aux_datatree::node_index(node._baseNode);
        }
        
        friend std::size_t datatree_node_depth(const NodeHandle& node)
        { 
            return aux_datatree::node_depth(node._baseNode);
        } 
        
        friend NodeHandle datatree_node_root(const NodeHandle& node) 
        { 
            return NodeHandle(aux_datatree::node_root(node._baseNode), node._transform);
        }
        
        friend node_value_type datatree_node_leaf_value(const NodeHandle& node) 
        { 
            return node._transform( aux_datatree::node_value(node._baseNode) );
        }
        
        friend std::size_t datatree_node_child_count(const NodeHandle& node)
        { 
            return aux_datatree::node_child_count(node._baseNode);
        }
        
        friend NodeHandle datatree_node_children_begin(const NodeHandle& node)
        {
            return NodeHandle(aux_datatree::node_children_begin(node._baseNode), node._transform);
        }
        
        friend NodeHandle datatree_node_children_end(const NodeHandle& node)
        {
            return NodeHandle(aux_datatree::node_children_end(node._baseNode), node._transform);
        }
        
        friend NodeHandle datatree_node_child_at(const NodeHandle& node, std::size_t index)
        { 
            return NodeHandle(datatree_node_child_at(node._baseNode, index), node._transform);
        }
        
        friend NodeHandle datatree_node_sibling_next(NodeHandle&& node)
        {
            return NodeHandle(aux_datatree::node_sibling_next(std::move(node._baseNode)), std::move(node._transform));
        }
        
        friend NodeHandle datatree_node_sibling_next(const NodeHandle& node)
        {
            return NodeHandle(aux_datatree::node_sibling_next(node._baseNode), node._transform);
        }
        
        friend NodeHandle datatree_node_dfs_end(const NodeHandle& node)
        { 
            return NodeHandle( aux_datatree::node_dfs_end(node._baseNode), node._transform);
        }
        
        friend NodeHandle datatree_node_dfs_next(NodeHandle&& node)
        { 
            return NodeHandle(aux_datatree::node_dfs_next(std::move(node._baseNode)), std::move(node._transform));
        }
        
        friend NodeHandle datatree_node_dfs_next(const NodeHandle& node)
        { 
            return NodeHandle(aux_datatree::node_dfs_next(node._baseNode), node._transform);
        }
        
//         friend bool datatree_node_shallow_equivalent(const NodeHandle& node1, const NodeHandle& node2)
//         {
//             return datatree_node_shallow_equivalent(node1._baseNode, node2._baseNode);
//         }
        
        friend bool datatree_node_is_branch(const NodeHandle& node)
        { 
            return aux_datatree::node_is_branch(node._baseNode);
        }
        
        friend bool datatree_node_is_leaf(const NodeHandle& node)
        { 
            return aux_datatree::node_is_leaf(node._baseNode);
        }
        
        friend NodeAddress datatree_node_address(const NodeHandle& node)
        { 
            return aux_datatree::node_address(node._baseNode);
        }
    };
    
    using Node = typename NodeHandle::node_view_t;
    
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
    
    inline TransformTree(const Tree& tree, TransformOp transform = {})
        : _root( datatree_root(tree), std::move(transform))
    {
    }
    
    inline NodeHandle rootHandle() const { return _root; }
    
    inline Node root() const { return *_root; }
    
    inline std::size_t size() const { return 0; /*_tree.size();*/ } // TODO
    
    inline const_iterator begin() const { return _root; }
    inline const_iterator cbegin() const { return _root; }
    
    inline const_iterator end() const { return aux_datatree::node_dfs_end(_root); }
    inline const_iterator cend() const { return aux_datatree::node_dfs_end(_root); }
    
//     inline const_leaf_range leaves() const { return root().leaves(); }
//     
//     inline const_branch_range branches() const { return root().branches(); }
    
private:
    NodeHandle _root;
};

template<class Tree, class TransformOp>
struct datatree_traits<TransformTree<Tree, TransformOp>>
{
    using node_handle = typename TransformTree<Tree, TransformOp>::NodeHandle;
    using const_node_handle = typename TransformTree<Tree, TransformOp>::NodeHandle;
    
    using node_value_type = typename TransformTree<Tree, TransformOp>::node_value_type;
};

template<typename Iterator, typename Tree, typename TransformOp, bool IsConst>
class NodeTransformIterator : public boost::iterator_adaptor<
        NodeTransformIterator<Iterator, Tree, TransformOp, IsConst>,
        boost::mp11::mp_if_c<
            IsConst,
            aux_datatree::const_node_handle_t<Tree>,
            aux_datatree::node_handle_t<Tree>
        >,
        NodeView<TransformTree<Tree, TransformOp>, IsConst>, 
        boost::use_default,
        NodeView<TransformTree<Tree, TransformOp>, IsConst>
    >
{
    using adaptor_t = boost::iterator_adaptor<
            NodeTransformIterator<Iterator, Tree, TransformOp, IsConst>,
            boost::mp11::mp_if_c<
                IsConst,
                aux_datatree::const_node_handle_t<Tree>,
                aux_datatree::node_handle_t<Tree>
            >,
            NodeView<TransformTree<Tree, TransformOp>, IsConst>, 
            boost::use_default,
            NodeView<TransformTree<Tree, TransformOp>, IsConst>
        >;
        
    using handle_t = typename TransformTree<Tree, TransformOp>::NodeHandle;
    using base_handle_t = boost::mp11::mp_if_c<
            IsConst,
            aux_datatree::const_node_handle_t<Tree>,
            aux_datatree::node_handle_t<Tree>
        >;
    using view_t = NodeView<Tree, IsConst>;
    
public:
    NodeTransformIterator() = default;
    NodeTransformIterator(const NodeTransformIterator&) = default;
    
    NodeTransformIterator(const Iterator& i)
        : adaptor_t(i)
    {
    }
    
    NodeTransformIterator(const handle_t& node)
        : adaptor_t(node._baseNode), _transform(node._transform)
    {
    }
    
    inline operator handle_t () const { return handle_t(this->base(), _transform); }
    
private:
    inline view_t dereference() const { return view_t(static_cast<handle_t>(*this)); }
    
    TransformOp _transform = {}; // TODO: deal with transforms not default constructible
    
    friend class iterator_core_access;
};

template<typename Tree, typename T>
struct CastOp
{
    inline T operator()(const node_value_t<Tree>& v) const
    {
        return static_cast<T>(v);
    }
    
    inline T operator()(node_value_t<Tree>&& v) const
    {
        return static_cast<T>(std::move(v));
    }
};

} // namespace aux_datatree

template<typename Tree, typename T>
using DataTreeCastView = aux_datatree::TransformTree<Tree, aux_datatree::CastOp<Tree, T>>;

} // namespace Addle 
