#pragma once

#include <functional>
#include <utility>
#include <optional>

#include <QList>
#include <QRegion>

#include <QImage>
#include <QPainter>

#include <boost/iterator/iterator_adaptor.hpp>
#include "./rastertile.hpp"

namespace Addle {

namespace aux_RasterTileEditHandle {

using tile_list_t    = QList<RasterTile*>;
    
template<class Derived>
class Handle_base
{
protected:
    using on_finished_t = std::function<void(Derived&)>;
public:
    
    Handle_base() = default;
    Handle_base(const Handle_base&) = delete;
    Handle_base(Handle_base&& other)
        : _tiles(std::move(other._tiles)),
        _regionHint(std::move(other._regionHint)),
        _onFinished(std::exchange(other._onFinished, {}))
    {
    }
    
    Handle_base(QList<RasterTile*> tiles, QRegion regionHint,
            on_finished_t onFinished)
        : _tiles(tiles), _regionHint(regionHint), _onFinished(onFinished)
    {
        assert(onFinished);
    }
    
    ~Handle_base()
    { 
        if (_onFinished) _onFinished(static_cast<Derived&>(*this)); 
    }
    
    Handle_base& operator=(const Handle_base&) = delete;
    Handle_base& operator=(Handle_base&& other)
    {
        if (_onFinished) _onFinished(static_cast<Derived&>(*this)); 
        
        _regionHint     = std::move(other._regionHint);
        _tiles          = std::move(other._tiles);
        _onFinished    = std::exchange(other._onFinished, {});
        
        other.deactivate();
        
        return *this;
    }
    
    void finish() 
    { 
        if (_onFinished) 
        {
            _onFinished(static_cast<Derived&>(*this));
            _onFinished = {};
            _tiles.clear();
        }
    }
    
    void deactivate()       { _onFinished = {}; }
    bool isActive() const   { return _onFinished; }
    
    QRegion regionHint() const          { return _regionHint; }
    void setRegionHint(QRegion hint)    { _regionHint = std::move(hint); }
    
protected:
    tile_list_t         _tiles;
    QRegion             _regionHint;
    
    on_finished_t      _onFinished;
};

}

/**
 * Exposes a set of tiles in IRasterSurface2 for editing.
 */
class RasterTileEditHandle 
    : public aux_RasterTileEditHandle::Handle_base<RasterTileEditHandle>
{
    using base_t = aux_RasterTileEditHandle::Handle_base<RasterTileEditHandle>;
    using base_t::on_finished_t;
    using tile_list_t = aux_RasterTileEditHandle::tile_list_t;
public:
    class iterator : public boost::iterator_adaptor<
            iterator, tile_list_t::const_iterator, RasterTile
        >
    {
        using adaptor_t = boost::iterator_adaptor<
                iterator, tile_list_t::const_iterator, RasterTile
            >;
        using base_t = tile_list_t::const_iterator;
            
    public:
        iterator() = default;
        iterator(const iterator& other) : adaptor_t(other) {}
        iterator(base_t base) : adaptor_t(base) {}
        
    private:
        RasterTile& dereference() const 
        {
            assert(*base());
            return **base();
        }
        
        friend class boost::iterator_core_access;
    };
    
    RasterTileEditHandle() = default;
    RasterTileEditHandle(tile_list_t tiles, QRegion regionHint, 
        on_finished_t onFinished)
        : base_t(tiles, regionHint, onFinished)
    {
    }
    
    iterator begin() const  { return iterator(_tiles.begin()); }
    iterator end() const    { return iterator(_tiles.end()); }
};

class RasterTilePaintHandle 
    : public aux_RasterTileEditHandle::Handle_base<RasterTilePaintHandle>
{
    using base_t = aux_RasterTileEditHandle::Handle_base<RasterTilePaintHandle>;
    using base_t::on_finished_t;
    using tile_list_t = aux_RasterTileEditHandle::tile_list_t;
public:
    class iterator : public boost::iterator_adaptor<
            iterator, tile_list_t::const_iterator, QPainter
        >
    {
        using adaptor_t = boost::iterator_adaptor<
                iterator, tile_list_t::const_iterator, QPainter
            >;
        using base_t = tile_list_t::const_iterator;
        
        struct State
        {
            State(RasterTile* tile);
            ~State();
            
            QImage image;
            QPainter painter;
        };
        
    public:
        iterator() = default;
        iterator(const iterator& other) : adaptor_t(other) {}
        iterator(base_t base) : adaptor_t(base) {}
        
        void release() { _state.reset(); }
        
    private:
        QPainter& dereference() const 
        {
            assert(*base());
            
            if (!_state) _state.emplace(*base());
            return _state->painter;
        }
        
        void increment() { _state.reset(); ++base_reference(); }
        void decrement() { _state.reset(); --base_reference(); }
        void advance(std::ptrdiff_t n) { _state.reset(); base_reference() += n; }
    
        mutable std::optional<State> _state;
        
        friend class boost::iterator_core_access;
    };
    
    RasterTilePaintHandle() = default;
    RasterTilePaintHandle(tile_list_t tiles, QRegion regionHint, 
        on_finished_t onFinished)
        : base_t(tiles, regionHint, onFinished)
    {
    }
    
    iterator begin() const  { return iterator(_tiles.begin()); }
    iterator end() const    { return iterator(_tiles.end()); }
};

}
