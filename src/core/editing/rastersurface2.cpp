#include <cstdlib> // malloc, calloc, free
#include <cstring> // memcpy

#include <QMargins>

#include "rastersurface2.hpp"
#include "utilities/multi_array_util.hpp"
#include "utilities/math.hpp"

using namespace Addle;

RasterSurface2::RasterSurface2()
    : _grid(nullptr, boost::extents[0][0]), 
    _renderEntity(QSharedPointer<RenderEntity>::create(this))
{
}

RasterSurface2::~RasterSurface2()
{
    {
        const QWriteLocker l(&_renderEntity->lock);
        _renderEntity->surface = nullptr;
        _renderEntity->region = {};
    }

    clear_p();
}

IRasterSurface2* RasterSurface2::clone() const
{
    Q_UNIMPLEMENTED();
    return nullptr;
}

RasterSurface2::tiles_view_t RasterSurface2::tiles() const
{
    using boost::multi_array_types::index_range;
    
    auto p = populatedGridRect().translated(-_gridOffset);
    if (p.isEmpty()) return nullTilesView();
    
    return _grid[boost::indices
            [index_range(p.left(), p.right() + 1)]
            [index_range(p.top(), p.bottom() + 1)]
        ];
}

const RasterTile RasterSurface2::tileAt(QPoint p) const
{
    auto gridPoint = toGridRect(QRect(p, QSize(1,1))).topLeft();
    if (!populatedGridRect().contains(gridPoint)) return {};
    
    gridPoint -= _gridOffset;
    return _grid[gridPoint.x()][gridPoint.y()];
}

RasterSurface2::tiles_view_t RasterSurface2::tilesAt(QRect rect) const
{
    using boost::multi_array_types::index_range;
    
    auto r = populatedGridRect().translated(-_gridOffset)
        .intersected(toGridRect(rect));
        
    if (r.isEmpty()) return nullTilesView();
        
    return _grid[boost::indices
            [index_range(r.left(), r.right() + 1)]
            [index_range(r.top(), r.bottom() + 1)]
        ];
}

const QList<RasterTile> RasterSurface2::tilesAt(QRegion region) const
{
    using boost::multi_array_types::index_range;
    
    if (!_populatedTileCount) return {};
    
    QList<RasterTile> result;
    auto r = populatedGridRect().intersected(toGridRect(region.boundingRect()))
        .translated(-_gridOffset);
        
    auto tiles = _grid[boost::indices
            [index_range(r.left(), r.right() + 1)]
            [index_range(r.top(), r.bottom() + 1)]
        ];
    
    for (auto&& tile : aux_multi_array_util::traverse_rows(tiles))
    {
        if (!tile.isNull()
            && region.intersects(toCanvasRect(
                QRect(gridPositionOfTile(&tile), QSize(1,1))
            )))
        {
            result.append(tile);
        }
    }
    
    return result;
}

const QList<RasterTile> RasterSurface2::tilesAt(QPainterPath path, int margin) const
{
    using boost::multi_array_types::index_range;
    
    if (!_populatedTileCount) return {};
    
    const QMargins m(margin, margin, margin, margin);
    
    QList<RasterTile> result;
    auto r = populatedGridRect().intersected(toGridRect(
            coarseBoundRect(path.boundingRect()).marginsAdded(m)
        )).translated(-_gridOffset);
        
    auto tiles = _grid[boost::indices
            [index_range(r.left(), r.right() + 1)]
            [index_range(r.top(), r.bottom() + 1)]
        ];
    
    for (auto&& tile : aux_multi_array_util::traverse_rows(tiles))
    {
        if (!tile.isNull()
            && path.intersects(
                toCanvasRect(
                    QRect(gridPositionOfTile(&tile), QSize(1,1))
                ).marginsAdded(m)
            ))
        {
            result.append(tile);
        }
    }
    
    return result;
}

RasterTileEditHandle RasterSurface2::editTiles(QRect rect)
{
    using boost::multi_array_types::index_range;
    QList<RasterTile*> resultTiles;
    
    if (rect.isEmpty()) return RasterTileEditHandle();
            
    _renderEntity->lock.lockForWrite();
    
    reserve_p(rect);
    
    auto r = toGridRect(rect).translated(-_gridOffset);
    auto tiles = _grid[boost::indices
            [index_range(r.left(), r.right() + 1)]
            [index_range(r.top(), r.bottom() + 1)]
        ];
    
    for (auto&& tile : aux_multi_array_util::traverse_rows(tiles))
    {
        if (tile.isNull())
        {
            auto p = gridPositionOfTile(&tile);
            tile = RasterTile(_format, p * (int)TILE_SIDE_LENGTH,
                _colorTable);
            incrementPopulation(p);
        }
        
        resultTiles.append(&tile);
    }
    
    return RasterTileEditHandle(
            std::move(resultTiles),
            QRegion(rect),
            [&] (RasterTileEditHandle& handle) {
                bool regionChanged_ = !(handle.regionHint() - _region).isEmpty();
                _region += handle.regionHint();
                _renderEntity->region = _region;
                
                _renderEntity->lock.unlock();
                
                if (!handle.regionHint().isEmpty())
                    emit renderChanged(handle.regionHint());
                if (regionChanged_)
                    emit regionChanged(_region);
            }
        );
}

RasterTileEditHandle RasterSurface2::editTiles(QRegion region)
{
}

RasterTileEditHandle RasterSurface2::editTiles(QPainterPath path, int margin)
{
}

RasterTilePaintHandle RasterSurface2::paintTiles(QRect rect)
{
    using boost::multi_array_types::index_range;
    QList<RasterTile*> resultTiles;
    
    if (rect.isEmpty()) return RasterTilePaintHandle();
            
    _renderEntity->lock.lockForWrite();
    
    reserve_p(rect);
    
    auto r = toGridRect(rect).translated(-_gridOffset);
    auto tiles = _grid[boost::indices
            [index_range(r.left(), r.right() + 1)]
            [index_range(r.top(), r.bottom() + 1)]
        ];
    
    for (auto&& tile : aux_multi_array_util::traverse_rows(tiles))
    {
        if (tile.isNull())
        {
            auto p = gridPositionOfTile(&tile);
            tile = RasterTile(_format, p * (int)TILE_SIDE_LENGTH,
                _colorTable);
            incrementPopulation(p);
        }
        
        resultTiles.append(&tile);
    }
    
    return RasterTilePaintHandle(
            std::move(resultTiles),
            QRegion(rect),
            [&] (RasterTilePaintHandle& handle) {
                bool regionChanged_ = !(handle.regionHint() - _region).isEmpty();
                _region += handle.regionHint();
                _renderEntity->region = _region;
                
                _renderEntity->lock.unlock();
                
                if (!handle.regionHint().isEmpty())
                    emit renderChanged(handle.regionHint());
                if (regionChanged_)
                    emit regionChanged(_region);
            }
        );
}

RasterTilePaintHandle RasterSurface2::paintTiles(QRegion region)
{
}

RasterTilePaintHandle RasterSurface2::paintTiles(QPainterPath path, int margin)
{
}

RenderRoutine RasterSurface2::renderRoutine() const
{
    return aux_render::RoutineBuilder()
        .addEntity(0.0, _renderEntity);
}
    
void RasterSurface2::squeeze()
{
    const QWriteLocker l(&_renderEntity->lock);
    if (_populatedTileCount)
        rebaseGrid(populatedGridRect());
    else
        clear_p();
}

void RasterSurface2::clear_p()
{
    using boost::multi_array_types::index_range;
    
    const QWriteLocker l(&_renderEntity->lock);
    
    if (!_grid.empty())
    {
        assert(_grid.data());
        
        if (_populatedTileCount)
        {
            auto p = populatedGridRect().translated(-_gridOffset);
            auto tiles = _grid[boost::indices
                    [index_range(p.left(), p.right() + 1)]
                    [index_range(p.top(), p.bottom() + 1)]
                ];
                
            for (RasterTile& tile : aux_multi_array_util::traverse(tiles))
                tile.~RasterTile();
        }
        
        std::free(_grid.data());
        
        _grid.~grid_t();
        new (&_grid) grid_t(nullptr, boost::extents[0][0]);
    }
    
    _gridOffset         = {};
    _populatedTileCount = 0;
    _populatedTileCountByXCoord.clear();
    _populatedTileCountByYCoord.clear();
    _region             = {};
    _renderEntity->region = {};
}

void RasterSurface2::reserve_p(QRect area)
{
    auto r = toGridRect(area);
    if (!_populatedTileCount)
        _gridOffset = r.topLeft();
    
    rebaseGrid(r);
}

void RasterSurface2::rebaseGrid(QRect newGrid)
{
    using boost::multi_array_types::index_range;
    if (newGrid.isEmpty() || gridRect().contains(newGrid)) return; // noop
    
    assert(!_populatedTileCount || newGrid.contains(populatedGridRect()));
    // Not allowed to de-populate tiles with rebase
    
    uint count = newGrid.width() * newGrid.height();
    grid_t oldGrid(_grid);
    
    RasterTile* data = (RasterTile*) std::calloc(count, sizeof(RasterTile));
    
    _grid.~grid_t();
    new (&_grid) grid_t(data, boost::extents[newGrid.width()][newGrid.height()]);
    
    if (_populatedTileCount)
    {
        assert(!oldGrid.empty() && oldGrid.data());
        
        auto moveTo = _grid[boost::indices
                [index_range(_gridOffset.x() - newGrid.left(), 
                             _gridOffset.x() - newGrid.left() + oldGrid.shape()[0])]
                [index_range(_gridOffset.y() - newGrid.top(), 
                             _gridOffset.y() - newGrid.top() + oldGrid.shape()[1])]
            ];
            
        auto fromIt = oldGrid.begin();
        auto fromEnd = oldGrid.end();
        auto toIt = moveTo.begin();
        auto toEnd = moveTo.end();
        
        while (fromIt != fromEnd)
        {
            assert(toIt != toEnd);
            
            std::memcpy((*toIt).origin(),
                        (*fromIt).origin(), 
                        oldGrid.shape()[1] * sizeof(RasterTile));
            
            ++fromIt;
            ++toIt;
        }
    }
    
    if (oldGrid.data()) std::free(oldGrid.data());
    _gridOffset = newGrid.topLeft();
}

QRect RasterSurface2::populatedGridRect() const
{
    if (!_populatedTileCount) return QRect();
    
    assert(!_populatedTileCountByXCoord.empty() && !_populatedTileCountByYCoord.empty());
    
    auto x1 = _populatedTileCountByXCoord.front().first;
    auto x2 = _populatedTileCountByXCoord.back().first;
    
    auto y1 = _populatedTileCountByXCoord.front().first;
    auto y2 = _populatedTileCountByXCoord.back().first;
    
    return QRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

QPoint RasterSurface2::gridPositionOfTile(const RasterTile* tile) const
{
    return QPoint(
            (tile - _grid.origin()) / _grid.strides()[0],
            (tile - _grid.origin()) % _grid.strides()[0]
        ) + _gridOffset;
}

void RasterSurface2::incrementPopulation(QPoint p)
{
    auto xIt = std::lower_bound(
            _populatedTileCountByXCoord.begin(),
            _populatedTileCountByXCoord.end(),
            p.x(),
            [] (const auto& pair, int x) -> bool { return pair.first < x; }
        );
    
    if (xIt != _populatedTileCountByXCoord.cend() && (*xIt).first == p.x())
        ++(*xIt).second;
    else
        _populatedTileCountByXCoord.insert(xIt, { p.x(), 1 });
    
    auto yIt = std::lower_bound(
            _populatedTileCountByYCoord.begin(),
            _populatedTileCountByYCoord.end(),
            p.y(),
            [] (const auto& pair, int y) -> bool { return pair.first < y; }
        );
    
    if (yIt != _populatedTileCountByYCoord.cend() && (*yIt).first == p.y())
        ++(*yIt).second;
    else
        _populatedTileCountByYCoord.insert(yIt, { p.y(), 1 });
    
    ++_populatedTileCount;
}

void RasterSurface2::decrementPopulation(QPoint p)
{
    assert(_populatedTileCount);
    
    auto xIt = std::lower_bound(
            _populatedTileCountByXCoord.begin(),
            _populatedTileCountByXCoord.end(),
            p.x(),
            [] (const auto& pair, int x) -> bool { return pair.first < x; }
        );
    
    assert(xIt != _populatedTileCountByXCoord.cend() && (*xIt).first == p.x());  
    
    --(*xIt).second;
    if (!(*xIt).second)
        _populatedTileCountByXCoord.erase(xIt);
    
    auto yIt = std::lower_bound(
            _populatedTileCountByYCoord.begin(),
            _populatedTileCountByYCoord.end(),
            p.y(),
            [] (const auto& pair, int y) -> bool { return pair.first < y; }
        );
    
    assert(yIt != _populatedTileCountByYCoord.cend() && (*yIt).first == p.y());
    
    --(*yIt).second;
    if (!(*yIt).second)
        _populatedTileCountByYCoord.erase(yIt);
    
    --_populatedTileCount;
}
    
RasterSurface2::RenderEntity::RenderEntity(RasterSurface2* surface_)
    : aux_render::Entity(aux_render::EntityBindingBuilder(this)
            .bindDraw<&RenderEntity::draw_p>()
            .bindRegion<&RenderEntity::region>()),
    surface(surface_)
{
}

void RasterSurface2::RenderEntity::draw_p(QPainter* painter, 
    const QRegion& region, bool* error) const noexcept
{
    using boost::multi_array_types::index_range;
    
    assert(painter);
    if (Q_UNLIKELY(!surface))
    {
        if (error) *error = true;
        return;
    }
    
    auto r = toGridRect(region.boundingRect())
        .intersected(surface->populatedGridRect())
        .translated(-surface->_gridOffset);
        
    auto tiles = surface->_grid[boost::indices
            [index_range(r.left(), r.right() + 1)]
            [index_range(r.top(), r.bottom() + 1)]
        ];
        
    for (const auto& tile : aux_multi_array_util::traverse(tiles))
    {
        if (tile.isNull())
        {
            // technically, a null tile is meant to be equivalent to a tile 
            // with all pixels at the zero-color, so if the format doesn't 
            // support transparency, this should probably be a fill operation
            continue;
        }
        
        painter->drawImage(tile.offset(), tile.asImage(),                            
                           QRect(0, 0, TILE_SIDE_LENGTH, TILE_SIDE_LENGTH),
                           Qt::NoFormatConversion);
    }
}
