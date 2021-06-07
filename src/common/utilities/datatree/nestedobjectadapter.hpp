#pragma once

#include <functional>
#include <iterator>
#include <variant>
#include <optional>

#include <boost/type_traits.hpp>

#include "./aux.hpp"

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

#ifdef ADDLE_TEST
class DataTree_UTest;
#endif

namespace Addle {
namespace aux_datatree {
    
template<class, typename, typename>
class NestedObjectAdapterImpl;

template<class, typename, typename>
class NestedObjectHandleImpl;

template<class>
struct nestedobject_rootHandleData {};

template<
    class NodeObject,
    typename GetChildNodes,
    typename RootRange //= void
>
struct nestedobject_adapter_traits
{
    using adapter_t = NestedObjectAdapterImpl<NodeObject, GetChildNodes, RootRange>;
    using handle_t = NestedObjectHandleImpl<NodeObject, GetChildNodes, RootRange>;
        
    using child_objects_range = std::invoke_result_t<GetChildNodes, const NodeObject&>;
    using child_objects_iterator = typename boost::range_iterator<const std::remove_reference_t<child_objects_range>>::type;
    
    using child_node_iterator = ConstChildNodeIterator<NestedObjectAdapterImpl<std::remove_const_t<NodeObject>, GetChildNodes, RootRange>>;
    
    using rootHandleData_t = nestedobject_rootHandleData<handle_t>;
    
    using root_iterator = boost::mp11::mp_eval_if_q<
            std::is_void<RootRange>,
            void,
            boost::mp11::mp_quote_trait<boost::range_iterator>,
            const RootRange
        >;
        
    using iterator_variant = boost::mp11::mp_eval_if<
            std::is_void<RootRange>,
            void,
            aux_range_utils::iterator_variant,
            child_objects_iterator,
            root_iterator
        >;
};

template<class NodeObject, typename GetChildNodes, typename RootRange>
struct nestedobject_rootHandleData<NestedObjectHandleImpl<NodeObject, GetChildNodes, RootRange>>
{
    using _traits = nestedobject_adapter_traits<NodeObject, GetChildNodes, RootRange>;
    using root_iterator_t = typename _traits::root_iterator;
    
    root_iterator_t rootRangeBegin;
    root_iterator_t rootRangeEnd;
};

template<class NodeObject, typename GetChildNodes>
struct nestedobject_rootHandleData<NestedObjectHandleImpl<NodeObject, GetChildNodes, void>>
{
    const NodeObject* rootObject;
};

template<
    class NodeObject,
    typename GetChildNodes,
    typename RootRange
>
class NestedObjectAdapterImpl
    : private storage_util<GetChildNodes>,
    private nestedobject_rootHandleData<
            typename nestedobject_adapter_traits<NodeObject, GetChildNodes, RootRange>::handle_t
        >
{   
private:
    using _traits = nestedobject_adapter_traits<NodeObject, GetChildNodes, RootRange>;
    
    using child_objects_range = typename _traits::child_objects_range;
    using child_objects_iterator = typename _traits::child_objects_iterator;
    using const_child_objects_iterator = child_objects_iterator;
    
    using rootHandleData_t = typename _traits::rootHandleData_t;
    
    static_assert(!std::is_reference_v<NodeObject>);
    static_assert(
        std::is_convertible_v<
            typename std::iterator_traits<child_objects_iterator>::reference,
            const NodeObject&
        >
    );
    
public:
    using handle_t = typename _traits::handle_t;
    
    using value_type = NodeObject;
    using reference = const NodeObject&;
    using iterator = ConstNodeIterator<
            NestedObjectAdapterImpl<NodeObject, GetChildNodes, RootRange>
        >;
    using const_iterator = iterator;
    
    template<
        typename GetChildNodes_
    >
    NestedObjectAdapterImpl(
        GetChildNodes_&& getChildNodes,
        rootHandleData_t&& rootData = {}
    )
        : storage_util<GetChildNodes> {
            std::forward<GetChildNodes_>(getChildNodes) 
        },
        rootHandleData_t(std::move(rootData))
    {
    }
    
    const_iterator begin() const
    {
        auto result = const_iterator(
                ::Addle::aux_datatree::DFSExtendedNode<handle_t, true>(
                    datatree_root(*this)
                )
            );
        
        if constexpr (!std::is_void_v<RootRange>)
        {
            // For a root range adapter, the root node is an artifact for
            // consistency with the DataTree API, but it is meaningless (and 
            // problematic) when treating the adapter as a range of TestObject, 
            // and so is skipped.
            ++result;
        }
        
        return result;
    }
    
    const_iterator end() const
    {
        return const_iterator(
                ::Addle::aux_datatree::DFSExtendedNode<handle_t, true>()
            );
    }
    
private:
    friend handle_t datatree_root(const NestedObjectAdapterImpl& adapter)
    {
        return handle_t(static_cast<const rootHandleData_t*>(std::addressof(adapter)));
    }
    
    template<class, typename, typename>
    friend class NestedObjectHandleImpl;//<NodeObject, GetChildNodes, RootRange>;

#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
};

template<class NodeObject, typename GetChildNodes, typename RootRange>
class NestedObjectHandleImpl
{
    using _traits = nestedobject_adapter_traits<NodeObject, GetChildNodes, RootRange>;
    
    // type of pointer to the accessor storage for this handle type
    using adapter_t =               typename _traits::adapter_t;
    using child_object_getter = storage_util<GetChildNodes>;
    using child_object_range_t =    typename _traits::child_objects_range;
    using child_node_iterator_t =   typename _traits::child_node_iterator;
    
public:
    NestedObjectHandleImpl() = default;
    NestedObjectHandleImpl(const NestedObjectHandleImpl&) = default;
    NestedObjectHandleImpl& operator=(const NestedObjectHandleImpl&) = default;
    
    explicit NestedObjectHandleImpl(const typename _traits::rootHandleData_t* rootData)
        : _adapter(static_cast<const adapter_t*>(rootData))
    {
    }
    
    NestedObjectHandleImpl(const adapter_t* adapter, const NodeObject* nodePtr)
        : _adapter(adapter), _nodePtr(nodePtr)
    {
    }
    
    explicit operator bool() const { return _nodePtr || _adapter; }
    bool operator!() const { return !_nodePtr && !_adapter; }
    
    bool operator==(const NestedObjectHandleImpl& other) const
    {
        return _nodePtr == other._nodePtr && _adapter == other._adapter; 
    }
    
    bool operator!=(const NestedObjectHandleImpl& other) const 
    { 
        return _nodePtr != other._nodePtr || _adapter != other._adapter;
    }
    
    bool hasValue() const { return static_cast<bool>(_nodePtr); }
    
    const NodeObject& operator*() const
    { 
        assert(_nodePtr);
        return *_nodePtr;
    }
    
private:
    child_node_iterator_t childrenBegin() const
    {
        if (_nodePtr)
        {
            auto&& childrenRange = std::invoke(
                    static_cast<const child_object_getter*>(_adapter)->get(),
                    *_nodePtr
                );

            return child_node_iterator_t(
                    _adapter,
                    boost::begin(const_cast<const child_object_range_t&>(childrenRange))
                );
        }
        else
        {
            assert(_adapter);
            return child_node_iterator_t(
                    _adapter,
                    static_cast<const typename _traits::rootHandleData_t*>(_adapter)->rootRangeBegin
                );
        }
    }
    
    child_node_iterator_t childrenEnd() const
    {
        if (_nodePtr)
        {
            auto&& childrenRange = std::invoke(
                    static_cast<const child_object_getter*>(_adapter)->get(),
                    *_nodePtr
                );

            return child_node_iterator_t(
                    _adapter,
                    boost::end(const_cast<const child_object_range_t&>(childrenRange))
                );
        }
        else
        {
            assert(_adapter);
            return child_node_iterator_t(
                    _adapter,
                    static_cast<const typename _traits::rootHandleData_t*>(_adapter)->rootRangeEnd
                );
        }
    }
    
    const adapter_t* _adapter = nullptr;
    const NodeObject* _nodePtr = nullptr;
    
    friend child_node_iterator_t datatree_node_children_begin(const NestedObjectHandleImpl& node)
    {
        return Q_LIKELY(node) ? node.childrenBegin() : child_node_iterator_t {};
    }
    
    friend child_node_iterator_t datatree_node_children_end(const NestedObjectHandleImpl& node)
    {
        return Q_LIKELY(node) ? node.childrenEnd() : child_node_iterator_t {};
    }
    
    friend bool datatree_node_has_value(const NestedObjectHandleImpl& node)
    {
        return node && node._nodePtr;
    }
    
    friend const NodeObject& datatree_node_value(const NestedObjectHandleImpl& node)
    {
        assert(node && node._nodePtr);
        return *(node._nodePtr);
    }
    
#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
    
    template<class, typename, typename>
    friend class NestedObjectAdapterImpl;
};

template<
    class NodeObject,
    typename GetChildNodes
>
class NestedObjectHandleImpl<NodeObject, GetChildNodes, void>
{
    using _traits = nestedobject_adapter_traits<NodeObject, GetChildNodes, void>;
    
    // type of pointer to the accessor storage for this handle type
    using adapter_t =              typename _traits::adapter_t;
    using child_object_getter = storage_util<GetChildNodes>;
    using child_object_range_t =    typename _traits::child_objects_range;
    using child_node_iterator_t =   typename _traits::child_node_iterator;
    
public:
    NestedObjectHandleImpl() = default;
    NestedObjectHandleImpl(const NestedObjectHandleImpl&) = default;
    NestedObjectHandleImpl& operator=(const NestedObjectHandleImpl&) = default;
    
    explicit NestedObjectHandleImpl(const typename _traits::rootHandleData_t* rootData)
        : _adapter(static_cast<const adapter_t*>(rootData)), _nodePtr(rootData->rootObject)
    {
    }
    
    NestedObjectHandleImpl(const adapter_t* adapter, const NodeObject* nodePtr)
        : _adapter(adapter), _nodePtr(nodePtr)
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
                static_cast<const child_object_getter*>(_adapter)->get(),
                *_nodePtr
            );

        return child_node_iterator_t(
                _adapter,
                boost::begin(const_cast<const child_object_range_t&>(childrenRange))
            );
    }
    
    child_node_iterator_t childrenEnd() const
    {
        auto&& childrenRange = std::invoke(
                static_cast<const child_object_getter*>(_adapter)->get(),
                *_nodePtr
            );
            
        return child_node_iterator_t(
                _adapter,
                boost::end(const_cast<const child_object_range_t&>(childrenRange))
            );
    }
    
    const adapter_t* _adapter = nullptr;
    const NodeObject* _nodePtr = nullptr;
    
    friend child_node_iterator_t datatree_node_children_begin(const NestedObjectHandleImpl& node)
    {
        return Q_LIKELY(node) ? node.childrenBegin() : child_node_iterator_t {};
    }
    
    friend child_node_iterator_t datatree_node_children_end(const NestedObjectHandleImpl& node)
    {
        return Q_LIKELY(node) ? node.childrenEnd() : child_node_iterator_t {};
    }
    
    friend const NodeObject& datatree_node_value(const NestedObjectHandleImpl& node)
    {
        assert(node);
        return *node;
    }
    
#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
    
    template<class, typename, typename>
    friend class NestedObjectAdapterImpl;
};


template<
    class NodeObject,
    typename GetChildNodes,
    typename RootRange
>
struct datatree_traits<
        NestedObjectAdapterImpl<NodeObject, GetChildNodes, RootRange>
    >
{
    using node_handle = typename nestedobject_adapter_traits<NodeObject, GetChildNodes, RootRange>::handle_t;
    using const_node_handle = node_handle;
    using node_value_type = NodeObject;
};

template<
    class NodeObject,
    typename GetChildNodes,
    typename RootRange,
    bool IsConst
>
class ChildNodeIterator<
        NestedObjectAdapterImpl<NodeObject, GetChildNodes, RootRange>,
        IsConst
    >
    : public boost::iterator_adaptor<
        ChildNodeIterator<NestedObjectAdapterImpl<NodeObject, GetChildNodes, RootRange>, IsConst>,
        boost::mp11::mp_if<
            std::is_void<RootRange>,
            typename nestedobject_adapter_traits<NodeObject, GetChildNodes, RootRange>::child_objects_iterator,
            typename nestedobject_adapter_traits<NodeObject, GetChildNodes, RootRange>::iterator_variant
        >,
        const NodeObject
    >
{
    using _traits = nestedobject_adapter_traits<NodeObject, GetChildNodes, RootRange>;
    
    using iterator_adaptor_t = boost::iterator_adaptor<
        ChildNodeIterator<NestedObjectAdapterImpl<NodeObject, GetChildNodes, RootRange>, IsConst>,
        boost::mp11::mp_if<
            std::is_void<RootRange>,
            typename _traits::child_objects_iterator,
            typename _traits::iterator_variant
        >,
        const NodeObject
    >;
    
    using base_t = boost::mp11::mp_if<
            std::is_void<RootRange>,
            typename _traits::child_objects_iterator,
            typename _traits::iterator_variant
        >;
    
    using handle_t =                typename _traits::handle_t;
    using nestedobject_adapter_t =  typename _traits::adapter_t;
    
public:
    ChildNodeIterator() = default;
    ChildNodeIterator(const ChildNodeIterator&) = default;
    ChildNodeIterator(ChildNodeIterator&&) = default;
    
    template<
        typename MutableIterator, 
        std::enable_if_t<
            IsConst
            && std::is_same<MutableIterator, ChildNodeIterator<nestedobject_adapter_t, false>>::value, 
            void*
        > = nullptr
    >
    ChildNodeIterator(const MutableIterator& i)
        : iterator_adaptor_t(i.base()), _adapter(i.adapter)
    {
    }
    
    ChildNodeIterator(const nestedobject_adapter_t* adapter, base_t iter)
        : iterator_adaptor_t(iter), 
        _adapter(adapter)
    {
    }
    
    ChildNodeIterator& operator=(const ChildNodeIterator&) = default;
    ChildNodeIterator& operator=(ChildNodeIterator&&) = default;
    
    inline operator handle_t () const { return handle_t(_adapter, std::addressof(*this->base())); }
    
private:
    inline _dereferenced_t<handle_t> dereference() const { return **this->base(); }
//     inline bool equal(const ChildNodeIterator& x) const { return this->base() == x.base(); }
//     inline bool equal(const handle_t& x) const { 
//         return this->base() != typename nestedobject_adapter_t::child_objects_iterator {} 
//             && *this->base() == x; 
//     }
    
    const nestedobject_adapter_t* _adapter = nullptr;
    
    friend class ChildNodeIterator<nestedobject_adapter_t, true>;
    friend class boost::iterator_core_access;
};

} // namespace aux_datatree

template<
    class NodeObject,
    typename GetChildNodes
>
auto make_nested_object_adapter(
        const NodeObject& root,
        GetChildNodes&& getChildNodes
    )
{
    using rootHandleData_t = typename aux_datatree::nestedobject_adapter_traits<
            boost::remove_cv_ref_t<NodeObject>,
            boost::remove_cv_ref_t<GetChildNodes>,
            void
        >::rootHandleData_t;
    
    return aux_datatree::NestedObjectAdapterImpl<   
            boost::remove_cv_ref_t<NodeObject>,
            boost::remove_cv_ref_t<GetChildNodes>,
            void
        >(
            std::forward<GetChildNodes>(getChildNodes),
            rootHandleData_t { std::addressof(root)}
        );
}


template<
    class RootRange,
    typename GetChildNodes
>
auto make_root_range_nested_object_adapter(
        const RootRange& rootRange,
        GetChildNodes&& getChildNodes
    )
{
    using node_object_t = typename boost::range_value<RootRange>::type;
    
    using rootHandleData_t = typename aux_datatree::nestedobject_adapter_traits<
            boost::remove_cv_ref_t<node_object_t>,
            boost::remove_cv_ref_t<GetChildNodes>,
            boost::remove_cv_ref_t<RootRange>
        >::rootHandleData_t;
    
    return aux_datatree::NestedObjectAdapterImpl<   
            boost::remove_cv_ref_t<node_object_t>,
            boost::remove_cv_ref_t<GetChildNodes>,
            boost::remove_cv_ref_t<RootRange>
        >(
            std::forward<GetChildNodes>(getChildNodes),
            rootHandleData_t { 
                boost::begin(rootRange), 
                boost::end(rootRange)
            }
        );
}


} // namespace Addle
