#include <QPixelFormat>
#include <algorithm>
#include <cstring> // std::memcpy

#include "utils.hpp"

#include "tilesurface.hpp"
#include "utilities/math.hpp"

using namespace Addle;

std::size_t aux_tilesurface::calcRasterDataLength(QImage::Format format)
{
    static_assert(TILE_SIDE_LENGTH % 32 == 0);
    // Inconvenient values of TILE_WIDTH may require some extra logic to meet
    // padding and alignment requirements. This is not implemented and it is 
    // unlikely to be needed.
    
    return (TILE_SIDE_LENGTH * TILE_SIDE_LENGTH / 8) * QImage::toPixelFormat(format).bitsPerPixel();
}

aux_tilesurface::Tile::TileData::TileData(const TileData& other)
    : rasterData(other.rasterData), rasterDataRefCount(other.rasterDataRefCount),
    offset(other.offset), format(other.format), colorTable(other.colorTable)
{
    if (rasterData)
    {
        assert(rasterDataRefCount);
        rasterDataRefCount->ref();
    }
}

aux_tilesurface::Tile::TileData::~TileData()
{
    if (rasterData)
    {
        assert(rasterDataRefCount);
        if (!rasterDataRefCount->deref())
        {
            disposeRasterData();
            delete rasterDataRefCount;
        }
    }
}

void aux_tilesurface::Tile::TileData::detachRasterData()
{
    assert(rasterDataRefCount);
    
    if (rasterDataRefCount->loadRelaxed() == 1) 
        return;
    
    const uchar* oldRasterData(rasterData);
    
    const std::size_t size = calcRasterDataLength(format);
    
    rasterData = (uchar*) std::malloc(size);
    
    std::memcpy(rasterData, oldRasterData, size);
    
    assert(rasterDataRefCount->deref());
    rasterDataRefCount = new QAtomicInteger<unsigned>(1);
}

void aux_tilesurface::Tile::TileData::disposeRasterData()
{
    assert(rasterData);
    assert(rasterDataRefCount && !rasterDataRefCount->loadRelaxed());
    
    std::free(rasterData);
}

QImage aux_tilesurface::Tile::TileData::getImage() const
{
    assert(rasterData);
    ref.ref();
    
    QImage result(rasterData, TILE_SIDE_LENGTH, TILE_SIDE_LENGTH, format, 
        &cleanupImage, const_cast<void*>(static_cast<const void*>(this)));
    result.setOffset(offset);
    result.setColorTable(colorTable);
    
    return result;
}

void aux_tilesurface::Tile::cleanupImage(void* data_)
{
    TileData* data = static_cast<TileData*>(data_);
    assert(data);
    
    if (!data->ref.deref())
        delete data;
}

aux_tilesurface::TileSurfaceData::TileSurfaceData(const TileSurfaceData& other)
    : format(other.format), colorTable(other.colorTable), 
    gridOffsetX(other.gridOffsetX), gridOffsetY(other.gridOffsetY),
    grid((Tile*) std::calloc(other.grid.num_elements(), sizeof(Tile)), 
        boost::extents[other.grid.shape()[0]][other.grid.shape()[1]]), 
    populatedXCoords(other.populatedXCoords), 
    populatedYCoords(other.populatedYCoords), 
    populatedCount(other.populatedCount)
{
    using boost::multi_array_types::index_range;
    auto populated = aux_tilesurface::calcPopulatedRect(populatedXCoords, populatedYCoords);
    
    grid[boost::indices
            [index_range(populated.left(), populated.right() + 1)]
            [index_range(populated.top(), populated.bottom() + 1)]
        ] 
        = other.grid[boost::indices
            [index_range(populated.left(), populated.right() + 1)]
            [index_range(populated.top(), populated.bottom() + 1)]
        ];
}

void aux_tilesurface::TileSurfaceData::clearGrid()
{
    assert(grid.empty() == !grid.data());
    if (grid.empty()) return;
    
    for (auto& tile : aux_multi_array_util::fast_traverse(grid))
        tile.~Tile();
    
    std::free(grid.data());

    grid.~grid_t();
    new (&grid) grid_t(nullptr, boost::extents[0][0]);
}

QPainter& TileSurface::TilePainter::iterator::dereference() const
{
    if (!_painter)
    {
        _painter.emplace(&*base());
        _painter->translate(-(*base()).offset());
    }
    
    assert(_painter->device() == &*base());
    return *_painter;
}

void TileSurface::squeeze()
{
//     assert(_populatedXCoords.empty() == _populatedYCoords.empty());
//     if (_populatedXCoords.empty())
//     {
//         clear();
//         return;
//     }
//     
    Q_UNIMPLEMENTED();
}

void TileSurface::clear()
{
    if (!_data || _data->grid.empty()) return;
    
    _data.detach();
    _data->clearGrid();
    
    _data->populatedXCoords.clear();
    _data->populatedYCoords.clear();
    _data->populatedCount = 0;
    
    _data->gridOffsetX = 0;
    _data->gridOffsetY = 0;
}

// void TileSurface::subtract(QRegion region)
// {
//     Q_UNIMPLEMENTED();
// }
// 
// void TileSurface::intersect(QRegion region)
// {
//     Q_UNIMPLEMENTED();
// }

const QList<aux_tilesurface::Tile> TileSurface::getTiles(QRegion overlap) const
{
    using boost::multi_array_types::index_range;
    
    if (Q_UNLIKELY(!_data || _data->grid.empty())) 
        return QList<Tile>();
    
    QList<Tile> result;
    auto gridRect = populatedRect().intersected(toGridRect(overlap.boundingRect()));
    
    auto tiles = _data->grid[boost::indices
            [index_range(gridRect.x() - _data->gridOffsetX, gridRect.x() + gridRect.width() - _data->gridOffsetX)]
            [index_range(gridRect.y() - _data->gridOffsetY, gridRect.y() + gridRect.height() - _data->gridOffsetY)]
        ];
    
    for (auto&& tile : aux_multi_array_util::traverse_rows(tiles))
    {
        auto p = gridPositionOf(&tile);
        
        if (tile.isNull()
            || !overlap.intersects(QRect(
                (p.x() + _data->gridOffsetX) * TILE_SIDE_LENGTH, 
                (p.y() + _data->gridOffsetY) * TILE_SIDE_LENGTH, 
                TILE_SIDE_LENGTH, TILE_SIDE_LENGTH
            )))
        {
            continue;
        }
    
        result.append(tile);
    }
    
    return result;
}

const QList<aux_tilesurface::Tile> TileSurface::getTiles(QPainterPath overlap, uint margin) const
{
    Q_UNIMPLEMENTED();
}

QList<aux_tilesurface::MutableTileRef> TileSurface::editTiles(QRect bound)
{
    Q_UNIMPLEMENTED();
}

QList<aux_tilesurface::MutableTileRef> TileSurface::editTiles(QRegion overlap)
{
    Q_UNIMPLEMENTED();
}

QList<aux_tilesurface::MutableTileRef> TileSurface::editTiles(QPainterPath overlap, uint margin)
{
    Q_UNIMPLEMENTED();
}

TileSurface::TilePainter TileSurface::paintTiles(QRect bound)
{
    using boost::multi_array_types::index_range;
    
    initData();
    reserveForEdit(bound);
    
    auto gridRect = toGridRect(bound);
    QList<QImage> result;
    result.reserve(gridRect.width() * gridRect.height());
    
    auto tiles = _data->grid[boost::indices
            [index_range(gridRect.left(), gridRect.right() + 1)]
            [index_range(gridRect.top(), gridRect.bottom() + 1)]
        ];

    for (auto&& tile : aux_multi_array_util::traverse_rows(tiles))
    {
        if (tile.isNull()) 
            populate(&tile);
        else
            tile._data.detach();
        
        result += tile;
    }
    
    return TilePainter(std::move(result));
}

TileSurface::TilePainter TileSurface::paintTiles(QRegion overlap)
{
    using boost::multi_array_types::index_range;
    
    initData();
    reserveForEdit(overlap.boundingRect());
    
    auto gridRect = toGridRect(overlap.boundingRect());
    QList<QImage> result;
    
    auto tiles = _data->grid[boost::indices
            [index_range(gridRect.left(), gridRect.right() + 1)]
            [index_range(gridRect.top(), gridRect.bottom() + 1)]
        ];

    for (auto&& tile : aux_multi_array_util::traverse_rows(tiles))
    {
        auto pos = gridPositionOf(&tile);
        QRect rect(
            (pos.x() - _data->gridOffsetX) * TILE_SIDE_LENGTH, 
            (pos.y() - _data->gridOffsetY) * TILE_SIDE_LENGTH, 
            TILE_SIDE_LENGTH, 
            TILE_SIDE_LENGTH);
        
        if (!overlap.intersects(rect)) continue;
        
        if (tile.isNull()) 
            populate(&tile);
        else
            tile._data.detach();
        
        result += tile;
    }
    
    return TilePainter(std::move(result));
}

TileSurface::TilePainter TileSurface::paintTiles(QPainterPath overlap, uint margin)
{
    using boost::multi_array_types::index_range;
    
    initData();
    QRect bound = coarseBoundRect(overlap.boundingRect())
        .adjusted(-margin, -margin, margin, margin);
    reserveForEdit(bound);
    
    auto gridRect = toGridRect(bound);
    QList<QImage> result;
    
    auto tiles = _data->grid[boost::indices
            [index_range(gridRect.left(), gridRect.right() + 1)]
            [index_range(gridRect.top(), gridRect.bottom() + 1)]
        ];

    for (auto&& tile : aux_multi_array_util::traverse_rows(tiles))
    {
        auto pos = gridPositionOf(&tile);
        QRect rect(
            (pos.x() - _data->gridOffsetX) * TILE_SIDE_LENGTH - margin, 
            (pos.y() - _data->gridOffsetY) * TILE_SIDE_LENGTH - margin, 
            TILE_SIDE_LENGTH + (2 * margin), 
            TILE_SIDE_LENGTH + (2 * margin));
        
        if (!overlap.intersects(rect)) continue;
        
        if (tile.isNull())
            populate(&tile);
        else
            tile._data.detach();
        
        result += tile;
    }
    
    return TilePainter(std::move(result));
}

// void TileSurface::merge(const TileSurface& source, 
//     QPainter::CompositionMode compositionMode,  TileSurface* diffOut)
// {
//     Q_UNIMPLEMENTED();
// }

void TileSurface::reserveForEdit(QRect editRect)
{    
    assert(_data);
    
    QRect gridRect = toGridRect(editRect);
    if (_data->grid.empty())
    {
        reserveGrid(gridRect.adjusted(
                -gridRect.width() / 2,
                -gridRect.height() / 2,
                gridRect.width() / 2,
                gridRect.height() / 2
            ));
    }
    else
    {
        QRect oldGridRect(0, 0, _data->grid.shape()[0], _data->grid.shape()[1]);
        
        if (!oldGridRect.contains(gridRect))
        {
            reserveGrid(gridRect.adjusted(
                    std::min(gridRect.left() - oldGridRect.left(), 0),
                    std::min(gridRect.top() - oldGridRect.top(), 0),
                    std::max(gridRect.right() - oldGridRect.right(), 0),
                    std::max(gridRect.bottom() - oldGridRect.bottom(), 0)
                ));
        }
    }
}

void TileSurface::reserveGrid(QRect gridArea)
{
    using boost::multi_array_types::index_range;
    using grid_t = data_t::grid_t;
    
    assert(_data);
    
    QRect oldRect(0, 0, _data->grid.shape()[0], _data->grid.shape()[1]);
    
    if (!gridArea.isValid() || oldRect.contains(gridArea)) return;
    
    if (_data->grid.empty())
    {
        uint count = gridArea.width() * gridArea.height();
        Tile* data = (Tile*) std::calloc(count, sizeof(Tile));
        
        _data->grid.~grid_t();
        new (&_data->grid) grid_t(data, boost::extents[gridArea.width()][gridArea.height()]);
                
        _data->gridOffsetX = gridArea.left();
        _data->gridOffsetY = gridArea.top();
    }
    else
    {
        QRect newRect = oldRect.united(gridArea);
        
        grid_t oldGrid(_data->grid);
        Tile* data = (Tile*) std::calloc(newRect.width() * newRect.height(), sizeof(QImage));
        
        _data->grid.~grid_t();
        new (&_data->grid) grid_t(data, boost::extents[newRect.width()][newRect.height()]);
        
        auto moveTo = _data->grid[boost::indices
                [index_range(-newRect.left(), -newRect.left() + oldRect.width())]
                [index_range(-newRect.top(), -newRect.top() + oldRect.height())]
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
                        oldRect.height() * sizeof(QImage));
            
            ++fromIt;
            ++toIt;
        }
        std::free(oldGrid.data());
        
        for (Tile& tile : aux_multi_array_util::traverse(moveTo))
        {
            auto pos = gridPositionOf(&tile);
            if (!tile.isNull())
                tile._data->offset = (pos + QPoint(_data->gridOffsetX, _data->gridOffsetY)) * (int)TILE_SIDE_LENGTH;
        }
            
//         // Initialize the rest of the reserved area to null QImage
//         
//         // all the area on the grid that is still uninitialized
//         const QRect initNullRects[4] = {
//                 // left
//                 QRect(0, 0, - newRect.left(), newRect.height()),
//                 // right
//                 QRect(oldRect.right() + 1 - newRect.left(), 0, newRect.right() - oldRect.right(), newRect.height()),
//                 // top middle
//                 QRect(oldRect.left() - newRect.left(), 0, oldRect.width(), oldRect.top() - newRect.top()),
//                 // bottom middle
//                 QRect(oldRect.left() - newRect.left(), oldRect.bottom() + 1 - newRect.top(), oldRect.width(), newRect.bottom() - oldRect.bottom()),
//             };
//             
//         for (const auto& initNullRect : initNullRects)
//         {
//             if (initNullRect.isEmpty()) continue;
//             
//             auto initNull = _data->grid[boost::indices
//                     [index_range(initNullRect.left(), initNullRect.right() + 1)]
//                     [index_range(initNullRect.top(), initNullRect.bottom() + 1)]
//                 ];
//                 
//             for (auto&& tile : aux_multi_array_util::traverse(initNull))
//                 new (&tile) QImage();
//         }
        
        _data->gridOffsetX += newRect.left();
        _data->gridOffsetY += newRect.top();
    }
}

void Addle::TileSurface::populate(Tile* tile)
{
    assert(_data && !_data->grid.empty());
    assert(tile - _data->grid.origin() >= 0 
        && tile - _data->grid.origin() < _data->grid.num_elements());
    assert(tile->isNull());
    
    auto pos = gridPositionOf(tile);
    
    uchar* buffer;
    
    std::size_t size = aux_tilesurface::calcRasterDataLength(_data->format);
    buffer = (uchar*) std::calloc(size, 1);
    
    auto offset = (pos + QPoint(_data->gridOffsetX, _data->gridOffsetY)) * (int)TILE_SIDE_LENGTH;
    
    (*tile) = Tile(buffer, offset, _data->format, _data->colorTable);
    
    incrementPopulated(pos.x(), pos.y());
}

void TileSurface::incrementPopulated(int gridX, int gridY)
{
    assert(noDetach(_data));
    auto aXIt = std::lower_bound(
            _data->populatedXCoords.begin(),
            _data->populatedXCoords.end(),
            gridX,
            [] (const auto& pair, int gridX_) -> bool { return pair.first < gridX_; }
        );
    
    if (aXIt != _data->populatedXCoords.cend() && (*aXIt).first == gridX)
        ++(*aXIt).second;
    else
        _data->populatedXCoords.insert(aXIt, { gridX, 1 });
    
    auto aYIt = std::lower_bound(
            _data->populatedYCoords.begin(),
            _data->populatedYCoords.end(),
            gridY,
            [] (const auto& pair, int gridY_) -> bool { return pair.first < gridY_; }
        );
    
    if (aYIt != _data->populatedYCoords.cend() && (*aYIt).first == gridY)
        ++(*aYIt).second;
    else
        _data->populatedYCoords.insert(aYIt, { gridY, 1 });
    
    ++(_data->populatedCount);
}

void TileSurface::decrementPopulated(int gridX, int gridY)
{
    assert(_data && _data->populatedCount);
    auto aXIt = std::lower_bound(
            _data->populatedXCoords.begin(),
            _data->populatedXCoords.end(),
            gridX,
            [] (const auto& pair, int gridX_) -> bool { return pair.first < gridX_; }
        );
    
    assert(aXIt != _data->populatedXCoords.cend() && (*aXIt).first == gridX);  
    
    --(*aXIt).second;
    if (!(*aXIt).second)
        _data->populatedXCoords.erase(aXIt);
    
    auto aYIt = std::lower_bound(
            _data->populatedYCoords.begin(),
            _data->populatedYCoords.end(),
            gridY,
            [] (const auto& pair, int gridY_) -> bool { return pair.first < gridY_; }
        );
    
    assert(aYIt != _data->populatedYCoords.cend() && (*aYIt).first == gridY);
    
    --(*aYIt).second;
    if (!(*aYIt).second)
        _data->populatedYCoords.erase(aYIt);
    
    --(_data->populatedCount);
}
