#pragma once

#include <optional>
#include <map>

#include <boost/range.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <QList>
#include <QPainter>
#include <QPainterPath>
#include <QRegion>
#include <QSharedData>
#include <QSharedPointer>

#include "interfaces/rendering/irenderentity.hpp"

#include "utilities/datatree/aux.hpp"
#include "utilities/datatree/observer.hpp"
#include "utilities/datatree/nestedobjectadapter.hpp"
#include "utilities/qobject.hpp"

namespace Addle {
    
namespace aux_render 
{
    class RenderRoutineNodeHandle; 
    enum EntityFlag
    {
        EntityFlag_None = 0x00,
        
        EntityFlag_Draws = 0x01
        
//         EntityFlag_ProvidesPMask = 0x02,
//         EntityFlag_ProvidesRMask = 0x04,
        
//         EntityFlag_StrictlySourceOver = 0x20,
//         
//         EntityFlag_RequiresARGB32Buffer = 0x100

//         EntityFlag_RequiresARGB16FBuffer

//         EntityFlag_FastAndInaccurate
//         EntityFlag_SlowAndAccurate

//         EntityFlag_RegisterForCacheAccess
    };
    
    Q_DECLARE_FLAGS(EntityFlags, EntityFlag);
}

/**
 * Declaratively gives rendering instructions. 
 * 
 * A RenderRoutine is made up of a set of components, with each component having 
 * a unique id and z-value. A component can either be an entity, with a shared 
 * pointer to an object that performs some step of the rendering routine (e.g., 
 * paints onto a paint device, exposes a mask, or performs an effect on a 
 * buffer), or a component can be a subroutine, consisting of a nested 
 * RenderRoutine.
 * 
 * RenderRoutine are exposed by instances of IRenderable2 and consumed by
 * IRenderer2. A renderable may build its render routine out of the routines of 
 * subordinate renderables as desired. External alterations may be made to a
 * routine in the context of a particular renderer without affecting the
 * renderable itself or its routine in any other renderers.
 */
class RenderRoutine
{
    struct Data;
    struct component_entry_t
    {
        double z;
        QUuid id;
        
        QSharedDataPointer<Data> routineData;
        QSharedPointer<IRenderEntity> entity;
        
        aux_render::EntityFlags entityFlags;
        
        bool operator==(const component_entry_t& other) const
        {
            return id           == other.id
                && z            == other.z
                && routineData  == other.routineData
                && entity       == other.entity;
        }
        
        friend bool operator<(const component_entry_t lhs, const component_entry_t& rhs) { return lhs.z < rhs.z; }
        friend bool operator<(double lhs, const component_entry_t& rhs) { return lhs < rhs.z; }
        friend bool operator<(const component_entry_t& lhs, double rhs) { return lhs.z < rhs; }
    };
    using component_list_t = std::vector<component_entry_t>;
    
public:
    class Component
    {
    public:
        Component() = default;
        Component(const Component&) = default;
        
        double z() const { return _z; }
        QUuid id() const { return _id; }
        
        bool isSubRoutine() const { return _routineData; }
        RenderRoutine asSubRoutine() const
        {
            RenderRoutine result;
            if (Q_LIKELY(_routineData))
                result._data = const_cast<Data*>(_routineData);
            
            return result;
        }
        
        bool isEntity() const { return _entity; }
        QSharedPointer<IRenderEntity> asEntity() const 
        { 
            return _entity.toStrongRef(); 
        }
        aux_render::EntityFlags entityFlags() const
        {
            return _entityFlags;
        }
        
    private:
        Component(const component_entry_t& entry)
            : _routineData(entry.routineData.data()), 
            _entity(entry.entity),
            _z(entry.z), 
            _id(entry.id),
            _entityFlags(entry.entityFlags)
        {
            assert(_routineData || _entity);
        }
        
        Component(const Data* routineData, QWeakPointer<IRenderEntity> entity)
            : _routineData(routineData), _entity(entity)
        {
            auto s_entity = _entity.toStrongRef();
            
            if (_routineData)
                _id = _routineData->id;
            else if (s_entity)
                _id = s_entity->id();
            else
                Q_UNREACHABLE();
        }
        
        const Data* _routineData = nullptr;
        QWeakPointer<IRenderEntity> _entity;
        
        double _z = qQNaN();
        QUuid _id;
        aux_render::EntityFlags _entityFlags;
        
        friend class RenderRoutine;
    };
    
    using component_range_t = aux_datatree::ConstChildNodeRange<RenderRoutine>;
    using dfs_range_t = aux_datatree::ConstNodeRange<RenderRoutine>;
    
    RenderRoutine() = default;
    RenderRoutine(const RenderRoutine&) = default;
    RenderRoutine(RenderRoutine&&) = default;
    
    RenderRoutine& operator=(const RenderRoutine&) = default;
    RenderRoutine& operator=(RenderRoutine&&) = default;
    
    RenderRoutine(QString name)
        : _data(new Data(std::move(name)))
    {
    }
    
    inline bool operator==(const RenderRoutine& other) const
    {
        return (_data == other._data)
            || (
                _data && other._data
                && (
                       _data->id                == other._data->id
                    && _data->opacity           == other._data->opacity
                    && _data->compositionMode   == other._data->compositionMode
                    && _data->passThroughMode   == other._data->passThroughMode
                    && _data->background        == other._data->background
                    
                    && _data->components        == other._data->components
                )
            );
    }
    
    inline bool operator!=(const RenderRoutine& other) const
    {
        return !(*this == other);
    }
    
    inline bool isNull() const { return !_data; }
    inline bool isEmpty() const { return !_data || _data->components.empty(); }
    
    RenderRoutine& addEntity(
            QSharedPointer<IRenderEntity> entity, 
            aux_render::EntityFlags flags,
            double z = 0
        )
    {
        assert(entity);
        initData();
        
        _data->components.insert(
                std::upper_bound(_data->components.begin(), _data->components.end(), z),
                {
                    z,
                    entity->id(),
                    QSharedDataPointer<Data> {},
                    entity,
                    flags
                }
            );
        
        return *this;
    }
    
    RenderRoutine& addSubRoutine(
            RenderRoutine subRoutine,
            double z = 0
        )
    {
        assert(subRoutine._data);
        initData();
        
        _data->components.insert(
                std::upper_bound(_data->components.begin(), _data->components.end(), z),
                {
                    z,
                    subRoutine.id(),
                    subRoutine._data,
                    QSharedPointer<IRenderEntity> {}
                }
            );
        
        return *this;
    }

    inline RenderRoutine& setId(QUuid id)
    {
        initData();
        _data->id = id;
        return *this;
    }
    
    inline QUuid id() const { return _data ? _data->id : QUuid(); }
    
    inline component_range_t components(double min = -Q_INFINITY, double max = Q_INFINITY) const;
    
    inline dfs_range_t depthFirstSearch() const;
    
    inline RenderRoutine& setOpacity(double opacity)
    {
        assert(!qIsNaN(opacity));
        initData();
        
        _data->opacity = qBound(0.0, opacity, 1.0);
        return *this;
    }
    
    inline double opacity() const { return _data ? _data->opacity : 1.0; }
    
    inline RenderRoutine& setCompositionMode(QPainter::CompositionMode compositionMode)
    {
        initData();
        
        _data->compositionMode = compositionMode;
        return *this;
    }
    
    inline QPainter::CompositionMode compositionMode() const
    {
        return _data ? _data->compositionMode : QPainter::CompositionMode_SourceOver;
    }
    
    inline RenderRoutine& setPassThroughMode(bool passThroughMode)
    {
        initData();
        
        _data->passThroughMode = passThroughMode;
        return *this;
    }
    
    inline bool passThroughMode() const { return _data ? _data->passThroughMode : false; }
        
    inline RenderRoutine& setBackground(QColor background)
    {
        initData();
        
        _data->background = std::move(background);
        return *this;
    }
    
    inline QColor background() const { return _data ? _data->background : QColor(); }
    
private:
    struct Data : QSharedData
    {
        Data() = default;
        Data(const Data&)   = default;
        Data(Data&&)        = delete;
        
        Data(QUuid id_) : id(id_) {}
        
        QUuid id;
        
        component_list_t components;
        
        double opacity = 1.0;
        QPainter::CompositionMode compositionMode = QPainter::CompositionMode_SourceOver;
        bool passThroughMode = false;
        QColor background;
    };
    
    inline void initData()
    {
        if (!_data) 
            _data = new Data;
        else 
            _data.detach();
    }
    
    // private Component constructor access for friends
    static Component makeComponent(const component_entry_t& entry) { return Component(entry); }
    static Component makeComponent(const Data* routineData, QWeakPointer<IRenderEntity> entity) { return Component(routineData, entity); }
    
    QSharedDataPointer<Data> _data;
    
    friend class aux_datatree::ChildNodeIterator<RenderRoutine, true>;
    friend class aux_datatree::ChildNodeIterator<RenderRoutine, false>;
        // Should we even allow non-const iterators?
    
    friend class aux_render::RenderRoutineNodeHandle;
};

template<>
struct aux_datatree::datatree_traits<RenderRoutine>
{
    using node_handle = aux_render::RenderRoutineNodeHandle;
    using const_node_handle = aux_render::RenderRoutineNodeHandle;
};

template<bool IsConst>
class aux_datatree::ChildNodeIterator<RenderRoutine, IsConst>
    : public boost::iterator_adaptor<
        ChildNodeIterator<RenderRoutine, IsConst>,
        RenderRoutine::component_list_t::const_iterator,
        RenderRoutine::Component,
        boost::use_default,
        RenderRoutine::Component
    >
{
    using adaptor_t = boost::iterator_adaptor<
            ChildNodeIterator<RenderRoutine, IsConst>,
            RenderRoutine::component_list_t::const_iterator,
            RenderRoutine::Component,
            boost::use_default,
            RenderRoutine::Component
        >;
    
public:
    ChildNodeIterator() = default;
    ChildNodeIterator(const ChildNodeIterator&) = default;
    ChildNodeIterator& operator=(const ChildNodeIterator&) = default;
    
    ChildNodeIterator(RenderRoutine::component_list_t::const_iterator base)
        : adaptor_t(base)
    {
    }
    
    inline aux_render::RenderRoutineNodeHandle cursor() const;
    inline operator aux_render::RenderRoutineNodeHandle () const; 
    
private:
    RenderRoutine::Component dereference() const
    {
        return RenderRoutine::makeComponent(*this->base());
    }
    
    friend class boost::iterator_core_access;
};

namespace aux_render {
    
class RenderRoutineNodeHandle
{
public:
    RenderRoutineNodeHandle() = default;
    RenderRoutineNodeHandle(const RenderRoutineNodeHandle&) = default;
    
    RenderRoutineNodeHandle& operator=(const RenderRoutineNodeHandle&) = default;
    
    RenderRoutineNodeHandle(const RenderRoutine& routine)
        : _routineData(routine._data.data())
    {
    }
    
    RenderRoutineNodeHandle(const RenderRoutine::component_entry_t& entry)
        : _routineData(entry.routineData.data()), _entity(entry.entity)
    {
    }
    
    explicit operator bool () const { return _routineData || _entity; }
    bool operator! () const { return !_routineData && !_entity; }
    
    bool operator==(const RenderRoutineNodeHandle& other) const 
    { 
        return _routineData == other._routineData 
            && _entity == other._entity;
    }
    
    bool operator!=(const RenderRoutineNodeHandle& other) const 
    { 
        return _routineData != other._routineData 
            || _entity != other._entity;
    }
    
    RenderRoutine::Component operator*() const
    {
        assert(*this);
        return RenderRoutine::makeComponent(_routineData, _entity);
    }
    
private:
    const RenderRoutine::Data* _routineData = nullptr;
    QWeakPointer<IRenderEntity> _entity;
    
    aux_datatree::ChildNodeIterator<RenderRoutine, true> children_begin_impl() const
    {
        if (!_routineData) 
            return {};
        else
            return _routineData->components.begin();
    }
    
    friend aux_datatree::ChildNodeIterator<RenderRoutine, true> datatree_node_children_begin(const RenderRoutineNodeHandle& handle)
    {
        if (Q_UNLIKELY(!handle)) 
            return {};
        else
            return handle.children_begin_impl();
    }
        
    aux_datatree::ChildNodeIterator<RenderRoutine, true> children_end_impl() const
    {
        if (!_routineData) 
            return {};
        else
            return _routineData->components.end();
    }
    
    friend aux_datatree::ChildNodeIterator<RenderRoutine, true> datatree_node_children_end(const RenderRoutineNodeHandle& handle)
    {
        if (Q_UNLIKELY(!handle)) 
            return {};
        else
            return handle.children_end_impl();
    }
};
    
} // namespace aux_render

inline RenderRoutine::component_range_t RenderRoutine::components(double min, double max) const
{
    if (Q_UNLIKELY(!_data)) return component_range_t();
    
    auto begin = std::lower_bound(
            _data->components.begin(),
            _data->components.end(),
            min
        );
    
    auto end = std::upper_bound(
            _data->components.begin(),
            _data->components.end(),
            max
        );
    
    return component_range_t(begin, end);
}

inline RenderRoutine::dfs_range_t RenderRoutine::depthFirstSearch() const
{
    using dfs_node_t = aux_datatree::DFSExtendedNode<aux_render::RenderRoutineNodeHandle, true>;
    if (Q_UNLIKELY(!_data)) return dfs_range_t();
    
    return dfs_range_t(
            dfs_node_t(aux_render::RenderRoutineNodeHandle(*this)),
            dfs_node_t()
        );
}

template<bool IsConst>
inline aux_render::RenderRoutineNodeHandle aux_datatree::ChildNodeIterator<RenderRoutine, IsConst>::cursor() const
{
    return aux_render::RenderRoutineNodeHandle(*this->base());
}

template<bool IsConst>
inline aux_datatree::ChildNodeIterator<RenderRoutine, IsConst>::operator aux_render::RenderRoutineNodeHandle () const
{
    return cursor();
}

inline aux_render::RenderRoutineNodeHandle datatree_root(const RenderRoutine& routine)
{
    return aux_render::RenderRoutineNodeHandle(routine);
}

/**
 * Identifies changes that have been made to a render routine and/or its 
 * descendant subroutines.
 */
class RenderRoutineChangedEvent
{    
    using entity_flags_list_t = QList<std::pair<std::size_t, aux_render::EntityFlags>>;
public:
    using sub_routine_map_t = QMap<DataTreeNodeAddress, RenderRoutineChangedEvent>;
    
    RenderRoutineChangedEvent()
        : _opacityChanged(false),
        _compositionModeChanged(false),
        _passThroughModeChanged(false),
        _backgroundChanged(false)
    {
    }
    
    RenderRoutineChangedEvent(const RenderRoutineChangedEvent&) = default;
    RenderRoutineChangedEvent(RenderRoutineChangedEvent&&) = default;
        
    RenderRoutineChangedEvent& operator=(const RenderRoutineChangedEvent&) = default;
    RenderRoutineChangedEvent& operator=(RenderRoutineChangedEvent&&) = default;
    
    // NOTE: all addresses in the RenderRoutineChangedEvent should be in terms
    // of the end state of the node event
    DataTreeNodeEvent nodeEvent() const { return _nodeEvent; }
    RenderRoutineChangedEvent& setNodeEvent(const DataTreeNodeEvent& event) 
    {
        _nodeEvent = event; 
        return *this; 
    }
    
    RenderRoutineChangedEvent& addNodeChunk(DataTreeNodeChunk chunk)
    {
        _nodeEvent.addChunk(chunk);
        return *this;
    }
    
    RenderRoutineChangedEvent& addNodeChunks(QList<DataTreeNodeChunk> chunks)
    {
        _nodeEvent.addChunks(chunks);
        return *this;
    }
    
    RenderRoutineChangedEvent& removeNodeChunk(DataTreeNodeChunk chunk)
    {
        _nodeEvent.removeChunk(chunk);
        return *this;
    }
    
    RenderRoutineChangedEvent& removeNodeChunks(QList<DataTreeNodeChunk> chunks)
    {
        _nodeEvent.removeChunks(chunks);
        return *this;
    }
    
    // Event objects describing changes to descendant subroutines, ordered by
    // the address of that subroutine. (Subroutine events only contain
    // information about themselves and not their descendants)
    const sub_routine_map_t& subRoutineEvents() const { return _subRoutineEvents; }
    
    bool zValuesChanged(DataTreeNodeAddress parent = {}) const
    {
        if (parent.isRoot())
        {
            return !_zValues.isEmpty();
        }
        else 
        {
            auto i = _subRoutineEvents.find(parent);
            return i != _subRoutineEvents.end()
                && !((*i)._zValues.isEmpty());
        }
    }
    
    // The z-values of the components of the subroutine at parent. An empty list
    // indicates that no z-values for the components of the given subroutine 
    // were changed. 
    QList<double> zValues(DataTreeNodeAddress parent = {}) const
    {
        if (parent.isRoot())
        {
            return _zValues;
        }
        else
        {
            auto i = _subRoutineEvents.find(parent);
            if (i != _subRoutineEvents.end())
                return (*i)._zValues;
            else
                return {};
        }
    }
    
    RenderRoutineChangedEvent& setZValues(QList<double> zValues)
    {
#ifdef ADDLE_DEBUG
        if(Q_UNLIKELY(!std::is_sorted(zValues.cbegin(), zValues.cend())))
        {
            qWarning("%s", qUtf8Printable(
                //% "RenderRoutineChangedEvent given a sequence of z-values "
                //% "that were not sorted."
                qtTrId("debug-messages.renderroutine.event-zvalues-unsorted")
            ));
        }
#endif
        _zValues = zValues;
        return *this;
    }
    
    RenderRoutineChangedEvent& setZValues(DataTreeNodeAddress parent, QList<double> zValues)
    {
#ifdef ADDLE_DEBUG
        if(Q_UNLIKELY(!std::is_sorted(zValues.cbegin(), zValues.cend())))
        {
            qWarning("%s", qUtf8Printable(
                qtTrId("debug-messages.renderroutine.event-zvalues-unsorted")
            ));
        }
#endif
        if (parent.isRoot())
            _zValues = std::move(zValues);
        else
            _subRoutineEvents[parent]._zValues = std::move(zValues);
        
        return *this;
    }
    
    // Automatically adds/updates the z-values for subroutines affected by 
    // nodeEvent.
    RenderRoutineChangedEvent& populateZValues(const RenderRoutine& routine);
    
    bool entityFlagsChanged(const DataTreeNodeAddress& parent = {}) const
    {
        if (parent.isRoot())
        {
            return !_entityFlags.isEmpty();
        }
        else 
        {
            auto i = _subRoutineEvents.find(parent);
            return i != _subRoutineEvents.end() 
                && !((*i)._entityFlags.isEmpty());
        }
    }
    
    auto entityFlagsList(const DataTreeNodeAddress& parent = {}) const
    {
        using namespace boost::adaptors;
            
        entity_flags_list_t list;
            
        if (parent.isRoot())
        {
            list = _entityFlags;
        }
        else 
        {
            auto i = _subRoutineEvents.find(parent);
            if (i != _subRoutineEvents.end())
                list = (*i)._entityFlags;
        }
        
        return  
              noDetach(std::move(list)) 
            | transformed([parent] (auto&& entry) {
                    return std::make_pair(parent << entry.first, entry.second);
                });
    }
    
    aux_render::EntityFlags entityFlags(const DataTreeNodeAddress& entity) const
    {
        if (Q_UNLIKELY(entity.isRoot()))
            return {};
        
        entity_flags_list_t list;
        auto parent = entity.parent();
        
        if (parent.isRoot())
        {
            list = _entityFlags;
        }
        else
        {
            auto i = _subRoutineEvents.find(parent);
            if (i == _subRoutineEvents.end())
                return {};
            
            list = (*i)._entityFlags;
        }
        
        auto j = std::lower_bound( 
                list.cbegin(),
                list.cend(),
                entity.lastIndex(),
                [] (const auto& entry, std::size_t index) -> bool {
                    return entry.first < index; 
                }
            );
        
        if (j != list.cend() && (*j).first == entity.lastIndex())
            return (*j).second;
        else
            return {};
    }
    
    RenderRoutineChangedEvent& setEntityFlags(
            const DataTreeNodeAddress& entity, 
            aux_render::EntityFlags flags
        )
    {
        auto parent = entity.parent();

        entity_flags_list_t& list = parent.isRoot() ? 
            _entityFlags : _subRoutineEvents[parent]._entityFlags;
                
        auto i = std::lower_bound(
                list.begin(),
                list.end(),
                entity.lastIndex(),
                [] (const auto& entry, std::size_t index) -> bool {
                    return entry.first < index; 
                }
            );
        
        if (i != list.cend() && (*i).first == entity.lastIndex())
            (*i).second = flags;
        else
            list.insert(i, { entity.lastIndex(), flags });
        
        return *this;
    }
    
    bool opacityChanged(const DataTreeNodeAddress& subRoutine = {}) const
    {
        if (subRoutine.isRoot()) 
        {
            return _opacityChanged;
        }
        else
        {
            auto i = _subRoutineEvents.find(subRoutine);
            return i != _subRoutineEvents.end() && (*i)._opacityChanged;
        }
    }
    
    std::optional<double> opacity(const DataTreeNodeAddress& subRoutine = {}) const
    {
        if (subRoutine.isRoot() && _opacityChanged)
        {
            return _opacityChanged;
        }
        else
        {
            auto i = _subRoutineEvents.find(subRoutine);
            if (i != _subRoutineEvents.end() && (*i)._opacityChanged)
                return (*i)._opacity;
        }
        
        return {};
    }
    
    RenderRoutineChangedEvent& setOpacity(double opacity)
    {
        _opacity = opacity;
        _opacityChanged = true;
        return *this;
    }
    
    RenderRoutineChangedEvent& setOpacity(const DataTreeNodeAddress& address, double opacity)
    {
        if (address.isRoot())
        {
            _opacity = opacity;
            _opacityChanged = true;
        }
        else
        {
            auto& subRoutine = _subRoutineEvents[address];
            subRoutine._opacity = opacity;
            subRoutine._opacityChanged = true;
        }
        
        return *this;
    }
    
    bool compositionModeChanged(const DataTreeNodeAddress& address = {}) const
    {
        if (address.isRoot()) 
        {
            return _compositionModeChanged;
        }
        else
        {
            auto i = _subRoutineEvents.find(address);
            return i != _subRoutineEvents.end() && (*i)._compositionModeChanged;
        }
    }
    
    std::optional<QPainter::CompositionMode> compositionMode(const DataTreeNodeAddress& address = {}) const
    {
        if (address.isRoot() && _compositionModeChanged)
        {
            return _compositionMode;
        }
        else
        {
            auto i = _subRoutineEvents.find(address);
            if (i != _subRoutineEvents.end() && (*i)._compositionModeChanged)
                return (*i)._compositionMode;
        }
        
        return {};
    }
    
    RenderRoutineChangedEvent& setCompositionMode(QPainter::CompositionMode compositionMode)
    {
        _compositionMode = compositionMode;
        _compositionModeChanged = true;
        return *this;
    }
    
    RenderRoutineChangedEvent& setCompositionMode(const DataTreeNodeAddress& address, QPainter::CompositionMode compositionMode)
    {
        if (address.isRoot())
        {
            _compositionMode = compositionMode;
            _compositionModeChanged = true;
        }
        else
        {
            auto& subRoutine = _subRoutineEvents[address];
            subRoutine._compositionMode = compositionMode;
            subRoutine._compositionModeChanged = true;
        }
        
        return *this;
    }
    
    bool passThroughModeChanged(const DataTreeNodeAddress& address = {}) const
    {
        if (address.isRoot()) 
        {
            return _passThroughModeChanged;
        }
        else
        {
            auto i = _subRoutineEvents.find(address);
            return i != _subRoutineEvents.end() && (*i)._passThroughModeChanged;
        }
    }
    
    std::optional<bool> passThroughMode(const DataTreeNodeAddress& address = {}) const
    { 
        if (address.isRoot() && _passThroughModeChanged)
        {
            return _passThroughMode;
        }
        else
        {
            auto i = _subRoutineEvents.find(address);
            if (i != _subRoutineEvents.end() && (*i)._passThroughModeChanged)
                return (*i)._passThroughMode;
        }
        
        return {};
    }
    
    RenderRoutineChangedEvent& setPassThroughMode(bool passThroughMode)
    {
        _passThroughMode = passThroughMode;
        _passThroughModeChanged = true;
        return *this;
    }
    
    RenderRoutineChangedEvent& setPassThroughMode(const DataTreeNodeAddress& address, bool passThroughMode)
    {
        if (address.isRoot())
        {
            _passThroughMode = passThroughMode;
            _passThroughModeChanged = true;
        }
        else
        {
            auto& subRoutine = _subRoutineEvents[address];
            subRoutine._passThroughMode = passThroughMode;
            subRoutine._passThroughModeChanged = true;
        }
        
        return *this;
    }
    
    bool backgroundChanged(const DataTreeNodeAddress& address = {}) const
    {
        if (address.isRoot()) 
        {
            return _backgroundChanged;
        }
        else
        {
            auto i = _subRoutineEvents.find(address);
            return i != _subRoutineEvents.end() && (*i)._backgroundChanged;
        }
    }
    
    std::optional<QColor> background(const DataTreeNodeAddress& address = {}) const 
    {
        if (address.isRoot() && _backgroundChanged)
        {
            return _background;
        }
        else
        {
            auto i = _subRoutineEvents.find(address);
            if (i != _subRoutineEvents.end() && (*i)._backgroundChanged)
                return (*i)._background;
        }
        
        return {};
    }
    
    RenderRoutineChangedEvent& setBackground(QColor background)
    {
        _background = std::move(background);
        _backgroundChanged = true;
        return *this;
    }
    
    RenderRoutineChangedEvent& setBackground(const DataTreeNodeAddress& address, QColor background)
    {
        if (address.isRoot())
        {
            _background = std::move(background);
            _backgroundChanged = true;
        }
        else
        {
            auto& subRoutine = _subRoutineEvents[address];
            subRoutine._background = std::move(background);
            subRoutine._backgroundChanged = true;
        }
        
        return *this;
    }
    
    RenderRoutineChangedEvent& moveRoot(DataTreeNodeAddress rel);
    
private:
    DataTreeNodeEvent   _nodeEvent;
    QList<double>       _zValues;
    entity_flags_list_t _entityFlags;
    
    double  _opacity;
    QPainter::CompositionMode _compositionMode;
    bool    _passThroughMode;
    QColor  _background;
    
    sub_routine_map_t _subRoutineEvents;
    
    bool _opacityChanged         : 1;
    bool _compositionModeChanged : 1;
    bool _passThroughModeChanged : 1;
    bool _backgroundChanged      : 1;
};

} // namespace Addle

Q_DECLARE_OPERATORS_FOR_FLAGS(Addle::aux_render::EntityFlags);
Q_DECLARE_METATYPE(Addle::RenderRoutineChangedEvent);
