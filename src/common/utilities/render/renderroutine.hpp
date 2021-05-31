#pragma once

#include <optional>

#include <boost/range.hpp>

#include <QList>
#include <QPainter>
#include <QPainterPath>
#include <QRegion>
#include <QSharedData>
#include <QSharedPointer>

#include "interfaces/rendering/irenderable.hpp"
#include "utilities/datatree/aux.hpp"
// #include "utilities/datatree/observers.hpp"
#include "utilities/qobject.hpp"

namespace Addle {
    
namespace aux_render { class RenderRoutineNodeHandle; }
    
class RenderHandle;

/**
 * A pointer to a function that implements the actual rendering of a component.
 * A renderable implementation may provide one or more render functions as part
 * of its render routine.
 * 
 * The first argument gives a `RenderHandle`, which contains the QPainter
 * (i.e., drawing API) and other data.
 * 
 * The second argument, "context" gives a `void*` which can, e.g., be
 * reinterpret_cast to/from an object pointer type specific to the
 * implementation and hidden elsewhere.
 * 
 * The function should be reentrant, and cannot be assumed to run on any 
 * particular thread. It should be very fast, and should not lock the thread it 
 * runs on. Renderable objects' state should be atomic from the perspective of 
 * the render function. 
 * 
 * If state is invalid when the function is run, it should do nothing (at most, 
 * print a warning to the console in debug builds), but preferably that kind of 
 * error detection should be done in response to state change events.
 */
using RenderFunctionPtr = void (*)(const RenderHandle&, const void*) noexcept;

struct RenderFunctionBinding
{
    RenderFunctionPtr function = nullptr;
    const void* context = nullptr;
    
    inline bool operator==(const RenderFunctionBinding& other) const
    {
        return function == other.function && context == other.context;
    }
    
    inline bool operator!=(const RenderFunctionBinding& other) const
    {
        return !(*this == other);
    }
    
    inline void operator()(const RenderHandle& handle) const noexcept
    {
        (*function)(handle, context);
    }
};

class RenderRoutine
{
public:
    using sub_routine_range = boost::iterator_range<typename QMap<double, RenderRoutine>::const_key_value_iterator>;
    using function_range = boost::iterator_range<typename QMap<double, RenderFunctionBinding>::const_key_value_iterator>;
    
    RenderRoutine() = default;
    RenderRoutine(const RenderRoutine&) = default;
    RenderRoutine(RenderRoutine&&) = default;
    
    RenderRoutine& operator=(const RenderRoutine&) = default;
    RenderRoutine& operator=(RenderRoutine&&) = default;
    
    inline bool operator==(const RenderRoutine& other) const
    {
        return (_data == other._data)
            || (
                _data && other._data
                && (
                    _data->opacity == other._data->opacity
                    && _data->compositionMode == other._data->compositionMode
                    && _data->passThroughMode == other._data->passThroughMode
                    && _data->background == other._data->background
                    
                    && _data->pMasks == other._data->pMasks
                    && _data->rMasks == other._data->rMasks
                    && _data->functions == other._data->functions
                    && _data->subRoutines == other._data->subRoutines
                )
            );
    }
    
    inline bool operator!=(const RenderRoutine& other) const
    {
        return !(*this == other);
    }
    
    inline RenderRoutine& addMask(const QPainterPath& mask, double z = 0)
    {
        assert(!qIsNaN(z));
        
        if (!_data)
            _data = new Data;
        
        _data->pMasks.insert(z, mask);
        return *this;
    }
    
    inline RenderRoutine& addMask(const QRegion& mask, double z = 0)
    {
        assert(!qIsNaN(z));
        
        if (!_data)
            _data = new Data;
        
        _data->rMasks.insert(z, mask);
        return *this;
    }
    
    inline std::size_t pMasksCount(double min = -Q_INFINITY, double max = Q_INFINITY) const
    {
        assert(!qIsNaN(min) && !qIsNaN(max));
        assert(min <= max);
        
        if (!_data)
            return 0;
        
        if (!_data)
            return 0;
        
        auto lower = _data->pMasks.lowerBound(min);
        auto upper = _data->pMasks.upperBound(max);
        
        if (lower == _data->pMasks.begin() && upper == _data->pMasks.end())
            return _data->pMasks.count();
        else
            return std::distance(lower, upper);
    }
    
    inline std::size_t rMasksCount(double min = -Q_INFINITY, double max = Q_INFINITY) const
    {
        assert(!qIsNaN(min) && !qIsNaN(max));
        assert(min <= max);
        
        if (!_data)
            return 0;
        
        if (!_data)
            return 0;
        
        auto lower = _data->rMasks.lowerBound(min);
        auto upper = _data->rMasks.upperBound(max);
        
        if (lower == _data->rMasks.begin() && upper == _data->rMasks.end())
            return _data->rMasks.count();
        else
            return std::distance(lower, upper);
    }
    
    inline std::size_t masksCount(double min = -Q_INFINITY, double max = Q_INFINITY) const
    {
        return pMasksCount(min, max) + rMasksCount(min, max);
    }
    
    inline RenderRoutine& addSubRoutine(const RenderRoutine& subRoutine, double z = 0)
    {
        assert(!qIsNaN(z));
        
        if (!_data)
            _data = new Data;
        
        _data->subRoutines.insert(z, subRoutine);
        return *this;
    }
    
    /**
     * Gives an iterator range of pairs providing access to subRoutines of this
     * routine whose z is between min and max inclusive. For each p in the range
     * p.first is a double giving the z value of that function and p.second is 
     * the routine as RenderRoutine
     * 
     * WARNING The resulting range may be invalidated by any modification to 
     * this RenderRoutine's subRoutines.
     */
    inline sub_routine_range subRoutines(double min = -Q_INFINITY, double max = Q_INFINITY) const
    {
        assert(!qIsNaN(min) && !qIsNaN(max));
        assert(min <= max);
        
        if (!_data)
            return sub_routine_range();
        else
            return sub_routine_range(_data->subRoutines.lowerBound(min), _data->subRoutines.upperBound(max));
    }
    
    inline std::size_t subRoutinesCount(double min = -Q_INFINITY, double max = Q_INFINITY) const
    {
        assert(!qIsNaN(min) && !qIsNaN(max));
        assert(min <= max);
        
        if (!_data)
            return 0;
        
        auto lower = _data->subRoutines.lowerBound(min);
        auto upper = _data->subRoutines.upperBound(max);
        
        if (lower == _data->subRoutines.begin() && upper == _data->subRoutines.end())
            return _data->subRoutines.count();
        else
            return std::distance(lower, upper);
    }
    
    inline RenderRoutine& addFunction(RenderFunctionPtr function, const void* context, double z = 0)
    {
        assert(!qIsNaN(z));
        
        if (!_data)
            _data = new Data;
        
        _data->functions.insert(z, { function, context });
        return *this;
    }
    
    /**
     * Gives an iterator range of pairs providing access to functions in this
     * routine whose z is between min and max inclusive. For each p in the range
     * p.first is a double giving the z value of that function and p.second is
     * the function given as RenderFunctionBinding
     * 
     * WARNING The resulting range may be invalidated by any modification to 
     * this RenderRoutine's functions.
     */
    inline function_range functions(double min = -Q_INFINITY, double max = Q_INFINITY) const
    {
        assert(!qIsNaN(min) && !qIsNaN(max));
        assert(min <= max);
        
        if (!_data)
            return function_range();
        else
            return function_range(_data->functions.lowerBound(min), _data->functions.upperBound(max));
    }
    
    inline std::size_t functionsCount(double min = -Q_INFINITY, double max = Q_INFINITY) const
    {
        assert(!qIsNaN(min) && !qIsNaN(max));
        assert(min <= max);
    
        if (!_data)
            return 0;
        
        auto lower = _data->functions.lowerBound(min);
        auto upper = _data->functions.upperBound(max);
        
        if (lower == _data->functions.begin() && upper == _data->functions.end())
            return _data->functions.count();
        else
            return std::distance(lower, upper);
    }
    
    inline std::size_t componentsCount(double min = -Q_INFINITY, double max = Q_INFINITY) const
    {
        return pMasksCount(min, max)
            + rMasksCount(min, max)
            + subRoutinesCount(min, max)
            + functionsCount(min, max);
    }
    
    inline RenderRoutine& setOpacity(double opacity)
    {
        if (!_data)
            _data = new Data;
        
        _data->opacity = opacity;
        return *this;
    }
    
    inline double opacity() const { return _data ? _data->opacity : 1.0; }
    
    inline RenderRoutine& setCompositionMode(QPainter::CompositionMode compositionMode)
    {
        if (!_data)
            _data = new Data;
        
        _data->compositionMode = compositionMode;
        return *this;
    }
    
    inline QPainter::CompositionMode compositionMode() const
    {
        return _data ? _data->compositionMode : QPainter::CompositionMode_SourceOver;
    }
    
    inline RenderRoutine& setPassThroughMode(bool passThroughMode)
    {
        if (!_data)
            _data = new Data;
        
        _data->passThroughMode = passThroughMode;
        return *this;
    }
    
    inline bool passThroughMode() const { return _data ? _data->passThroughMode : false; }
        
    inline RenderRoutine& setBackground(QColor background)
    {
        if (!_data)
            _data = new Data;
        
        _data->background = std::move(background);
        return *this;
    }
    
    inline QColor background() const { return _data ? _data->background : QColor(); }
    
    enum VisitorFilter
    {
        VisitorFilter_None          = 0x00,
        VisitorFilter_PMasks        = 0x01,
        VisitorFilter_RMasks        = 0x02,
        VisitorFilter_SubRoutines   = 0x04,
        VisitorFilter_Functions     = 0x08,
        
        VisitorFilter_All           = 0x0F
    };
    
    Q_DECLARE_FLAGS(VisitorFilters, VisitorFilter);
    
    /**
     * Visits the values of all masks, sub-routines, and functions in this
     * routine, in order of z between min and max inclusive, from lowest to
     * highest.
     * 
     * `visitor` must be a callable accepting two parameters: a double
     * representing the z of the value, and the value itself. P-masks are given
     * as QPainterPath, r-masks as QRegion, sub-routines as RenderRoutine, and
     * functions as RenderFunctionBinding. All values are passed by const lvalue
     * reference
     * 
     * The return value of `visitor`, if any, is ignored.
     * 
     * Values of different types with the same z value are visited in the
     * following order by type: p-masks, r-masks, sub-routines, and functions.
     * Values of the same type with the same z value are visited in the same
     * order as they were inserted.
     * 
     * If not all values are desired, then VisitorFilters may be applied in the 
     * template parameters of the call to visit. The values whose corresponding 
     * bits in the filter are set will be visited.
     * 
     * WARNING It is not safe to modify this RenderRoutine from inside `visitor`.
     */
    template<typename Visitor, typename VisitorFilters::Int Filters = VisitorFilter_All>
    inline void visit(Visitor&& visitor, double min = -Q_INFINITY, double max = Q_INFINITY) const;
    
private:
    struct Data : QSharedData
    {
        QMultiMap<double, QPainterPath> pMasks;
        QMultiMap<double, QRegion> rMasks;
        QMultiMap<double, RenderRoutine> subRoutines;
        QMultiMap<double, RenderFunctionBinding> functions;
        
        double opacity = 1.0;
        QPainter::CompositionMode compositionMode = QPainter::CompositionMode_SourceOver;
        bool passThroughMode = false;
        QColor background;
    };
    
    QSharedDataPointer<Data> _data;
};

template<typename Visitor, typename RenderRoutine::VisitorFilters::Int Filters>
inline void RenderRoutine::visit(Visitor&& visitor, double min, double max) const
{
    assert(!qIsNaN(min) && !qIsNaN(max));
    assert(min <= max);
    
    if constexpr (Filters == 0)
        return;
    
    if (!_data)
        return;
    
    double z = min;
    while (true)
    {
        if constexpr ((Filters & VisitorFilter_PMasks) != 0)
        {
            if (_data->pMasks.contains(z))
            {
                auto i = _data->pMasks.find(z);
                auto&& end = _data->pMasks.end();
                while (i != end && i.key() == z)
                {
                    visitor(z, *i);
                    ++i;
                }
            }
        }
        
        if constexpr ((Filters & VisitorFilter_RMasks) != 0)
        {
            if (_data->rMasks.contains(z))
            {
                auto i = _data->rMasks.find(z);
                auto&& end = _data->rMasks.end();
                while (i != end && i.key() == z)
                {
                    visitor(z, *i);
                    ++i;
                }
            }
        }
        
        if constexpr ((Filters & VisitorFilter_SubRoutines) != 0)
        {
            if (_data->subRoutines.contains(z))
            {
                auto i = _data->subRoutines.find(z);
                auto&& end = _data->subRoutines.end();
                while (i != end && i.key() == z)
                {
                    visitor(z, *i);
                    ++i;
                }
            }
        }
        
        if constexpr ((Filters & VisitorFilter_Functions) != 0)
        {
            if (_data->functions.contains(z))
            {
                auto i = _data->functions.find(z);
                auto&& end = _data->functions.end();
                while (i != end && i.key() == z)
                {
                    visitor(z, *i);
                    ++i;
                }
            }
        }
        
        if (z >= max)
            break;
        
        double next = max;

        if constexpr ((Filters & VisitorFilter_PMasks) != 0)
        {
            auto i = _data->pMasks.upperBound(z);
            if (i != _data->pMasks.end())
                next = qMin(next, i.key());
        }
        
        if constexpr ((Filters & VisitorFilter_RMasks) != 0)
        {
            auto i = _data->rMasks.upperBound(z);
            if (i != _data->rMasks.end())
                next = qMin(next, i.key());
        }
        
        if constexpr ((Filters & VisitorFilter_SubRoutines) != 0)
        {
            auto i = _data->subRoutines.upperBound(z);
            if (i != _data->subRoutines.end())
                next = qMin(next, i.key());
        }
        
        if constexpr ((Filters & VisitorFilter_Functions) != 0)
        {
            auto i = _data->functions.upperBound(z);
            if (i != _data->functions.end())
                next = qMin(next, i.key());
        }
        
        z = next;
    }
}

template<>
struct aux_datatree::datatree_traits<RenderRoutine>
{
    using node_handle = aux_render::RenderRoutineNodeHandle;
    using const_node_handle = aux_render::RenderRoutineNodeHandle;
};

template<bool IsConst>
class aux_datatree::ChildNodeIterator<RenderRoutine, IsConst>
    : public boost::iterator_facade<
        ChildNodeIterator<RenderRoutine, IsConst>,
        RenderRoutine,
        boost::bidirectional_traversal_tag,
        RenderRoutine
    >
{
public:
    using base_iterator = RenderRoutine::sub_routine_range::iterator;
    
    ChildNodeIterator() = default;
    ChildNodeIterator(const ChildNodeIterator&) = default;
    ChildNodeIterator& operator=(const ChildNodeIterator&) = default;
    
    inline ChildNodeIterator(QList<aux_render::RenderRoutineNodeHandle> ancestors, base_iterator base)
        : _ancestors(ancestors), _base(base)
    {
    }
    
    aux_render::RenderRoutineNodeHandle cursor() const;
    
private:
    RenderRoutine dereference() const { return (*_base).second; }
    bool equal(ChildNodeIterator& other) const { return _base == other._base; }
    
    void increment() { ++_base; }
    void decrement() { --_base; }
        
    QList<aux_render::RenderRoutineNodeHandle> _ancestors;
    base_iterator _base;
    
    friend class boost::iterator_core_access;
};

namespace aux_render {
    
class RenderRoutineNodeHandle
{
    using subroutine_iterator = RenderRoutine::sub_routine_range::iterator;
    
public:
    RenderRoutineNodeHandle() = default;
    RenderRoutineNodeHandle(const RenderRoutineNodeHandle&) = default;
    RenderRoutineNodeHandle(RenderRoutineNodeHandle&&) = default;
    
    RenderRoutineNodeHandle& operator=(const RenderRoutineNodeHandle&) = default;
    RenderRoutineNodeHandle& operator=(RenderRoutineNodeHandle&&) = default;
    
    
    RenderRoutineNodeHandle(QList<RenderRoutineNodeHandle> ancestors, RenderRoutine routine)
        : _ancestors(std::move(ancestors)), _routine(std::move(routine))
    {
    }
        
    inline bool operator==(const RenderRoutineNodeHandle& other) const
    {
        return _routine == other._routine;
    }
    
    inline bool operator!=(const RenderRoutineNodeHandle& other) const
    {
        return _routine != other._routine;
    }
    
    explicit inline operator bool () const
    {
        return _routine.has_value();
    }
    
    inline bool operator!() const
    {
        return !_routine.has_value();
    }
    
    inline RenderRoutine operator*() const
    {
        return *_routine;
    }
    
    inline double z() const
    {
        lazyInit();
        return _z;
    }
    
private:
    inline void lazyInit() const
    {
        if (qIsNaN(_z))
        {
            if (!_routine || _ancestors.isEmpty())
            {
                _index = 0;
            }
            else
            {
                std::size_t index = 0;
                
                auto&& begin = (*_ancestors.last()).subRoutines().begin();
                auto&& end = (*_ancestors.last()).subRoutines().end();
                
                for (auto i = begin; i != end; ++i)
                {
                    if ((*i).second == *_routine)
                    {
                        _iterator = i;
                        _z = (*i).first;
                        _index = index;
                        break;
                    }
                    ++index;
                }
            }
        }
    }
    
    friend RenderRoutineNodeHandle datatree_node_parent(const RenderRoutineNodeHandle& node)
    {
        if (node._ancestors.isEmpty())
            return RenderRoutineNodeHandle();
        else
            return node._ancestors.last();
    }
    
    friend std::size_t datatree_node_index(const RenderRoutineNodeHandle& node)
    {
        node.lazyInit();
        return node._index;
    }
    
    friend std::size_t datatree_node_depth(const RenderRoutineNodeHandle& node)
    {
        return node._ancestors.size();
    }
    
    friend DataTreeNodeAddress datatree_node_address(const RenderRoutineNodeHandle& node)
    {
        Q_UNUSED(node);
        Q_UNREACHABLE();
//         if (!node._address)
//         {
//             node.lazyInit();
//             if (!node._routine || node._ancestors.isEmpty())
//             {
//                 node._address = DataTreeNodeAddress();
//             }
//             else
//             {
//                 node._address = datatree_node_address(node._ancestors.last()) << node._index;
//             }
//         }
        
//         return *node._address;
    }
    
    friend RenderRoutineNodeHandle datatree_node_root(const RenderRoutineNodeHandle& node)
    {
        if (node._ancestors.isEmpty())
            return node;
        else
            return node._ancestors.first();
    }
    
    friend std::size_t datatree_node_child_count(const RenderRoutineNodeHandle& node)
    {
        return node._routine ? (*node._routine).subRoutinesCount() : 0;
    }
    
    friend aux_datatree::ChildNodeIterator<RenderRoutine> datatree_node_children_begin(const RenderRoutineNodeHandle& node)
    {
        return node._routine ? 
            aux_datatree::ChildNodeIterator<RenderRoutine>(
                    node._ancestors,
                    (*node._routine).subRoutines().begin() 
                ) : 
            aux_datatree::ChildNodeIterator<RenderRoutine>();
    }
    
    friend aux_datatree::ChildNodeIterator<RenderRoutine> datatree_node_children_end(const RenderRoutineNodeHandle& node)
    {
        return node._routine ? 
            aux_datatree::ChildNodeIterator<RenderRoutine>(
                    node._ancestors,
                    (*node._routine).subRoutines().begin() 
                ) : 
            aux_datatree::ChildNodeIterator<RenderRoutine>();
    }
    
    friend aux_datatree::NodeIterator<RenderRoutine> datatree_node_dfs_begin(const RenderRoutineNodeHandle& node)
    { 
        node.lazyInit();
        return aux_datatree::NodeIterator<RenderRoutine>(node);
    }
    
    friend aux_datatree::NodeIterator<RenderRoutine> datatree_node_dfs_end(const RenderRoutineNodeHandle& node)
    {
        node.lazyInit();
        
        if (node._ancestors.isEmpty())
        {
            return aux_datatree::NodeIterator<RenderRoutine>();
        }
        else
        {
            const auto& parent = node._ancestors.last();
            auto peerRoutines = (*parent._routine).subRoutines();
            auto i = node._iterator;
            ++i;
            
            if (i == peerRoutines.end())
            {
                return datatree_node_dfs_end(parent);
            }
            else
            {
                auto ancestors = node._ancestors;
                ancestors.removeLast();
                
                return aux_datatree::NodeIterator<RenderRoutine>({ ancestors, (*i).second });
            }
        }
    }
        
    friend RenderRoutineNodeHandle datatree_node_dfs_next(const RenderRoutineNodeHandle& node)
    { 
        node.lazyInit();
        
        if (!node._routine)
            return RenderRoutineNodeHandle();
        
        auto subRoutines = (*node._routine).subRoutines();
        
        
    }
    
    
    QList<RenderRoutineNodeHandle> _ancestors;
    std::optional<RenderRoutine> _routine;
    
    mutable double _z = qQNaN();
    mutable subroutine_iterator _iterator;
    mutable std::size_t _index;
    mutable std::optional<DataTreeNodeAddress> _address;
};
    
} // namespace aux_render

} // namespace Addle

Q_DECLARE_OPERATORS_FOR_FLAGS(Addle::RenderRoutine::VisitorFilters);
