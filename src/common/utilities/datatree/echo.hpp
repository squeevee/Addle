#pragma once

#include <deque>
#include <boost/mp11.hpp>

#include "./aux.hpp"
#include "./observer.hpp"

namespace Addle::aux_datatree {

struct echo_do_nothing_tag                  {};

struct echo_default_node_value_tag          {};
struct echo_default_node_added_tag          : echo_do_nothing_tag {};

struct _echo_node_value_discr_tag           {};
struct _echo_node_added_discr_tag           {};

template<class SourceTree, class DestTree, typename NodeValue>
class _echo_NodeValueGetter : private storage_util<NodeValue>
{
public:
    decltype(auto) getNodeValue(const const_node_handle_t<SourceTree>& sourceCursor) const
    {
        if constexpr (std::is_convertible_v<NodeValue, echo_default_node_value_tag>)
        {
            if constexpr (std::is_convertible_v<
                    node_value_t<SourceTree>, node_value_t<DestTree>
                >)
            {
                return Q_LIKELY(::Addle::aux_datatree::node_has_value(sourceCursor)) ?
                    ::Addle::aux_datatree::node_value(sourceCursor)
                    : node_value_t<DestTree> {};
            }
            else
            {
                return node_value_t<DestTree> {};
            }
        }
        else if constexpr (std::is_invocable_v<NodeValue, 
                decltype(*std::declval<const_node_handle_t<SourceTree>>())
            >)
        {
            return std::invoke(
                    static_cast<const storage_util<NodeValue>*>(this)->get(),                            
                    *sourceCursor
                );
        }
        else if constexpr (std::is_invocable_v<NodeValue, const_node_handle_t<SourceTree>>)
        {
            return std::invoke(
                    static_cast<const storage_util<NodeValue>*>(this)->get(),
                    sourceCursor
                );
        }
        else 
        {
            return std::invoke(
                    static_cast<const storage_util<NodeValue>*>(this)->get(),
                    Q_LIKELY(::Addle::aux_datatree::node_has_value(sourceCursor)) ?
                        ::Addle::aux_datatree::node_value(sourceCursor)
                        : node_value_t<SourceTree> {}
                );
        }
    }
    
protected:
    template<typename NodeValue_>
    _echo_NodeValueGetter(NodeValue_&& value)
        : storage_util<NodeValue> { std::forward<NodeValue_>(value) }
    {
    }
};

template<class SourceTree, class DestTree, typename NodeValue>
using _echo_NodeValueType = decltype(
        std::declval<_echo_NodeValueGetter<SourceTree, DestTree, NodeValue>>()
            .getNodeValue(std::declval<const_node_handle_t<SourceTree>>())
    );

template<class SourceTree, class DestTree, typename NodeValue>
class _echo_NodeValueIterator 
    : public boost::iterator_facade<
        _echo_NodeValueIterator<SourceTree, DestTree, NodeValue>,
        std::remove_reference_t<_echo_NodeValueType<SourceTree, DestTree, NodeValue>>,
        boost::mp11::mp_cond<
            _handle_is_iterator_with_category<
                child_node_handle_t<const_node_handle_t<SourceTree>>,
                std::random_access_iterator_tag
            >,
                std::random_access_iterator_tag,
                
            _handle_is_iterator_with_category<
                child_node_handle_t<const_node_handle_t<SourceTree>>,
                std::bidirectional_iterator_tag
            >,    
                std::bidirectional_iterator_tag,
                
            boost::mp11::mp_true,
                std::forward_iterator_tag
        >,
        _echo_NodeValueType<SourceTree, DestTree, NodeValue>
    >
{
    using getter_t = _echo_NodeValueGetter<SourceTree, DestTree,NodeValue>;
    using handle_t = child_node_handle_t<const_node_handle_t<SourceTree>>;
    
public:
    _echo_NodeValueIterator() = default;
    _echo_NodeValueIterator(const _echo_NodeValueIterator&) = default;
    
    _echo_NodeValueIterator(handle_t node, const getter_t* getter = nullptr)
        : _node(node), _getter(getter)
    {
    }

private:
    decltype(auto) dereference() const
    { 
        return _getter->getNodeValue(
                static_cast<const_node_handle_t<SourceTree>>(_node)
            );
    }
    
    bool equal(const _echo_NodeValueIterator& other) const 
    { 
        return _node == other._node; 
    }
    
    void increment() { ::Addle::aux_datatree::node_sibling_increment(_node); }
    
    void decrement()
    {
        if constexpr (_handle_is_iterator_with_category<
            child_node_handle_t<const_node_handle_t<SourceTree>>,
            std::bidirectional_iterator_tag
        >::value)
            --_node;
        else
            Q_UNREACHABLE();
    }
    
    void advance(std::ptrdiff_t n)
    {
        if constexpr (_handle_is_iterator_with_category<
            child_node_handle_t<const_node_handle_t<SourceTree>>,
            std::random_access_iterator_tag
        >::value)
            _node += n;
        else
            Q_UNREACHABLE();
    }
    
    std::ptrdiff_t distance_to(const _echo_NodeValueIterator& other) const
    {
        if constexpr (_handle_is_iterator_with_category<
            child_node_handle_t<const_node_handle_t<SourceTree>>,
            std::random_access_iterator_tag
        >::value)
            return other._node - _node;
        else
            Q_UNREACHABLE();
    }
    
    handle_t _node = {};
    const getter_t* _getter = nullptr;
    
    friend class boost::iterator_core_access;
};


template<class SourceTree, class DestTree, typename NodeAdded>
class _echo_NodeAddedInvoker
    : private storage_util<NodeAdded>
{
public:
    void invoke(const_node_handle_t<SourceTree> source, node_handle_t<DestTree> dest) const
    {
        if constexpr (std::is_convertible_v<NodeAdded, echo_default_node_added_tag>)
        {
            return;
        }
        else if constexpr (std::is_invocable_v<NodeAdded, 
                const_node_handle_t<SourceTree>,
                node_handle_t<DestTree>
            >)
        {
            std::invoke(
                    static_cast<const storage_util<NodeAdded>*>(this)->get(),
                    source,
                    dest
                );
        }
        else
        {
            std::invoke(
                    static_cast<const storage_util<NodeAdded>*>(this)->get(),
                    Q_LIKELY(::Addle::aux_datatree::node_has_value(source)) ?
                        ::Addle::aux_datatree::node_value(source) :
                        node_value_t<SourceTree> {},
                    Q_LIKELY(::Addle::aux_datatree::node_has_value(dest)) ?
                        ::Addle::aux_datatree::node_value(dest) :
                        node_value_t<DestTree> {}
                );
        }
    }
    
protected:
    template<typename NodeAdded_>
    _echo_NodeAddedInvoker(NodeAdded_&& nodeAdded)
        : storage_util<NodeAdded> { std::forward<NodeAdded_>(nodeAdded) }
    {
    }
};


template<
    class SourceTree,
    class DestTree,
    typename NodeValue = echo_default_node_value_tag, 
    typename NodeAdded = echo_default_node_added_tag
>
class BFSTreeEchoImpl
    : private _echo_NodeValueGetter<SourceTree, DestTree, NodeValue>,
    private _echo_NodeAddedInvoker<SourceTree, DestTree, NodeAdded>
{
    using dest_cursor_t = node_handle_t<DestTree>;
    using node_value_iter_t = _echo_NodeValueIterator<SourceTree, DestTree, NodeValue>;
    using node_value_range_t = boost::iterator_range<node_value_iter_t>;
    
    using node_value_getter_t = _echo_NodeValueGetter<SourceTree, DestTree, NodeValue>;
    using node_added_invoker_t = _echo_NodeAddedInvoker<SourceTree, DestTree, NodeAdded>;
public:
    template<
        typename NodeValue_ = NodeValue,
        typename NodeAdded_ = NodeAdded
    >   
    BFSTreeEchoImpl(
            const SourceTree& source,
            DestTree&         dest,
            NodeValue_&& nodeValue = {},
            NodeAdded_&& nodeAdded = {}
        )
        : _echo_NodeValueGetter<SourceTree, DestTree, NodeValue>( std::forward<NodeValue_>(nodeValue) ),
        _echo_NodeAddedInvoker<SourceTree, DestTree, NodeAdded>( std::forward<NodeAdded_>(nodeAdded) ),
        _sourceCursor(::Addle::aux_datatree::tree_root(source)),
        _destCursor(::Addle::aux_datatree::tree_root(dest))
    {
    }

    void run();
    
private:
    void enqueue()
    {
        auto i = ::Addle::aux_datatree::node_children_begin(_sourceCursor);
        auto end = ::Addle::aux_datatree::node_children_end(_sourceCursor);
        
        if (i != end)
        {
            _searchQueue.push_back({ 
                    std::move(i), 
                    std::move(end), 
                    _destCursor 
                });
        }
    }
    
    const_node_handle_t<SourceTree> _sourceCursor;
    node_handle_t<DestTree> _destCursor;
    
    struct SearchQueueEntry
    {
        child_node_handle_t<const_node_handle_t<SourceTree>> sourceChunkBegin;
        child_node_handle_t<const_node_handle_t<SourceTree>> sourceChunkEnd;
        node_handle_t<DestTree> destParent;
    };
    
    std::deque<SearchQueueEntry> _searchQueue;
};

template<class SourceTree, class DestTree, typename NodeValue, typename NodeAdded>
void BFSTreeEchoImpl<SourceTree, DestTree, NodeValue, NodeAdded>::run()
{
    assert(_sourceCursor && _destCursor);
    
    static_cast<const node_added_invoker_t*>(this)
        ->invoke(_sourceCursor, _destCursor);
        
    enqueue();
    
    // TODO: insert then invoke after add one by one for dest trees that don't 
    // have a batch insert overload
    
    while (!_searchQueue.empty())
    {
        const SearchQueueEntry& entry = _searchQueue.front();
        
        ::Addle::aux_datatree::node_insert_children(
                entry.destParent,
                ::Addle::aux_datatree::node_children_end(entry.destParent),
                node_value_range_t(
                    node_value_iter_t( entry.sourceChunkBegin, 
                        static_cast<const node_value_getter_t*>(this) ),
                    node_value_iter_t( entry.sourceChunkEnd )
                )
            );
        
        auto i = entry.sourceChunkBegin;
        auto j = ::Addle::aux_datatree::node_children_begin(entry.destParent);
        
        while (i != entry.sourceChunkEnd)
        {
            _sourceCursor = static_cast<const_node_handle_t<SourceTree>>(i);
            _destCursor = static_cast<node_handle_t<DestTree>>(j);
            
            static_cast<const node_added_invoker_t*>(this)
                ->invoke(_sourceCursor, _destCursor);
            
            enqueue();
            
            ::Addle::aux_datatree::node_sibling_increment(i);
            ::Addle::aux_datatree::node_sibling_increment(j);
        }
        
        _searchQueue.pop_front();
    }
}

template<
    class SourceTree,
    class DestTree,
    typename NodeValue = echo_default_node_value_tag,
    typename NodeAdded = echo_default_node_added_tag
>
void echo_tree(
    const SourceTree& source, 
    DestTree& dest,
    NodeValue&& nodeValue = {},
    NodeAdded&& nodeAdded = {}
)
{
    BFSTreeEchoImpl<
        SourceTree, 
        DestTree, 
        boost::remove_cv_ref_t<NodeValue>,
        boost::remove_cv_ref_t<NodeAdded>
    > impl(
        source,
        dest,
        std::forward<NodeValue>(nodeValue),
        std::forward<NodeAdded>(nodeAdded)
    );
        
    impl.run();
}

/*
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
}*/



} // namespace Addle::aux_datatree 
