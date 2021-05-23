#pragma once

#include "./aux.hpp"
#include <functional>
#include <iterator>

// NOTE this header outlines a concept whose name uses the word "adapter",
// spelled with an 'e'. Other places use the word "adaptor" spelled with an 'o',
// for consistency with some external libraries that prefer that spelling, e.g.,
// Boost.Iterator.

/** 
 * A "nested object" is typically an instance of some class T which has a 
 * collection of T as a non-static data member. A nested object adapter takes 
 * this generic structure and exposes it to the data tree API.
 * 
 * The nested object adapter is strictly read-only. A mutable version is 
 * conceivable but it would be very tricky to work with copy-on-write containers
 * or other implicitly shared data.
 */

namespace Addle {
namespace aux_datatree {
    
template<class, typename>
class NestedObjectAdapterImpl;

template<class, typename>
class NestedObjectHandleImpl;

template<
    class NodeObject,
    typename GetChildNodes
>
struct nestedobject_adapter_traits
{
    using handle_t = NestedObjectHandleImpl<NodeObject, GetChildNodes>;
    using child_objects_range = boost::remove_cv_ref_t<std::invoke_result_t<GetChildNodes, const NodeObject&>>;
    using child_objects_iterator = typename boost::range_iterator<const child_objects_range>::type;
    using child_node_iterator = ConstChildNodeIterator<NestedObjectAdapterImpl<NodeObject, GetChildNodes>>;
};

template<typename T>
struct _nestedobject_basic_accessor_storage
{
    std::decay_t<T> accessor;
    
    inline T& get() { return static_cast<T&>(accessor); }
    inline const T& get() const { return static_cast<const T&>(accessor); }
};

template<typename T>
struct _nestedobject_ebo_accessor_storage : T
{
    inline T& get() { return *static_cast<T*>(this); }
    inline const T& get() const { return *static_cast<const T*>(this); }
};

template<typename T>
using nestedobject_children_accessor_storage = typename boost::mp11::mp_if<
        boost::mp11::mp_and<
            std::is_class<T>,
#ifdef __cpp_lib_is_final
            boost::mp11::mp_not< std::is_final<T> >,
#endif
            std::is_empty<T>
        >,
        boost::mp11::mp_defer<_nestedobject_ebo_accessor_storage, T>,
        boost::mp11::mp_defer<_nestedobject_basic_accessor_storage, T>
    >::type;

template<
    class NodeObject,
    typename GetChildNodes
>
class NestedObjectAdapterImpl
    : private nestedobject_children_accessor_storage<GetChildNodes>
{   
private:
    using _traits = nestedobject_adapter_traits<NodeObject, GetChildNodes>;
    
    //using top_level_nodes_range = boost::remove_cv_ref_t<std::invoke_result_t<GetTopLevelNodes, RootObject&>>;
    //using top_level_node_iterator = typename boost::range_iterator<top_level_nodes_range>::type;
    
    using handle_t = typename _traits::handle_t;
    using const_handle_t = handle_t;
    using child_objects_range = typename _traits::child_objects_range;
    using child_objects_iterator = typename _traits::child_objects_iterator;
    using const_child_objects_iterator = child_objects_iterator;
    
    //static_assert(std::is_convertible_v<typename std::iterator_traits<top_level_node_iterator>::reference_type, NodeObject&>);
    static_assert(!std::is_reference_v<NodeObject>);
    static_assert(
        std::is_convertible_v<
            typename std::iterator_traits<child_objects_iterator>::reference,
            const NodeObject&
        >
    );
    
public:
    using value_type = NodeObject;
    using reference = const NodeObject&;
    using iterator = ConstNodeIterator<
            NestedObjectAdapterImpl<const NodeObject, GetChildNodes>
        >;
    using const_iterator = iterator;
    
    template<
        typename GetChildNodes_
    >
    NestedObjectAdapterImpl(
        NodeObject* root,
        GetChildNodes_&& getChildNodes
    )
        : nestedobject_children_accessor_storage<GetChildNodes> {
            std::forward<GetChildNodes_>(getChildNodes)
        },
        _root(root)
    {
    }
    
    const_iterator begin() const
    {
        return const_iterator(
                ::Addle::aux_datatree::node_dfs_begin(datatree_root(*this))
            );
    }
    
    const_iterator end() const
    {
        return const_iterator(
                ::Addle::aux_datatree::node_dfs_end(datatree_root(*this))
            );
    }
    
private:
    const NodeObject* _root;
    
    friend handle_t datatree_root(NestedObjectAdapterImpl& adapter)
    {
        return handle_t(
            static_cast<
                const nestedobject_children_accessor_storage<GetChildNodes>*
            >(std::addressof(adapter)), 
            adapter._root
        );
    }
    
    friend const_handle_t datatree_root(const NestedObjectAdapterImpl& adapter)
    {
        return const_handle_t(
            static_cast<
                const nestedobject_children_accessor_storage<GetChildNodes>*
            >(std::addressof(adapter)), 
            adapter._root
        );
    }
    
    friend class NestedObjectHandleImpl<NodeObject, GetChildNodes>;
};

template<
    class NodeObject,
    typename GetChildNodes
    //, bool PreventChildObjectRangeCopy
>
class NestedObjectHandleImpl
{
    using child_object_range_t = std::invoke_result_t<
            const GetChildNodes&,
            const NodeObject&
        >;
    
    using qual_child_object_range_t = boost::mp11::mp_if<
            boost::mp11::mp_or<
//                 boost::mp11::mp_bool<!PreventChildObjectRangeCopy>,
                std::is_lvalue_reference<child_object_range_t>
            >,
            child_object_range_t,
            boost::copy_reference_t<
                const std::remove_reference_t<child_object_range_t>,
                child_object_range_t
            >
        >;
    
    using _traits = nestedobject_adapter_traits<NodeObject, GetChildNodes>;
    
    // type of pointer to the accessor storage for this handle type
    using accessor_storage_ptr_t = const nestedobject_children_accessor_storage<GetChildNodes>*;
        
    using child_node_iterator_t = typename _traits::child_node_iterator;
    
public:
    NestedObjectHandleImpl() = default;
    NestedObjectHandleImpl(const NestedObjectHandleImpl&) = default;
    NestedObjectHandleImpl& operator=(const NestedObjectHandleImpl&) = default;
    
    NestedObjectHandleImpl(accessor_storage_ptr_t accessors, const NodeObject* nodePtr)
        : _accessors(accessors), _nodePtr(nodePtr)
    {
    }
    
    explicit operator bool() const { return _nodePtr; }
    bool operator!() const { return !_nodePtr; }
    
    bool operator==(const NestedObjectHandleImpl& other) const { return _nodePtr == other._nodePtr; }
    bool operator!=(const NestedObjectHandleImpl& other) const { return _nodePtr != other._nodePtr; }
    
    const NodeObject& operator*() const { return *_nodePtr; }
    
private:
    child_node_iterator_t childrenBegin() const
    {
        auto&& childrenRange = std::invoke(
                _accessors->get(),
                *_nodePtr
            );

        return child_node_iterator_t(
                _accessors,
                boost::begin(const_cast<const child_object_range_t&&>(childrenRange))
            );
    }
    
    child_node_iterator_t childrenEnd() const
    {
        auto&& childrenRange = std::invoke(
                _accessors->get(),
                *_nodePtr
            );
            
        return child_node_iterator_t(
                _accessors,
                boost::end(const_cast<const child_object_range_t&&>(childrenRange))
            );
    }
    
    accessor_storage_ptr_t _accessors = nullptr;
    const NodeObject* _nodePtr = nullptr;
    
    friend child_node_iterator_t datatree_node_children_begin(const NestedObjectHandleImpl& node)
    {
        return Q_LIKELY(node) ? node.childrenBegin() : child_node_iterator_t {};
    }
    
    friend child_node_iterator_t datatree_node_children_end(const NestedObjectHandleImpl& node)
    {
        return Q_LIKELY(node) ? node.childrenEnd() : child_node_iterator_t {};
    }
};

template<
    class NodeObject,
    typename GetChildNodes
>
struct datatree_traits<
        NestedObjectAdapterImpl<NodeObject, GetChildNodes>
    >
{
    using node_handle = NestedObjectHandleImpl<NodeObject, GetChildNodes>;
    using const_node_handle = NestedObjectHandleImpl<const NodeObject, GetChildNodes>;
    using node_value_type = NodeObject;
};

template<
    class NodeObject,
    typename GetChildNodes,
    bool IsConst
>
class ChildNodeIterator<
        NestedObjectAdapterImpl<NodeObject, GetChildNodes>,
        IsConst
    >
    : public boost::iterator_facade<
        ChildNodeIterator<NestedObjectAdapterImpl<NodeObject, GetChildNodes>, IsConst>,
        std::remove_reference_t<
            _dereferenced_t<NestedObjectHandleImpl<NodeObject, GetChildNodes>>
        >,
        typename boost::range_category<
            typename nestedobject_adapter_traits<NodeObject, GetChildNodes>::child_objects_range
        >::type,
        _dereferenced_t<NestedObjectHandleImpl<NodeObject, GetChildNodes>>
    >
{
    using _traits = nestedobject_adapter_traits<NodeObject, GetChildNodes>;
    
    using adapter_t = NestedObjectAdapterImpl<NodeObject, GetChildNodes>;
    using accessor_storage_ptr_t = const nestedobject_children_accessor_storage<GetChildNodes>*;

    using handle_t = typename _traits::handle_t;
    
    using child_objects_iterator = typename _traits::child_objects_iterator;
        
public:
    ChildNodeIterator() = default;
    ChildNodeIterator(const ChildNodeIterator&) = default;
    ChildNodeIterator(ChildNodeIterator&&) = default;
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, ChildNodeIterator<adapter_t, false>>::value, 
            void*
        > = nullptr
    >
    ChildNodeIterator(const MutableIterator& i)
        : _accessors(i._accessors), _iter(i._iter)
    {
    }
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, ChildNodeIterator<adapter_t, false>>::value,
            void*
        > = nullptr
    >
    ChildNodeIterator(MutableIterator&& i)
        : _accessors(i._accessors), _iter(std::move(i._iter))
    {
    }
    
    ChildNodeIterator(accessor_storage_ptr_t accessors, child_objects_iterator iter)
        : _accessors(accessors), _iter(iter)
    {
    }
    
    ChildNodeIterator& operator=(const ChildNodeIterator&) = default;
    ChildNodeIterator& operator=(ChildNodeIterator&&) = default;
    
    inline operator handle_t () const { return handle_t(_accessors, std::addressof(*_iter)); }
    
private:
    inline _dereferenced_t<handle_t> dereference() const { return **_iter; }
    inline bool equal(const ChildNodeIterator& x) const { return _iter == x._iter; }
    inline bool equal(const handle_t& x) const { return _iter != typename adapter_t::child_objects_iterator {} && *_iter == x; }
    inline void increment() { ++_iter; }
    
    accessor_storage_ptr_t _accessors = nullptr;
    child_objects_iterator _iter = {};
    
    friend class ChildNodeIterator<adapter_t, true>;
    friend class boost::iterator_core_access;
};

}

template<
    class NodeObject,
    typename GetChildNodes
>
auto make_nested_object_adapter(
        NodeObject&& root,
        GetChildNodes&& getChildNodes
    )
{
    return aux_datatree::NestedObjectAdapterImpl<   
            std::remove_reference_t<NodeObject>,
            boost::remove_cv_ref_t<GetChildNodes>
        >(
            std::addressof(root),
            std::forward<GetChildNodes>(getChildNodes)
        );
}

}
