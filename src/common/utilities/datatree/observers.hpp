#pragma once

#include <QSharedData>

#include <any>
#include <deque>
#include <variant>
#include <map>
#include <QMap>
#include "./aux.hpp"

#include <boost/range/adaptor/reversed.hpp>
#include <boost/mp11.hpp>
#include <boost/type_traits.hpp>

namespace Addle {
namespace aux_datatree {

struct NodeUnit
{
    NodeAddress address;
    std::size_t length;
    
    friend bool operator<(const NodeUnit& lhs, const NodeUnit& rhs)
    {
        return lhs.address < rhs.address; 
    }
    
    friend bool operator<(const NodeAddress& lhs, const NodeUnit& rhs)
    {
        return lhs < rhs.address; 
    }
    
    friend bool operator<(const NodeUnit& lhs, const NodeAddress& rhs)
    {
        return lhs.address < rhs; 
    }
    
#ifdef ADDLE_DEBUG
    friend QDebug operator<<(QDebug debug, const NodeUnit& unit)
    {
        return debug << unit.address
            << unit.length;
    }
#endif
};

template<class Tree>
class ExtensibleNodeEventBase
{
    using handle_t = node_handle_t<Tree>; 
    using unit_list_t = QList<NodeUnit>;
    static constexpr std::size_t MAX_OBSERVER_CACHE_SIZE = 256;
public:
    ExtensibleNodeEventBase() = default;
    ExtensibleNodeEventBase(const ExtensibleNodeEventBase&) = default;
    ExtensibleNodeEventBase(ExtensibleNodeEventBase&&) = default;
    
    ExtensibleNodeEventBase& operator=(const ExtensibleNodeEventBase&) = default;
    ExtensibleNodeEventBase& operator=(ExtensibleNodeEventBase&&) = default;
    
protected:
    inline unit_list_t primaryUnits() const
    {
        if (!_data) return unit_list_t();
        
        if (_data->primaryUnitsCache.isEmpty())
        {
            _data->primaryUnitsCache.reserve(_data->primaryUnitsCount);
            for (const auto& unitList : _data->primaryUnitsByParent)
            {
                for (NodeUnit u : unitList)
                {
                    _data->primaryUnitsCache.append(u);
                }
            }
        }
        
        return _data->primaryUnitsCache;
    }
    
    inline NodeAddress mapSubtractive(const NodeAddress& address) const
    {
        if (!_data) return address;
        if (address.isRoot()) return NodeAddress();
        
        if (_data->subtractiveMappingCache.contains(address))
        {
            return noDetach(_data->subtractiveMappingCache)[address];
        }
        
        NodeAddress ancestor;
        NodeAddress result;
        while (ancestor.size() < address.size())
        {
            std::size_t index = address[result.size()];
            if (_data->primaryUnitsByParent.contains(ancestor))
            {
                for (const auto& removal : _data->primaryUnitsByParent[ancestor])
                {
                    if (removal.address > address)
                        break;
                    
                    index -= removal.length;
                }
            }
            
            result = std::move(result) << index;
            ancestor = std::move(ancestor) << address[ancestor.size()];
        }
        
        if (_data->subtractiveMappingCache.size() >= MAX_OBSERVER_CACHE_SIZE)
            _data->subtractiveMappingCache.clear();
        
        _data->subtractiveMappingCache[address] = result;
        
        return result;
    }
    
    inline NodeAddress mapAdditive(const NodeAddress& address) const
    {
        if (!_data) return address;
        if (address.isRoot()) return NodeAddress();
        
        if (_data->additiveMappingCache.contains(address))
        {
            return noDetach(_data->additiveMappingCache)[address];
        }
        
        NodeAddress result;
        while (result.size() < address.size())
        {
            std::size_t index = address[result.size()];
            if (_data->primaryUnitsByParent.contains(result))
            {
                for (const auto& removal : _data->primaryUnitsByParent[result])
                {
                    NodeAddress test = result << index;
                    if (removal.address > test)
                        break;
                    
                    index += removal.length;
                }
            }
            
            result = std::move(result) << index;
        }
        
        if (_data->additiveMappingCache.size() >= MAX_OBSERVER_CACHE_SIZE)
            _data->additiveMappingCache.clear();
        
        _data->additiveMappingCache[address] = result;
        
        return result;
    }
    
    inline bool isAffected(const NodeAddress& address) const
    {
        if (!_data) return false;
        
        NodeAddress ancestor;
        while (ancestor.size() < address.size())
        {
            if (_data->primaryUnitsByParent.contains(ancestor))
            {
                auto& peers = _data->primaryUnitsByParent[ancestor];
            
                auto upperBound = std::upper_bound(peers.begin(), peers.end(), address);
                if (upperBound == peers.begin())
                    continue;
                
                auto lowerIndex = (upperBound - 1)->address.lastIndex();
                auto lowerLength = (upperBound - 1)->length;
                
                if (lowerIndex + lowerLength > address[ancestor.size()])
                    return true;
            }
            ancestor = std::move(ancestor) << address[ancestor.size()];
        }
        
        return false;
    }
    
    template<bool forward>
    inline std::size_t childCount_impl(NodeAddress address, std::size_t progress) const
    {
        if (!_data || !_data->childCountByParent.contains(address)) 
            return 0;
        
        if (progress >= _data->primaryUnitsCount)
            return _data->childCountByParent[address];
        
        if (_data->childCountCache.contains(qMakePair(progress, address)))
            return _data->childCountCache[qMakePair(progress, address)];
        
        std::size_t result;
        if constexpr (forward)
            result = 0;
        else
            result = _data->childCountByParent[address];
        
        unit_list_t primaryUnits = this->primaryUnits();
        
        for (const auto& unit : _data->primaryUnitsByParent[address])
        {
            std::size_t index = std::distance(
                    primaryUnits.begin(),
                    std::lower_bound(
                        primaryUnits.begin(),
                        primaryUnits.end(),
                        unit
                    )
                );
            
            if constexpr (forward)
            {
                if (index < progress)
                    result += unit.length;
            }
            else
            {
                if ((_data->primaryUnitsCount - index) < progress)
                    result -= unit.length;
            }
        }
        
        if (_data->childCountCache.size() >= MAX_OBSERVER_CACHE_SIZE)
            _data->childCountCache.clear();
        
        _data->childCountCache[qMakePair(progress, address)] = result;
        
        return result;
    }
    
    void initializeData(handle_t root)
    {
        if (!_data)
            _data = new Data(root);
        else
            validateRoot(root);
    }
    
    void validateRoot(handle_t root, handle_t default_ = {}) const
    {
        // TODO: exception
        if (default_)
            assert(_data ? root == _data->root : root == default_);
        else
            assert(!_data || root == _data->root);
    }
    
    void extendPrimaryUnits(NodeUnit&& unit)
    {
        _data->additiveMappingCache.clear();
        _data->subtractiveMappingCache.clear();
        _data->childCountCache.clear();
        _data->primaryUnitsCache.clear();
        
        std::size_t index = unit.address.lastIndex();
        
        {
            // erase any previously recorded primary removals that are 
            // descendants of this removal. presumably they have consequently
            // been discovered as secondary removals so we don't need to worry
            // about that.
            
            auto i = _data->primaryUnitsByParent.lowerBound(unit.address);
            while ( i != _data->primaryUnitsByParent.end()
                && (unit.address == i.key() || unit.address.isAncestorOf(i.key())) )
            {
                _data->primaryUnitsCount -= i->size();
                _data->childCountByParent.remove(i.key());
                
                i = _data->primaryUnitsByParent.erase(i);
            }
        }
        
        auto parentAddress = unit.address.parent();
        
        if (_data->primaryUnitsByParent.contains(parentAddress))
        {
            // there are other removals in this record with the same parent.
            // check to see if this is adjacent to any of them and possibly
            // merge them together
            
            auto& peers = _data->primaryUnitsByParent[parentAddress];
            auto upperBound = std::upper_bound(peers.begin(), peers.end(), unit.address);
            
            bool mergeWithUpper = false;
            
            if (upperBound != peers.end())
                mergeWithUpper = (index + unit.length == (*upperBound).address.lastIndex());
            
            if (upperBound != peers.begin())
            {
                auto lowerBound = upperBound - 1;
                
                if ((*lowerBound).address.lastIndex() + (*lowerBound).length == index)   
                {
                    // current removal is adjacent to the one prior, 
                    // merge with it.
                    (*lowerBound).length += unit.length;
                    
                    if (mergeWithUpper)
                    {
                        // and to the one after, merge that with the one prior
                        // as well then erase it. (last operation performed with 
                        // iterator so it will be fine to invalidate it)
                        (*lowerBound).length += (*upperBound).length;
                        peers.erase(upperBound);
                        --(_data->primaryUnitsCount);
                    }
                    return;
                }
            }
            
            if (mergeWithUpper)
            {
                // the current removal is adjacent to only the one after, 
                // merge with it
                (*upperBound).address = unit.address;
                (*upperBound).length += unit.length;
                return;
            }
            
            // the current removal is isolated, don't merge with anything.
            peers.insert(upperBound, std::move(unit));
            ++(_data->primaryUnitsCount);
        }
        else
        {
            // the record has no other removals with the same parent,
            // so just record this and move on.
            _data->primaryUnitsByParent[parentAddress] = { std::move(unit) };
            ++(_data->primaryUnitsCount);
        }
    }
    
    template<typename Range>
    inline void extendSecondaryUnits(Range&& range)
    {
        std::size_t oldSecondariesSize = _data->secondaryUnits.size();
        
        reserve_for_size_if_cheap(_data->secondaryUnits, range);
                
        for (auto&& unit : range)
        {
            _data->secondaryUnits.append(std::move(unit));
        }
        
        std::inplace_merge(
                _data->secondaryUnits.begin(),
                _data->secondaryUnits.begin() + oldSecondariesSize,
                _data->secondaryUnits.end()
            );
    }
    
    inline void extendSecondaryUnits(NodeUnit&& unit)
    {
        auto i = std::upper_bound(
                _data->secondaryUnits.begin(),
                _data->secondaryUnits.end(),
                unit
            );
        _data->secondaryUnits.insert(i, std::move(unit));
    }
    
    struct Data : QSharedData
    {
        inline Data(handle_t root_ = {}) : root(root_) {}
        Data(const Data&) = default;
        
        handle_t root;
        
        QHash<NodeAddress, std::size_t> childCountByParent;
        QMap<NodeAddress, unit_list_t> primaryUnitsByParent;
        std::size_t primaryUnitsCount = 0;
        
        unit_list_t secondaryUnits;
        
        mutable unit_list_t primaryUnitsCache;
        mutable QHash<NodeAddress, NodeAddress> additiveMappingCache;
        mutable QHash<NodeAddress, NodeAddress> subtractiveMappingCache;
        mutable QHash<QPair<std::size_t, NodeAddress>, std::size_t> childCountCache;
    };
    
    QSharedDataPointer<Data> _data;
    template<class, class> friend class NostalgicVisitorBase;
};

template<class Tree>
class NostalgicNodeEventVisitor;

template<class Tree>
class AddNodesObserver;

template<class Tree>
class NodesAddedEvent : public ExtensibleNodeEventBase<Tree>
{
public:
    using handle_t = node_handle_t<Tree>;
    
    NodesAddedEvent() = default;
    NodesAddedEvent(const NodesAddedEvent&) = default;
    NodesAddedEvent(NodesAddedEvent&&) = default;
    
    NodesAddedEvent& operator=(const NodesAddedEvent&) = default;
    NodesAddedEvent& operator=(NodesAddedEvent&&) = default;
        
    inline NodeAddress mapForward(NodeAddress address) const
    {
        return this->mapAdditive(address);
    }
    
    inline NodeAddress mapBackward(NodeAddress address) const
    {
        return this->mapSubtractive(address);
    }
    
    inline std::size_t childCount(NodeAddress address, std::size_t progress) const
    {
        return this->template childCount_impl<true>(address, progress);
    }
    
    friend class AddNodesObserver<Tree>;
    friend class NostalgicNodeEventVisitor<Tree>;
};

template<class Tree>
class AddNodesObserver
{
public:
    using handle_t = node_handle_t<Tree>;
    
    AddNodesObserver() = default;
    
    AddNodesObserver(const AddNodesObserver&) = delete;
    AddNodesObserver(AddNodesObserver&&) = delete;
    AddNodesObserver& operator=(const AddNodesObserver&) = delete;
    AddNodesObserver& operator=(AddNodesObserver&&) = delete;
    
    inline const NodesAddedEvent<Tree>& event() const & { return _event; }
    inline NodesAddedEvent<Tree>&& event() && { return std::move(_event); }
    
    inline operator const NodesAddedEvent<Tree>& () const & { return _event; }
    inline operator NodesAddedEvent<Tree>&& () && { return std::move(_event); }
    
//     /**
//      * addNode and addNodeRange add an existing node or range of existing nodes 
//      * to the observer. This allows the tree's native API to be used to do the 
//      * actual node creation and insertion.
//      * 
//      * If addNodeRange is given a range that is forward iterable, i.e., that it
//      * can be repeat iterated without side-effects, the range is returned again
//      * as a convenience. (It is returned by copy if given as an r-value and by 
//      * l-value reference if given by l-value). If the range is not forward
//      * iterable then nothing is returned.
//      */
//     
//     template<typename Range,
//         std::enable_if_t<
//             std::is_convertible_v<
//                 typename boost::range_category<boost::remove_cv_ref_t<Range>>::type, 
//                 std::forward_iterator_tag
//             >,
//             void*> = nullptr
//         >
//     inline Range addNodeRange(Range&& range)
//     {
//         static_assert(
//             std::is_convertible_v<
//                 typename boost::range_iterator<boost::remove_cv_ref_t<Range>>::type,
//                 const_node_handle_t<Tree>
//             >,
//             "The given range is not recognized as a datatree node range."
//         );
//         
//         addNodeRange_impl(std::forward<Range>(range));
//         return range;
//     }
//     
//     template<typename Range,
//         std::enable_if_t<
//             !std::is_convertible_v<
//                 typename boost::range_category<boost::remove_cv_ref_t<Range>>::type, 
//                 std::forward_iterator_tag
//             >,
//             void*> = nullptr
//         >
//     inline void addNodeRange(Range&& range)
//     {
//         static_assert(
//             std::is_convertible_v<
//                 typename boost::range_iterator<boost::remove_cv_ref_t<Range>>::type,
//                 const_node_handle_t<Tree>
//             >,
//             "The given range is not recognized as a datatree node range."
//         );
//         
//         addNodeRange_impl(std::forward<Range>(range));
//     }
//     
//     inline handle_t addNode(handle_t node)
//     {
//         NodeAddress address = datatree_node_address(node);
//         std::size_t siblingSize = datatree_node_child_count(datatree_node_parent(node));
//         handle_t root = datatree_node_root(node);
//         
//         // no modification of observer state before this point. no calls to 
//         // (potentially throwing) datatree_* functions after this point.
//         
//         _event.initializeData(root);
//         
//         _event._data->childCountByParent[address.parent()] = siblingSize;
//         _event.extendPrimaryUnits({ std::move(address), 1 });
//         
//         return node;
//     }
//     
    template<typename ChildHandle, typename Range>
    inline auto addValueRange(handle_t parent, ChildHandle&& pos, Range&& values)
    {
    }
    
    template<typename ChildHandle, typename T>
    inline auto addValue(handle_t parent, ChildHandle&& pos, T&& value)
    {
    }
    
private:
//     template<typename Range>
//     void addNodeRange_impl(Range&& range)
//     {
//         if (boost::empty(range)) return;
//         
//         auto&& begin = boost::begin(range);
//         auto&& end = boost::end(range);
//         
//         handle_t root = {};
//         
//         QVarLengthArray<NodeUnit, 64> primaryUnitsBuffer;
//         QVarLengthArray<NodeUnit, 64> secondaryUnitsBuffer;
//         QVarLengthArray<std::size_t, 64> childCountBuffer;
//         
// 
//         {
//             handle_t lastParent = {};
//             std::size_t lastIndex = 0;
//             
//             for (auto i = begin; i != end; ::Addle::aux_datatree::node_sibling_increment(i) )
//             {
//                 handle_t node(i);
//                 handle_t currentRoot = ::Addle::aux_datatree::node_root(node);
//             
//                 handle_t parent = ::Addle::aux_datatree::node_parent(node);
//                 std::size_t index = ::Addle::aux_datatree::node_index(node);
//                 
//                 if (primaryUnitsBuffer.isEmpty()
//                         || parent != lastParent
//                         || index != lastIndex + 1
//                     )
//                 {
//                     primaryUnitsBuffer.append({ ::Addle::aux_datatree::node_address(node), 1 });
//                     childCountBuffer.append( ::Addle::aux_datatree::node_child_count(parent));
//                     root = currentRoot;
//                 }
//                 else
//                 {
//                     ++(primaryUnitsBuffer.last().length);
//                     _event.validateRoot(currentRoot, root);
//                 }
//                 
//                 lastParent = parent;
//                 lastIndex = index;
//             }
//             
// //         for (std::size_t i = 0; i < count; ++i)
// //         {
// //             if (i + index > childCount)
// //                 break; 
// //                 // this probably indicates an error case, but we'll let 
// //                 // datatree_node_remove decide what to do about it.
// //             
// //             handle_t child = datatree_node_child_at(parent, i + index);
// //             
// //             auto&& begin = datatree_node_dfs_begin(child);
// //             auto&& end = datatree_node_dfs_end(child);
// //             
// //             for (auto j = begin; j != end; ++j)
// //             {
// //                 handle_t descendant = j.cursor();
// //                 
// //                 std::size_t descendantChildCount = datatree_node_child_count(descendant);
// //                 if (descendantChildCount > 0)
// //                 {
// //                     secondaryUnitsBuffer.append({
// //                             mapBackward(datatree_node_address(descendant) << 0),
// //                             descendantChildCount
// //                         });
// //                 }
// //             }
// //         }
//             
//         }
// 
//         // no modification of observer state before this point. no calls to 
//         // (potentially throwing) datatree_* functions after this point.
//         
//         _event.initializeData(root);
//         
//         {
//             std::size_t i = 0;
//             for (NodeUnit& unit : primaryUnitsBuffer)
//             {
//                 _event.extendPrimaryUnits(std::move(unit));
//                 _event._data->childCountByParent[unit.address.parent()] = childCountBuffer[i];
//                 ++i;
//             }
//         }
//         //this->extendSecondaryUnits(secondaryUnitsBuffer);
//     }
    
    NodesAddedEvent<Tree> _event;
};

template<typename Tree>
class RemoveNodesObserver;

template<class Tree>
class NodesRemovedEvent : public ExtensibleNodeEventBase<Tree>
{
public:
    using handle_t = node_handle_t<Tree>;
    
    //using nostalgia_t = NostalgicVisitorImpl<RemoveNodesObserver<Tree>, false>;
    
    NodesRemovedEvent() = default;
    
//     inline NodesRemovedEvent(handle_t root)
//         : _data(new ExtensibleNodeEventBase<Tree>::Data(root))
//     {
//     }
    
    NodesRemovedEvent(const NodesRemovedEvent&) = default;
    NodesRemovedEvent(NodesRemovedEvent&&) = default;
    
    NodesRemovedEvent& operator=(const NodesRemovedEvent&) = default;
    NodesRemovedEvent& operator=(NodesRemovedEvent&&) = default;
    
    inline NodeAddress mapForward(NodeAddress address) const
    {
        return this->mapSubtractive(address);
    }
    
    inline NodeAddress mapBackward(NodeAddress address) const
    {
        return this->mapAdditive(address);
    }
    
    inline std::size_t childCount(NodeAddress address, std::size_t progress) const
    {
        return this->template childCount_impl<false>(address, progress);
    }
    
    friend class RemoveNodesObserver<Tree>;
    friend class NostalgicNodeEventVisitor<Tree>;
};

template<typename Tree>
class RemoveNodesObserver
{
public:
    using handle_t = node_handle_t<Tree>;
    
    RemoveNodesObserver() = default;
//     inline RemoveNodesObserver(handle_t root)
//         : _event(root)
//     {
//     }
    
    RemoveNodesObserver(const RemoveNodesObserver&) = delete;
    RemoveNodesObserver(RemoveNodesObserver&&) = delete;
    RemoveNodesObserver& operator=(const RemoveNodesObserver&) = delete;
    RemoveNodesObserver& operator=(RemoveNodesObserver&&) = delete;
    
    inline const NodesRemovedEvent<Tree>& event() const & { return _event; }
    inline NodesRemovedEvent<Tree>&& event() && { return std::move(_event); }
    
    inline operator const NodesRemovedEvent<Tree>& () const & { return _event; }
    inline operator NodesRemovedEvent<Tree>&& () && { return std::move(_event); }
        
    void remove(handle_t parent, std::size_t index, std::size_t count = 1)
    {
        assert(parent);
        if (count == 0)
            return;
        
        std::size_t childCount = aux_datatree::node_child_count(parent);
        
//         ChildNodeRange<Tree> children(
//                 aux_datatree::node_child_at(parent, index),
//                 (index + count >= childCount) ?
//                     aux_datatree::node_child_at(parent, inde 
//             );
        
//         remove_impl(parent, children, index, count);
    }
    
    void remove(ChildNodeRange<Tree> children)
    {
        if (children.empty())
            return;
        
        handle_t parent = aux_datatree::node_parent(children.begin());
        std::size_t index = aux_datatree::node_index(children.begin());
        std::size_t count = boost::size(children);
        
        auto&& begin = children.begin();
        auto&& end = children.end();
        for (auto i = begin; i != end; ++i)
        {
            assert(aux_datatree::node_parent(i) == parent);
        }
        
        remove_impl(parent, children, index, count);
    }
    
private:
    // TODO: adapt to work with any node range?
    void remove_impl(handle_t parent, ChildNodeRange<Tree> children, std::size_t index, std::size_t count)
    {
        handle_t root = aux_datatree::node_root(parent);
        
        QVarLengthArray<NodeUnit, 64> secondaryUnitsBuffer;
        
        auto&& begin = children.begin();
        auto&& end = children.end();
        
        // record secondary removals
        for (handle_t child = begin; child != end; aux_datatree::node_sibling_increment(child))
        {
            auto&& desc_begin = aux_datatree::node_dfs_begin(child);
            auto&& desc_end = aux_datatree::node_dfs_end(child);
            
            for (
                auto descendant = desc_begin; 
                descendant != desc_end; 
                aux_datatree::node_dfs_increment(descendant)
            )
            {
                std::size_t descendantChildCount = aux_datatree::node_child_count(descendant);
                if (descendantChildCount > 0)
                {
                    secondaryUnitsBuffer.append({
                            _event.mapBackward(aux_datatree::node_address(descendant) << 0),
                            descendantChildCount
                        });
                }
            }
        }
        
        std::size_t originalChildCount = aux_datatree::node_child_count(parent);
        
        ::Addle::aux_datatree::node_remove_children(parent, begin, end);
        
        NodeAddress parentAddress = datatree_node_address(parent);

        // no modification of observer state before this point. no calls to 
        // (potentially throwing) datatree_* functions after this point.
        
        _event.initializeData(root);
        
        _event._data->childCountByParent[parentAddress] = originalChildCount;
        _event.extendPrimaryUnits({ std::move(parentAddress) << index, count });
        _event.extendSecondaryUnits(secondaryUnitsBuffer);
    }
    
    NodesRemovedEvent<Tree> _event;
};

// A utility for processing node events, e.g., to synchronize the structure of
// two data trees. When given a node event, the visitor becomes "nostalgic" and 
// reverse-constructs structural information about the tree as it was before the
// change(s). The visitor exposes a `unit` describing the starting location (in 
// terms of its present nostalgic state) and length of a block of nodes that
// have been added or removed from the tree. The `advance` function will "step
// forward", now emulating the state of the tree as if the previous unit has
// just been added or removed, and exposing the next unit. This will walk
// through a sequence of contiguous changes to reproduce the complex change as
// described in the node event until the visitor is no longer nostalgic.
//
// WARNING Don't give this multiple events yet
template<class Tree>
class NostalgicNodeEventVisitor
{
    using added_event_t = NodesAddedEvent<Tree>;
    using removed_event_t = NodesRemovedEvent<Tree>;
    using event_variant_t = std::variant<added_event_t, removed_event_t>;
    
    using handle_t = node_handle_t<Tree>;
        
public:
    NostalgicNodeEventVisitor() = default;
        
    inline void clear() 
    {
        _events.clear(); 
        _currentEventProgress = 0; 
    }
    
    template<typename Event>
    inline void pushEvent(Event&& e)
    { 
        _events.push_back(std::forward<Event>(e));
    }
    
    inline bool isNostalgic() const 
    { 
        return !_events.empty();
    }
    
    inline NodeUnit unit() const
    {
        if (_events.empty())
            return NodeUnit();
        
        return std::visit( 
                eventVisitor_unit { _currentEventProgress }, 
                _events.front() 
            );
    }
    
    inline NodeAddress unitAddress() const { return unit().address; }
    inline std::size_t unitLength() const { return unit().length; }
    
    inline std::size_t eventsCount() const { return _events.count(); }
    
    inline bool currentIsAddedEvent() const
    { 
        return !_events.empty() && std::holds_alternative<added_event_t>(_events.front());
    }
    inline added_event_t currentAddedEvent() const
    {
        if (_events.empty())
            return added_event_t();
        else
            return std::get<added_event_t>(_events.front());
    }
    
    inline bool currentIsRemovedEvent() const
    { 
        return !_events.empty() && std::holds_alternative<removed_event_t>(_events.front());
    }
    inline removed_event_t currentRemovedEvent() const
    {
        if (_events.empty())
            return removed_event_t();
        else
            return std::get<removed_event_t>(_events.front());
    }
    
    inline std::size_t eventProgress() const { return _currentEventProgress; }
    
    inline NodeAddress mapForward(NodeAddress address) const
    { 
        if (_events.empty())
            return address;
        
        auto v = eventVisitor_mapForward { address };
        
        for (auto& var : _events)
            std::visit(v, var);
        
        return v.address;
    }
    
    inline NodeAddress mapBackward(NodeAddress address) const 
    { 
        if (_events.empty())
            return address;
        
        auto v = eventVisitor_mapForward { address };
        
        for (auto& var : _events | boost::adaptors::reversed)
            std::visit(v, var);
        
        return v.address;
    }
    
    inline std::size_t childCount(NodeAddress address) const 
    { 
        if (_events.empty())
        {
            return 0;
        }
        else
        {
            return std::visit( 
                    eventVisitor_childCount { address,  _currentEventProgress }, 
                    _events.front() 
                );
        }
    }
    
    inline void advance()
    { 
        if (_events.empty())
            return;
        
        ++_currentEventProgress;
        
        if (_currentEventProgress >= std::visit( eventVisitor_unitCount {}, _events.front() ))
        {
            _currentEventProgress = 0;
            _events.pop_front();
        }
    }
//     inline void regress() 
//     { 
//         if (isNostalgic() && _progress > 0) --_progress; 
//         
//     }
    
private:
    struct eventVisitor_unit
    {
        std::size_t progress;
        
        inline NodeUnit operator() (const added_event_t& added) const
        {
            return added.primaryUnits()[progress];
        }
        
        inline NodeUnit operator() (const removed_event_t& added) const
        {
            return added.primaryUnits()[
                    added._data->primaryUnitsCount - progress - 1
                ];
        }
    };

    struct eventVisitor_mapForward
    {
        NodeAddress address;
        
        template<class Event>
        inline void operator() (const Event& e)
        {
            address = e.mapForward(address);
        }
    };
    
    struct eventVisitor_mapBackward
    {
        NodeAddress address;
        
        template<class Event>
        inline void operator() (const Event& e)
        {
            address = e.mapBackward(address);
        }
    };
    
    struct eventVisitor_childCount
    {
        NodeAddress address;
        std::size_t progress;
        
        template<class Event>
        inline std::size_t operator() (const Event& e) const
        {
            return e.childCount(address, progress);
        }
    };
    
    struct eventVisitor_unitCount
    {
        template<class Event>
        inline std::size_t operator() (const Event& e) const
        {
            return e._data->primaryUnitsCount;
        }
    };
    
    std::deque<event_variant_t> _events;
    std::size_t _currentEventProgress = 0;
};

// TODO: Some work is needed before this truly supports asynchronous usage.
//
// I think this design will allow multiple observers to be written on the same 
// data tree, then visited in the same order and the behavior is equivalent to 
// if one observer had been used. 
//
// However, there is currently no mechanism to verify that a tree's current
// state is equivalent to the end state of any observer. Nor (relatedly) is 
// there a mechanism to verify that observers are received in the correct order.
// 
// It is conceivable that there is a way to represent incremental versions of 
// the structure of a data tree with an efficient hash scheme. Such a hash could 
// be used as a signature of the start and end states of observers, and of a 
// whole data tree (e.g., DataTree would implicitly keep such a hash, other data
// tree classes could be wrapped in a manager object [which would likely need to
// perform other synchronization functions as well]). Such a hash scheme should 
// be cautious to focus on the structure as-is and not on the changes since many 
// different combinations of changes can result in equivalent trees.
// 
// In the event that observers are recieved in the wrong order, and assuming 
// the order cannot be corrected, it may still not be desirable to "throw away 
// everything and rebuild", since that can destroy some implicit non-trivial 
// state (e.g., the scroll position in a QItemView). In such a case, it may be 
// possible to construct an observer via a "diff" algorithm (i.e., longest 
// common subsequence) to emulate the sequence of changes required to produce 
// the new state from the old state while preserving intermediate identities. It 
// should however be noted that doing this is *not strictly cheap*.

} // namespace aux_datatree

template<typename Tree>
using DataTreeNodesAddedEvent = aux_datatree::NodesAddedEvent<Tree>;

template<typename Tree>
using DataTreeAddNodesObserver = aux_datatree::AddNodesObserver<Tree>;

template<typename Tree>
using DataTreeNodesRemovedEvent = aux_datatree::NodesRemovedEvent<Tree>;

template<typename Tree>
using DataTreeRemoveNodesObserver = aux_datatree::RemoveNodesObserver<Tree>;

template<typename Tree>
using DataTreeNostalgicNodeEventVisitor = aux_datatree::NostalgicNodeEventVisitor<Tree>;

} // namespace Addle
