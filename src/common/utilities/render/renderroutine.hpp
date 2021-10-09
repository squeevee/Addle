#pragma once

#include <optional>
#include <map>

#include <boost/range.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <QAtomicPointer>
#include <QList>
#include <QPainter>
#include <QPainterPath>
#include <QRegion>
#include <QSharedData>
#include <QSharedPointer>
#include <QReadWriteLock>

//#include "interfaces/rendering/irenderentity.hpp"

#include "utilities/datatree/aux.hpp"
#include "utilities/datatree/observer.hpp"
#include "utilities/datatree/nestedobjectadapter.hpp"
#include "utilities/qobject.hpp"

#include "utilities/hashfunctions.hpp" // for std::hash<QUuid> 

#ifdef ADDLE_TEST
class RenderRoutine_UTest;
#endif

namespace Addle {
    
class IRenderable;

class RenderRoutine;
namespace aux_render { class RoutineNodeHandle; }

template<>
struct aux_datatree::datatree_traits<RenderRoutine>
{
    using node_handle = aux_render::RoutineNodeHandle;
    using const_node_handle = aux_render::RoutineNodeHandle;
};

class RenderRoutineChangedEvent;
class RasterSurface2;
    
namespace aux_render 
{
    class RoutineNodeHandle; 
    
    enum EntityOptimization
    {
        EntityOptimization_None                 = 0x00,
        
        EntityOptimization_AlphaNonSubtractive  = 0x100
        // Indicates that an entity's draw_fn or fast_draw_fn will not decrease
        // the alpha of any pixels. This may allow a renderer to elide 
        // intermediate buffers.
    };
    
    Q_DECLARE_FLAGS(EntityOptimizations, EntityOptimization);
    
    struct Entity;
    
    using draw_fn_t = void (*)(const Entity*, QPainter*, const QRegion&, bool*) noexcept;
    
    struct EntityBuilder
    {
        QByteArray name;
        EntityOptimizations optimizations   = {};
        
        const QRegion* region_ptr           = nullptr;
        const RasterSurface2* surface_ptr   = nullptr;
        draw_fn_t draw_fn                   = nullptr;
//         draw_fn_t fast_draw_fn              = nullptr;
    };
    
    /**
     * A render entity is an abstract object that forms the basic unit of a
     * render routine, exposing properties used by the renderer to perform 
     * actual drawing steps.
     * 
     * Although Entity is a polymporphic class, only the destructor is a virtual
     * function. To reduce overhead during render operations, most of an 
     * entity's properties and functionality are exposed as pointers/function 
     * pointers. See EntityBindingBuilder for a way to initialize an 
     * Entity with these pointers bound to members of a derived class.
     * 
     * Instances of Entity are managed by QSharedPointer (and may outlive the
     * renderable that created them). Instances must also be thread safe when
     * synchronized with `lock`. All non-null property pointers must be safe to
     * dereference from a thread that has acquired `lock.lockForRead()`, i.e.,
     * their values must not change while that lock is acuqired. All non-null 
     * function pointers must be safe to call from any thread, and must not
     * block the calling thread during concurrent read operations.
     */
    struct Entity
    {
        Entity() = default;
        explicit Entity(QByteArray name_) noexcept : name(name_) {}
        
        Entity(const EntityBuilder& builder)
            : name(builder.name),
            optimizations(builder.optimizations),
            region_ptr(builder.region_ptr),
            surface_ptr(builder.surface_ptr),
            draw_fn(builder.draw_fn)
        {
        }
        
        Entity(const Entity&)           = delete;
        Entity(Entity&&)                = delete;
        
        virtual ~Entity() noexcept
        {
#ifdef ADDLE_DEBUG
            assert(lock.tryLockForWrite());
            lock.unlock();
#endif
        }
        
        const QByteArray name;
        
        const EntityOptimizations optimizations = {};
        
        const QRegion* const region_ptr = nullptr;
        const RasterSurface2* const surface_ptr = nullptr;
        const draw_fn_t draw_fn = nullptr;
//         draw_fn_t _fast_draw_fn          = nullptr;
                
        mutable QReadWriteLock lock     = QReadWriteLock();
        
        inline void draw(QPainter* painter, const QRegion& drawRegion, bool* error = nullptr) const noexcept
        {
            assert(draw_fn);
            draw_fn(this, painter, drawRegion, error); 
        }
    };
    
    /**
     * EntityBindingBuilder can be passed into the constructor of Entity to 
     * initialize it such that its `fn_data` pointer holds an instance of a 
     * class, and whose properties and methods are populated from data members 
     * and member functions of that class.
     */
    template<class Class>
    class EntityBindingBuilder : public EntityBuilder
    {
    public:
        static_assert(std::is_base_of_v<Entity, Class>);
        
        explicit EntityBindingBuilder(Class* obj)
            : _obj(obj)
        {
            assert(obj);
        }
        
        EntityBindingBuilder& setName(QByteArray name)
        {
            this->name = name;
            return *this;
        }
        
        EntityBindingBuilder& setOptimizations(EntityOptimizations opt)
        {
            this->optimizations = opt;
            return *this;
        }
        
        template<QRegion Class::*DataMember>
        EntityBindingBuilder& bindRegion()
        {
            assert(!this->region_ptr);
            this->region_ptr = addressof_data_member<QRegion, DataMember>();
            return *this;
        }
        
        template<void (Class::*Method)(QPainter*, const QRegion&, bool*) const noexcept>
        EntityBindingBuilder& bindDraw()
        {
            assert(!this->draw_fn);
            this->draw_fn = &(wrap_method<void, QPainter*, const QRegion&, bool*>::template impl<Method>);
            return *this;
        }
        
    private:    
        template<typename Ret, typename... Args>
        struct wrap_method
        {
            template<Ret (Class::*Method)(Args...) const noexcept>
            static Ret impl(const Entity* entity, Args... args) noexcept;
        };
        
        template<typename T, T Class::*DataMember>
        const T* addressof_data_member() const
        { 
            return std::addressof(_obj->*DataMember);
        }
        
        const Class* _obj;
    };
    
    template<class Class>
    template<typename Ret, typename... Args>
    template<Ret (Class::*Method)(Args...) const noexcept>
    Ret EntityBindingBuilder<Class>::wrap_method<Ret, Args...>::impl(const Entity* entity, Args... args) noexcept
    {
        // forwarding references and std::forward are not needed because the 
        // template type parameters are specified and not deduced.
        return (static_cast<const Class*>(entity)->*Method)(args...);
    }

    class RoutineComponentBuilder;

    class RoutineBuilder
    {
    public:
        RoutineBuilder() = default;
        RoutineBuilder(const RoutineBuilder&) = default;
        RoutineBuilder(RoutineBuilder&&) = default;
        
        RoutineBuilder& operator=(const RoutineBuilder&) = default;
        RoutineBuilder& operator=(RoutineBuilder&&) = default;
        
        QByteArray name() const { return _name; }
        RoutineBuilder& setName(QByteArray name) { _name = name; return *this; }
        
        const QMap<double, RoutineComponentBuilder>& components() const & { return _components; }
        QMap<double, RoutineComponentBuilder> components() && { return std::move(_components); }
        RoutineBuilder& setComponents(QMap<double, RoutineComponentBuilder> components) 
        { 
            _components = std::move(components);
            return *this; 
        }
        
        inline RoutineBuilder& addEntity(double z, QSharedPointer<Entity> entity);
        inline RoutineBuilder& addSubRoutine(double z, RoutineBuilder subRoutine);
        inline RoutineBuilder& addSubRoutine(double z, RenderRoutine subRoutine);
        
        double opacity() const { return _opacity; }
        RoutineBuilder& setOpacity(double opacity) { _opacity = opacity; return *this; }
        
        QPainter::CompositionMode compositionMode() const { return _compositionMode; }
        RoutineBuilder& setCompositionMode(QPainter::CompositionMode compositionMode) 
        { 
            _compositionMode = compositionMode; 
            return *this; 
        }
        
        bool passThroughMode() const { return _passThroughMode; }
        RoutineBuilder& setPassThroughMode(bool passThroughMode)
        { 
            _passThroughMode = passThroughMode; 
            return *this; 
        }
        
        QColor background() const { return _background; }
        RoutineBuilder& setBackground(QColor background)
        { 
            _background = background; 
            return *this; 
        }
        
    private:
        QByteArray _name;
        QMap<double, RoutineComponentBuilder> _components;
        
        double _opacity = 1.0;
        QPainter::CompositionMode _compositionMode = QPainter::CompositionMode_SourceOver;
        bool _passThroughMode = false;
        QColor _background;
        
#ifdef ADDLE_TEST
        friend class ::RenderRoutine_UTest;
#endif
    };
    
    struct RoutineData;

    class RoutineComponentBuilder
    {
    public:
        RoutineComponentBuilder() = default;
        RoutineComponentBuilder(const RoutineComponentBuilder&) = default;
        RoutineComponentBuilder(RoutineComponentBuilder&&) = default;
        
        RoutineComponentBuilder& operator=(const RoutineComponentBuilder&) = default;
        RoutineComponentBuilder& operator=(RoutineComponentBuilder&&) = default;
        
        RoutineComponentBuilder(RoutineBuilder subRoutineBuilder)
            : _subRoutineBuilder(std::move(subRoutineBuilder))
        {
        }
        
        RoutineComponentBuilder(QSharedDataPointer<RoutineData> subRoutineData)
            : _subRoutineData(std::move(subRoutineData))
        {
        }
        
        RoutineComponentBuilder(QSharedPointer<Entity> entity)
            : _entity(std::move(entity))
        {
            assert(_entity);
        }
        
        const std::optional<RoutineBuilder>& subRoutineBuilder() const & { return _subRoutineBuilder; }
        std::optional<RoutineBuilder> subRoutineBuilder() && { return std::move(_subRoutineBuilder); }
        
        QSharedDataPointer<RoutineData> subRoutineData() const & { return _subRoutineData; }
        QSharedDataPointer<RoutineData> subRoutineData() && { return std::move(_subRoutineData); }
        
        QSharedPointer<Entity> entity() const & { return _entity; }
        QSharedPointer<Entity> entity() && { return std::move(_entity); }
        
    private:
        std::optional<RoutineBuilder> _subRoutineBuilder;
        QSharedDataPointer<RoutineData> _subRoutineData;
        QSharedPointer<Entity> _entity;
        
#ifdef ADDLE_TEST
        friend class RenderRoutine_UTest;
#endif
    };

    RoutineBuilder& RoutineBuilder::addEntity(double z, QSharedPointer<Entity> entity)
    { 
        assert(entity);
        _components.insert(z, RoutineComponentBuilder(std::move(entity)));
        return *this; 
    }

    RoutineBuilder& RoutineBuilder::addSubRoutine(double z, RoutineBuilder subRoutine)
    { 
        _components.insert(z, RoutineComponentBuilder(std::move(subRoutine)));
        return *this; 
    }
    
    // defined in renderroutine.cpp and renderroutine.moc
    class RoutineSlotProxy;
    
    struct RoutineData : QSharedData
    {
        struct component_entry_t
        {
            double z;
            
            QSharedDataPointer<RoutineData> routineData;
            QSharedPointer<Entity> entity;
            
            QByteArray name() const
            {
                return routineData ? routineData->name : entity->name;
            }
            
            bool operator==(const component_entry_t& other) const
            {
                return z            == other.z
                    && routineData  == other.routineData
                    && entity       == other.entity;
            }
            
            friend bool operator<(const component_entry_t lhs, const component_entry_t& rhs) { return lhs.z < rhs.z; }
            friend bool operator<(double lhs, const component_entry_t& rhs) { return lhs < rhs.z; }
            friend bool operator<(const component_entry_t& lhs, double rhs) { return lhs.z < rhs; }
        };
        using component_list_t = QVector<component_entry_t>;
    
        RoutineData() = default;
        RoutineData(const RoutineData& other)
            : name(other.name),
            components(other.components),
            opacity(other.opacity),
            compositionMode(other.compositionMode),
            passThroughMode(other.passThroughMode),
            background(other.background)
        {
        }
        
        RoutineData(RoutineData&&) = delete;
        RoutineData(aux_render::RoutineBuilder&& builder);
        RoutineData(QByteArray name_) : name(name_) {}
        
        ~RoutineData() noexcept;
        
        const QByteArray name;
        
        component_list_t components;
        
        double opacity = 1.0;
        QPainter::CompositionMode compositionMode = QPainter::CompositionMode_SourceOver;
        bool passThroughMode = false;
        QColor background;
        
        std::size_t subComponentCount = 0;
        
        mutable QRecursiveMutex cacheMutex;
                
        std::size_t maxSubComponentIdCacheCount = 512;
        
        // using QSet because this value is publicly exposed by RenderRoutine, 
        // be mindful not to accidentally detach it in const functions.
        mutable QSet<QByteArray> subComponentNameCache;
        
        struct SubRoutineCacheEntry
        {
            const RoutineData* subRoutineData;
            DataTreeNodeAddress address;
            std::optional<QRegion> region;
            bool regionIsUnbounded = false;
        };
        
        using sub_routine_cache_t = std::list<SubRoutineCacheEntry>;
        mutable sub_routine_cache_t subRoutineCache;
        
        mutable std::unordered_map<QByteArray, sub_routine_cache_t::iterator> subRoutineCacheByName;
        mutable std::unordered_map<const RoutineData*, sub_routine_cache_t::iterator> subRoutineCacheByPointer;
        mutable std::map<DataTreeNodeAddress, sub_routine_cache_t::iterator> subRoutineCacheByAddress;
        
        std::size_t maxSubRoutineCacheCount = 512;
        
        struct EntityCacheEntry
        {
            const Entity* entity;
            DataTreeNodeAddress address;
            std::optional<QRegion> region;
            bool regionIsUnbounded = false;
        };
        
        using entity_cache_t = std::list<EntityCacheEntry>;
        mutable entity_cache_t entityCache;
        
        mutable std::unordered_map<QByteArray, entity_cache_t::iterator> entityCacheByName;
        mutable std::unordered_map<const Entity*, entity_cache_t::iterator> entityCacheByPointer;
        mutable std::map<DataTreeNodeAddress, entity_cache_t::iterator> entityCacheByAddress;
        
        std::size_t maxEntityCacheCount = 512;
        
        mutable std::optional<QRegion> totalRegionCache;
        mutable bool totalRegionIsUnbounded = false;
        
        mutable aux_render::RoutineSlotProxy* slotProxy = nullptr;
                
        void setRenderable_p(const IRenderable* renderable) const;
    
        void onRoutineChanged(RenderRoutineChangedEvent) const;
        void onChanged(QRegion, DataTreeNodeAddress) const;
        
        void invalidateCache() const;
        void pruneCache() const;
    };
    
    class RoutineSlotProxy : public QObject
    {
        Q_OBJECT
    public: 
        RoutineSlotProxy(const aux_render::RoutineData* data, const IRenderable* renderable);
        
        virtual ~RoutineSlotProxy() = default;
    
        void dispose();
        
    private slots:
        void onDestroyed();
        void onRoutineChanged(RenderRoutineChangedEvent event);
        void onChanged(QRegion region, DataTreeNodeAddress entity);
        
    private:
        const aux_render::RoutineData* _data;
    };
    
    class RoutineComponentView;
    
    class RoutineNodeHandle
    {
    public:
        RoutineNodeHandle() = default;
        RoutineNodeHandle(const RoutineNodeHandle&) = default;
        
        RoutineNodeHandle& operator=(const RoutineNodeHandle&) = default;
        
        RoutineNodeHandle(const RoutineData* routineData)
            : _routineData(routineData)
        {
        }
        
        RoutineNodeHandle(const RoutineData::component_entry_t& component)
            : _component(&component)
        {
        }
        
        explicit operator bool () const { return _routineData || _component; }
        bool operator! () const { return !_routineData && !_component; }
        
        bool operator==(const RoutineNodeHandle& other) const 
        { 
            return _routineData == other._routineData 
                && _component == other._component;
        }
        
        bool operator!=(const RoutineNodeHandle& other) const 
        { 
            return _routineData != other._routineData 
                || _component != other._component;
        }
        
        inline RoutineComponentView operator*() const;
        
        const RoutineData* routineData() const
        {
            return _component ? _component->routineData.data() : _routineData; 
        }
        
        const Entity* entity() const
        { 
            return (_component && !_component->routineData) ? (_component->entity.data()) : nullptr;
        }
        
    protected:
        const RoutineData* _routineData = nullptr;
        const RoutineData::component_entry_t* _component = nullptr;
        
        friend aux_datatree::ChildNodeIterator<RenderRoutine, true> datatree_node_children_begin(const RoutineNodeHandle& handle);
        friend aux_datatree::ChildNodeIterator<RenderRoutine, true> datatree_node_children_end(const RoutineNodeHandle& handle);
        
    #ifdef ADDLE_TEST
        friend class ::RenderRoutine_UTest;
    #endif
    };
    
    class RoutineComponentView : private RoutineNodeHandle
    {
    public:
        RoutineComponentView() = default;
        RoutineComponentView(const RoutineComponentView&) = default;
        RoutineComponentView(RoutineComponentView&&) = default;
        
        double z() const
        { 
            return _component ? _component->z : qQNaN(); 
        }
        
        QByteArray name() const
        { 
            if (_routineData) 
                return _component->name();
            else if (_component)
                return _routineData->name;
            else
                return QByteArray();
        }
        
        bool isSubRoutine() const
        { 
            return _routineData || (_component && _component->routineData); 
        }
        inline RenderRoutine asSubRoutine() const;
        
        bool isEntity() const { return _component && !_component->routineData; }
        const Entity& asEntity() const 
        {
            assert(isEntity());
            return *entity(); 
        }
        
        friend class RoutineNodeHandle;
#ifdef ADDLE_TEST
        friend class ::RenderRoutine_UTest;
#endif
    };
    
    inline RoutineComponentView RoutineNodeHandle::operator*() const
    {
        assert(*this);
        return static_cast<const RoutineComponentView&>(*this);
    }
    
    // Thrown when a query on a render routine times out
    class RoutineQueryTimeout : public std::runtime_error
    {
#ifdef ADDLE_DEBUG
    public:
        // For debug builds only, the exception reports which RenderRoutine 
        // query operation was being performed when the timeout occurred, and 
        // which category of lock timed out
        
        enum _WhichOp
        {
            _WhichOp_region
        };
        
        enum _WhichLock
        {
            _WhichLock_cacheMutex,
            _WhichLock_entityReadLock
        };
        
        RoutineQueryTimeout(int waited, _WhichOp op, _WhichLock lock);
        
        _WhichOp _whichOp() const { return _op; }
        _WhichLock _whichLock() const { return _lock; }
        
    private:
        _WhichOp _op;
        _WhichLock _lock;
#else
    public:
        explicit RoutineQueryTimeout(int waited);
#endif
        RoutineQueryTimeout(const RoutineQueryTimeout&) = default;
        int waited() const { return _waited; }
    private:
        int _waited;
    };
    
    class RoutineComponentNameCollision : public std::runtime_error
    {
    public:
        explicit RoutineComponentNameCollision(QList<QByteArray> names);
        RoutineComponentNameCollision(const RoutineComponentNameCollision&) = default;
        QList<QByteArray> names() const { return _names; }
    private:
        QList<QByteArray> _names;
    };
}

/**
 * Declaratively gives rendering instructions for a renderable object.
 * 
 * A RenderRoutine is made up of a set of components, with each component having 
 * a unique id and z-value. A component can either be an entity, with a shared 
 * pointer to an object that performs some step of the rendering routine (e.g., 
 * paints onto a paint device, exposes a mask, or performs an effect on a 
 * buffer), or a component can be a subroutine, consisting of a nested 
 * RenderRoutine.
 * 
 * RenderRoutine are exposed by instances of IRenderable and consumed by
 * IRenderer. A renderable may build its render routine out of the routines of 
 * subordinate renderables as desired. External alterations may be made to a
 * routine in the context of a particular renderer without affecting the
 * renderable itself or its routine in any other renderers.
 */
class RenderRoutine
{
public:
    using component_range_t = aux_datatree::ConstChildNodeRange<RenderRoutine>;
    using dfs_range_t = aux_datatree::ConstNodeRange<RenderRoutine>;
    
    RenderRoutine() = default;
    RenderRoutine(const RenderRoutine&) = default;
    RenderRoutine(RenderRoutine&&) = default;
    
    RenderRoutine& operator=(const RenderRoutine&) = default;
    RenderRoutine& operator=(RenderRoutine&&) = default;
    
    RenderRoutine(QByteArray name);
    RenderRoutine(aux_render::RoutineBuilder builder);
    
    inline bool operator==(const RenderRoutine& other) const
    {
        return (_data == other._data)
            || (
                _data && other._data
                && (
                       _data->name              == other._data->name
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
    
//     inline RenderRoutine& setId(QUuid id)
//     {
//         initData();
//         _data->id = id;
//         return *this;
//     }
    
    inline QByteArray name() const { return _data ? _data->name : QByteArray(); }
    
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
    
    //QSet<QUuid> subComponentIds(
    
    std::optional<QRegion> region(DataTreeNodeAddress componentAddress = {}, 
        bool skipCache = false, 
        int timeout = -1) const;
    
    aux_render::RoutineNodeHandle findComponent(DataTreeNodeAddress componentAddress,
        bool skipCache = false,
        int timeout = -1) const;
        
    aux_render::RoutineNodeHandle findComponentByName(QByteArray name,
        bool skipCache = false,
        int timeout = -1) const;
        
private:
    struct Query
    {
        enum Op
        {
            Op_Initialize,
            Op_CalcRegion,
            Op_FindIds,
            Op_FindComponentByName,
            Op_FindComponentByAddress
        };
        
        const aux_render::RoutineData* data;
        
        DataTreeNodeAddress target;
        Op op = (Op)(NULL);
        QByteArray targetName;
        
        int timeout = -1;
        
        aux_render::RoutineNodeHandle cursor;
        DataTreeNodeAddress cursorAddress;
        
        QVector<aux_render::RoutineData::EntityCacheEntry> entities;
        QVector<aux_render::RoutineData::SubRoutineCacheEntry> subRoutines;
        
        struct PendingRegionEntry
        {
            DataTreeNodeAddress address;
            QRegion region;
        };
        
        QVector<PendingRegionEntry> pendingRegions;
    
        QRegion                             resultRegion;
        bool                                resultRegionIsUnbounded = false;
        aux_render::RoutineNodeHandle       resultNode;
        std::optional<DataTreeNodeAddress>  resultAddress;
        QSet<QByteArray>                    resultNames;
    
        bool hasResult() const { return static_cast<bool>(resultNode); }
        void run();
        
    private:
        void updatePendingRegions(QRegion cursorRegion, bool cursorRegionIsUnbounded);
        void commit();
    };
    
    inline void initData()
    {
        if (!_data) 
            _data = new aux_render::RoutineData;
        else 
            _data.detach();
    }

    void addEntity_p(QSharedPointer<aux_render::Entity> entity, double z = 0);
    void addSubRoutine_p(RenderRoutine subRoutine, double z = 0);
    
    QSharedDataPointer<aux_render::RoutineData> _data;
    
    friend class aux_datatree::ChildNodeIterator<RenderRoutine, true>;
    friend class aux_datatree::ChildNodeIterator<RenderRoutine, false>;
        // Should we even allow non-const iterators?
    
    friend class aux_render::RoutineNodeHandle;
    friend class aux_render::RoutineSlotProxy;
    friend class aux_render::RoutineComponentView;
    friend aux_render::RoutineNodeHandle datatree_root(const RenderRoutine& routine);
    friend aux_render::RoutineBuilder& aux_render::RoutineBuilder::addSubRoutine(double, RenderRoutine);
    
#ifdef ADDLE_TEST
    friend class ::RenderRoutine_UTest;
#endif
};

inline aux_render::RoutineBuilder& aux_render::RoutineBuilder::addSubRoutine(double z, RenderRoutine subRoutine)
{
    _components.insert(z, RoutineComponentBuilder(std::move(subRoutine._data)));
    return *this;
}

inline RenderRoutine aux_render::RoutineComponentView::asSubRoutine() const
{
    RenderRoutine result;
    if (Q_LIKELY(routineData()))
        result._data = const_cast<RoutineData*>(routineData());
    
    return result;
}

template<bool IsConst>
class aux_datatree::ChildNodeIterator<RenderRoutine, IsConst>
    : public boost::iterator_adaptor<
        ChildNodeIterator<RenderRoutine, IsConst>,
        aux_render::RoutineData::component_list_t::const_iterator,
        aux_render::RoutineComponentView,
        boost::use_default,
        aux_render::RoutineComponentView
    >
{
    using adaptor_t = boost::iterator_adaptor<
            ChildNodeIterator<RenderRoutine, IsConst>,
            aux_render::RoutineData::component_list_t::const_iterator,
            aux_render::RoutineComponentView,
            boost::use_default,
            aux_render::RoutineComponentView
        >;
    
public:
    ChildNodeIterator() = default;
    ChildNodeIterator(const ChildNodeIterator&) = default;
    ChildNodeIterator& operator=(const ChildNodeIterator&) = default;
    
    ChildNodeIterator(aux_render::RoutineData::component_list_t::const_iterator base)
        : adaptor_t(base)
    {
    }
    
    inline aux_render::RoutineNodeHandle cursor() const
    {
        return aux_render::RoutineNodeHandle(*this->base());
    }
    
    inline operator aux_render::RoutineNodeHandle () const { return this->cursor(); }
    
private:
    aux_render::RoutineComponentView dereference() const
    {
        return *(this->cursor());
    }
    
    friend class boost::iterator_core_access;
};

namespace aux_render {
    inline aux_datatree::ChildNodeIterator<RenderRoutine, true> datatree_node_children_begin(const RoutineNodeHandle& handle)
    {
        if (Q_LIKELY(handle))
        {
            if (handle._routineData)
                return handle._routineData->components.begin();
            else if (handle._component && handle._component->routineData)
                return handle._component->routineData->components.begin();
        }
        
        return {};
    }
        
    inline aux_datatree::ChildNodeIterator<RenderRoutine, true> datatree_node_children_end(const RoutineNodeHandle& handle)
    {
        if (Q_LIKELY(handle))
        {
            if (handle._routineData)
                return handle._routineData->components.end();
            else if (handle._component && handle._component->routineData)
                return handle._component->routineData->components.end();
        }
        
        return {};
    }
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
    using dfs_node_t = aux_datatree::DFSExtendedNode<aux_render::RoutineNodeHandle, true>;
    if (Q_UNLIKELY(!_data)) return dfs_range_t();
    
    return dfs_range_t(
            dfs_node_t(aux_render::RoutineNodeHandle(_data.data())),
            dfs_node_t()
        );
}

inline aux_render::RoutineNodeHandle datatree_root(const RenderRoutine& routine)
{
    return aux_render::RoutineNodeHandle(routine._data.data());
}

/**
 * Identifies changes that have been made to a render routine and/or its 
 * descendant subroutines.
 */
class RenderRoutineChangedEvent
{    
//     using entity_flags_list_t = QList<std::pair<std::size_t, aux_render::EntityFlags>>;
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
    
//     bool entityFlagsChanged(const DataTreeNodeAddress& parent = {}) const
//     {
//         if (parent.isRoot())
//         {
//             return !_entityFlags.isEmpty();
//         }
//         else 
//         {
//             auto i = _subRoutineEvents.find(parent);
//             return i != _subRoutineEvents.end() 
//                 && !((*i)._entityFlags.isEmpty());
//         }
//     }
    
//     auto entityFlagsList(const DataTreeNodeAddress& parent = {}) const
//     {
//         using namespace boost::adaptors;
//             
//         entity_flags_list_t list;
//             
//         if (parent.isRoot())
//         {
//             list = _entityFlags;
//         }
//         else 
//         {
//             auto i = _subRoutineEvents.find(parent);
//             if (i != _subRoutineEvents.end())
//                 list = (*i)._entityFlags;
//         }
//         
//         return  
//               noDetach(std::move(list)) 
//             | transformed([parent] (auto&& entry) {
//                     return std::make_pair(parent << entry.first, entry.second);
//                 });
//     }
    
//     aux_render::EntityFlags entityFlags(const DataTreeNodeAddress& entity) const
//     {
//         if (Q_UNLIKELY(entity.isRoot()))
//             return {};
//         
//         entity_flags_list_t list;
//         auto parent = entity.parent();
//         
//         if (parent.isRoot())
//         {
//             list = _entityFlags;
//         }
//         else
//         {
//             auto i = _subRoutineEvents.find(parent);
//             if (i == _subRoutineEvents.end())
//                 return {};
//             
//             list = (*i)._entityFlags;
//         }
//         
//         auto j = std::lower_bound( 
//                 list.cbegin(),
//                 list.cend(),
//                 entity.lastIndex(),
//                 [] (const auto& entry, std::size_t index) -> bool {
//                     return entry.first < index; 
//                 }
//             );
//         
//         if (j != list.cend() && (*j).first == entity.lastIndex())
//             return (*j).second;
//         else
//             return {};
//     }
    
//     RenderRoutineChangedEvent& setEntityFlags(
//             const DataTreeNodeAddress& entity, 
//             aux_render::EntityFlags flags
//         )
//     {
//         auto parent = entity.parent();
// 
//         entity_flags_list_t& list = parent.isRoot() ? 
//             _entityFlags : _subRoutineEvents[parent]._entityFlags;
//                 
//         auto i = std::lower_bound(
//                 list.begin(),
//                 list.end(),
//                 entity.lastIndex(),
//                 [] (const auto& entry, std::size_t index) -> bool {
//                     return entry.first < index; 
//                 }
//             );
//         
//         if (i != list.cend() && (*i).first == entity.lastIndex())
//             (*i).second = flags;
//         else
//             list.insert(i, { entity.lastIndex(), flags });
//         
//         return *this;
//     }
    
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
//     entity_flags_list_t _entityFlags;
    
    double  _opacity;
    QPainter::CompositionMode _compositionMode;
    bool    _passThroughMode;
    QColor  _background;
    
    sub_routine_map_t _subRoutineEvents;
    
    bool _opacityChanged         : 1;
    bool _compositionModeChanged : 1;
    bool _passThroughModeChanged : 1;
    bool _backgroundChanged      : 1;
    
#ifdef ADDLE_TEST
    friend class ::RenderRoutine_UTest;
#endif
};

} // namespace Addle

//Q_DECLARE_OPERATORS_FOR_FLAGS(Addle::aux_render::EntityFlags);
Q_DECLARE_METATYPE(Addle::RenderRoutineChangedEvent);
