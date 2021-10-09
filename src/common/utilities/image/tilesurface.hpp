#pragma once

#include <cmath>
#include <optional>

#include <QAtomicInteger>
#include <QtGlobal>
#include <QRegion>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QFuture>

#include <QReadWriteLock>

#include "globals.hpp"

#include <boost/multi_array.hpp>
#include "utilities/multi_array_util.hpp"

namespace Addle {
    
class TileSurface;
    
namespace aux_tilesurface {

constexpr unsigned int TILE_SIDE_LENGTH = 256;
constexpr QSize TILE_SIZE = QSize(TILE_SIDE_LENGTH, TILE_SIDE_LENGTH);
constexpr QImage::Format DEFAULT_FORMAT = QImage::Format_ARGB32;

Q_DECL_PURE_FUNCTION std::size_t calcRasterDataLength(QImage::Format format);

inline QRect calcPopulatedRect(
        const QVector<std::pair<int, uint>>& xCoords,
        const QVector<std::pair<int, uint>>& yCoords
    )
{
    assert(xCoords.isEmpty() == yCoords.isEmpty());
    if (xCoords.isEmpty())
    {
        return QRect();
    }
    else
    {
        int gridX = xCoords.front().first;
        int gridY = yCoords.front().first;
        int w = xCoords.back().first - gridX + 1;
        int h = yCoords.back().first - gridY + 1;
        
        return QRect(gridX, gridY, w, h);
    }
}

class MutableTileRef;
class Tile 
{
public:
    Tile() = default;
    Tile(const Tile&) = default;
    Tile(Tile&&) = default;
    
    ~Tile() = default;
    
    Tile& operator=(const Tile&) = default;
    Tile& operator=(Tile&&) = default;
    
    QImage::Format format() const { return _data ? _data->format : (QImage::Format)(NULL); }
    QVector<QRgb> colorTable() const { return _data ? _data->colorTable : QVector<QRgb>(); }
    
    bool isNull() const { return !_data || !(_data->rasterData); }
    
    // NOTE a null tile always gives an `offset` of (0,0) even if it is within
    // the boundingRect of a TileSurface
    QPoint offset() const { return _data ? _data->offset : QPoint(); }
    int x() const { return _data ? _data->offset.x() : 0; }
    int y() const { return _data ? _data->offset.y() : 0; }
    
    QRect rect() const { return _data ? QRect(_data->offset, TILE_SIZE) : QRect(); }
    
    QImage asImage()
    { 
        if (!_data || !_data->rasterData)
            return QImage();
        
        _data.detach();
        _data->detachRasterData();
        return _data->getImage(); 
    }
    
    const QImage asImage() const
    { 
        if (!_data || !_data->rasterData)
            return QImage();
        return _data->getImage(); 
    }
    
    operator QImage() { return asImage(); }
    operator const QImage() const { return asImage(); }
    
    std::size_t rasterDataLength() const { return _data ? calcRasterDataLength(_data->format) : 0; }
    
    uchar* rasterData()
    { 
        _data.detach();
        _data->detachRasterData();
        return _data ? _data->rasterData : nullptr; 
    }
    
    const uchar* rasterData() const { return _data ? _data->rasterData : nullptr; }
    
    QReadWriteLock& lock() const { assert(_data); return _data->lock; }
    
private:
    struct TileData : QSharedData
    {
        inline TileData(uchar* rasterData_, QPoint offset_, 
            QImage::Format format_, QVector<QRgb> colorTable_)
            : rasterData(rasterData_), offset(offset_), format(format_), 
            colorTable(colorTable_)
        {
            if (rasterData)
                rasterDataRefCount = new QAtomicInteger<unsigned>(1);
        }
        
        TileData(const TileData&);
        
        ~TileData();
        
        QImage getImage() const;
        void detachRasterData();
        void disposeRasterData();
    
        uchar* rasterData = nullptr;        
        QAtomicInteger<unsigned>* rasterDataRefCount = nullptr;
        
        QPoint offset;
        QImage::Format format;
        QVector<QRgb> colorTable;
        
        mutable QReadWriteLock lock;
    };
    
    Tile(uchar* rasterData, QPoint offset, QImage::Format format, 
         QVector<QRgb> colorTable)
        : _data(new TileData(rasterData, offset, format, colorTable))
    {
    }
    
    static void cleanupImage(void* data);
    
    QSharedDataPointer<TileData> _data;
    
    friend class Addle::TileSurface;
    friend class MutableTileRef;
};

struct TileSurfaceData : QSharedData
{
    using grid_t = boost::multi_array_ref<Tile, 2>;
    
    TileSurfaceData(QImage::Format format_ = DEFAULT_FORMAT)
        : format(format_), grid(nullptr, boost::extents[0][0])
    {
    }
    
    TileSurfaceData(const TileSurfaceData&);
    ~TileSurfaceData() { clearGrid(); }
    
    void clearGrid();
    
    QImage::Format format;
    QVector<QRgb> colorTable;
    
    int gridOffsetX = 0;
    int gridOffsetY = 0;
    
    // If the "absolute grid" is one where (0,0) corresponds to the tile whose
    // top-left corner is (0,0) on the canvas and all other coordinates map to
    // the canvas by the scaling factor of TILE_WIDTH, these values represent
    // the offset from the indices of _grid to the absolute grid
    grid_t grid;
    
    // Ordered vectors of populated tile counts per grid x/y coordinate
    // respectively, not including any zeroes. Used to quickly find the
    // minimally sufficient bounding rectangle of allocated tiles, maintained by
    // incrementPopulated and decrementPopulated
    QVector<std::pair<int, uint>> populatedXCoords;
    QVector<std::pair<int, uint>> populatedYCoords;
    std::size_t populatedCount = 0;
};

// Mutable uncounted reference to a Tile (allows the data of a tile to be edited 
// without detaching it, and without exposing `Tile::operator=`)
class MutableTileRef
{
    using data_t = Tile::TileData;
public:
    MutableTileRef() = default;
    MutableTileRef(const MutableTileRef&) = default;
    MutableTileRef(MutableTileRef&&) = default;
    
    ~MutableTileRef() = default;
    
    MutableTileRef& operator=(const MutableTileRef&) = default;
    MutableTileRef& operator=(MutableTileRef&&) = default;
    
    bool isNull() const { return !_data || !(_data->rasterData); }
    
    QImage::Format format() const { return _data ? _data->format : (QImage::Format)(NULL); }
    QVector<QRgb> colorTable() const { return _data ? _data->colorTable : QVector<QRgb>(); }
    
    QPoint offset() const { return _data ? _data->offset : QPoint(); }
    int x() const { return _data ? _data->offset.x() : 0; }
    int y() const { return _data ? _data->offset.y() : 0; }
    
    QImage asImage() const { return _data ? _data->getImage() : QImage(); }
    operator QImage() const { return asImage(); }
    
    std::size_t rasterDataLength() const { return _data ? calcRasterDataLength(_data->format) : 0; }
    uchar* rasterData() { return _data ? _data->rasterData : nullptr; }
    
    QReadWriteLock& lock() const { assert(_data); return _data->lock; }
    
private:
    data_t* _data = nullptr;
};

struct ModifyResult;

}

/**
 * A container for raster image data, modeled as a size-unbounded two-
 * dimensional grid of tiles. Each tile has the same format and size, is 
 * allocated lazily by the TileSurface as needed for write operations, and is 
 * managed as shared memory. A tile's `asImage` function (or QImage conversion 
 * operator) creates a reference to the tile's raster data as a  QImage, (or a 
 * null QImage if the tile is null).
 * 
 * A tile may be populated, meaning it has raster data, or it may be null. 
 * `boundingRect()` gives the rectangle encompassing all populated tiles
 * in the surface, and `populated()` gives the exact region covered by populated 
 * tiles. 
 * 
 * TileSurface accessors can accept a QRect, QRegion, or QPainterPath, to select 
 * tiles covering a specific area. For write accessors, the TileSurface will
 * populate or detach tiles as necessary so that write operations within the
 * given area are valid. 
 * 
 * Read accessors may only expose a subset of selected tiles if not all tiles in
 * the selected area are populated. For example tiles() and the getTiles
 * overload accepting a QRect will only expose tiles within boundingRect, and 
 * the getTiles overloads accepting a QRegion or QPainterPath will only expose
 * populated tiles. 
 * 
 * All coordinates (i.e., as QPoint, QRect, QRegion, QPainterPath, or 
 * QTransform) given to or exposed by the TileBuffer are relative to the canvas. 
 * The tiles_view_t object returned by tiles() and getTiles(QRect) is a Boost 
 * multi-array view withindices that are proportional to canvas  coordinates, 
 * i.e., ascending values of the first index refer to tiles moving from left to 
 * right and ascending values of the second index refer to tiles moving from top 
 * to bottom. The mapping of indices to coordinates is otherwise not specified. 
 * The order of indices of other tile collections exposed by accessors are not 
 * specified.
 * 
 * `reserved()` gives the rectangle of all tiles held internally by the surface.
 * This is at minimum the same size as boundingRect, but is typically larger to
 * allow the surface to grow more easily. `reserve()` and `squeeze()` can be 
 * used to manage reserved space, similar to an container like std::vector.
 */

// TODO: compression -- if the raster data of a tile buffer does not need to be
// accessed quickly, it could be useful to conserve memory by compressing all or
// part of it. This would be useful for e.g., Undo/Redo states. The compression
// could be done as a simple LZW applied directly to the raster buffer of each 
// tile -- in practice, raster data often contains long runs of zeros and other
// dictionary-friendly patterns -- or, there could be a reason to serialize an
// undo history in which case we may want to export the tiles into PNG objects.
//
// TODO: some operations: 
// transform (optimize for transpose and integer translation), area extract, 
// area duplicate
// 
// TODO: QFuture for parallel operations on tiles -- editTiles and paintTiles 
// return random-access collections so those can already be used with
// QtConcurrent but operations like merge and transform should expose QFuture

// TODO rename to RasterSurface after other RasterSurface has been removed
class TileSurface
{
    using data_t = aux_tilesurface::TileSurfaceData;
    
public:
    static constexpr unsigned int TILE_SIDE_LENGTH  = aux_tilesurface::TILE_SIDE_LENGTH;
    static constexpr unsigned int DEFAULT_FORMAT    = aux_tilesurface::DEFAULT_FORMAT;

    using Tile              = aux_tilesurface::Tile;
    using MutableTileRef    = aux_tilesurface::MutableTileRef;
    
    // Iterable that provides a QPainter for each tile in a set of tiles, e.g., 
    // that was selected by a TileBuffer accessor. The QPainter is automatically
    // adjusted to the tile's offset so all paint operations can implicitly be 
    // done in canvas coordinates.
    class TilePainter
    {
        using tiles_t = QList<QImage>;
    public:
        class iterator : public boost::iterator_adaptor<
                iterator,
                tiles_t::iterator,
                QPainter&
            >
        {
            using adaptor_t = iterator_adaptor<
                    iterator,
                    tiles_t::iterator,
                    QPainter&
                >;
        public:
            iterator() = default;
            iterator(const iterator& other)
            {
            }
            
        private:
            iterator(tiles_t::iterator i)
                : adaptor_t(i)
            {
            }
            
            QPainter& dereference() const;
            void increment() { _painter.reset(); ++base_reference(); }
            void decrement() { _painter.reset(); --base_reference(); }
            void advance(std::ptrdiff_t n) { _painter.reset(); base_reference() += n; }
            
            mutable std::optional<QPainter> _painter;
            
            friend class TilePainter;
            friend class boost::iterator_core_access;
        };
        
        TilePainter() = delete;
        TilePainter(const TilePainter&) = delete;
        TilePainter(TilePainter&& other)
            : _tiles(std::move(other._tiles))
        {
        }
        
        iterator begin() { return iterator(_tiles.begin()); }
        iterator end() { return iterator(_tiles.end()); }
        
    private:
        explicit TilePainter(tiles_t tiles)
            : _tiles(std::move(tiles))
        {
        }
        
        tiles_t _tiles;
        
        friend class TileSurface;
    };
    
    using tiles_view_t = data_t::grid_t::const_array_view<2>::type;
    using modify_result_t       = aux_tilesurface::ModifyResult;
    
    // Controls the behavior of generating a differencing image when modifying a
    // tile surface
    enum DiffOption {
        // No differencing image is generated
        DiffOption_None     = 0x00,
        
        // A differencing image will be generated except when performing a
        // reversible transform
        DiffOption_Relaxed  = 0x01
        
    //     // A differencing image is generated even when performing a reversible
    //     // transform
    //     , DiffOption_Force    = 0x03

    //     // The tiles of the generated differencing image will be compressed 
    //     , DiffOption_Compress = 0x05
    };
    Q_DECLARE_FLAGS(DiffOptions, DiffOption);
    
    TileSurface() = default;
    TileSurface(const TileSurface& other) = default;
    TileSurface(TileSurface&&) = default;
    
    TileSurface(QImage::Format format)
        : _data(new data_t(format))
    {
    }

    ~TileSurface() = default;
    
    QImage::Format format() const { return _data ? _data->format : (QImage::Format)(NULL); }

    bool isEmpty() const
    {
        return !_data || !(_data->populatedCount);
    }
    
    QRegion populated() const
    {
        QRegion result;
        
        if (Q_UNLIKELY(!_data)) return QRegion();
        
        for (auto&& tile : aux_multi_array_util::traverse(tiles()))
        {
            if (tile.isNull()) continue;
            
            auto pos = gridPositionOf(&tile);
            result += tile.rect();
        }
        
        return result;
    }
    
    QRect boundingRect() const
    {
        if (Q_UNLIKELY(!_data)) return QRect();
        
        auto alloc = populatedRect();
        return QRect(
                alloc.x() * TILE_SIDE_LENGTH,
                alloc.y() * TILE_SIDE_LENGTH,
                alloc.width() * TILE_SIDE_LENGTH,
                alloc.height() * TILE_SIDE_LENGTH
            );
    }
    
    QRect reserved() const 
    {
        if (Q_UNLIKELY(!_data)) return QRect();
        
        return QRect(
            _data->gridOffsetX * TILE_SIDE_LENGTH,
            _data->gridOffsetY * TILE_SIDE_LENGTH,
            _data->grid.shape()[0] * TILE_SIDE_LENGTH,
            _data->grid.shape()[1] * TILE_SIDE_LENGTH
        );
    }
    
    void reserve(QRect area) 
    { 
        if (!_data)
            initData();
        
        reserveGrid(toGridRect(area)); 
    }
    
    void squeeze();
    
    void clear();
    
    const Tile tileAt(int x, int y) const
    {
        if (Q_UNLIKELY(!_data)) return Tile();
        
        int gridX = (x / TILE_SIDE_LENGTH) - _data->gridOffsetX;
        int gridY = (y / TILE_SIDE_LENGTH) - _data->gridOffsetY;
        
        if (gridX >= 0 && gridX < _data->grid.shape()[0] 
            && gridY >= 0 && gridY < _data->grid.shape()[1])
        {
            return _data->grid[gridX][gridY];
        }
        else
        {
            return Tile();
        }
    }
    
    const Tile tileAt(QPoint p) const { return tileAt(p.x(), p.y()); }
    
    // tiles and getTiles return a two-dimensional array view. The indices of
    // this array view may be treated as relative positional coordinates. All 
    // indices are non-negative integers, the first index selects horizontal 
    // positions and the second index selects vertical positions, and the origin
    // corresponds to the top left corner of array view. However, beyond this
    // the coordinate system of an array veiw is not specified, including the
    // mapping to canvas positions or to another array view from any TileBuffer.
    //
    // If the tile at a position is not null, then its offset property will be
    // set to the position of its top-left corner in canvas coordinates.
    
    tiles_view_t tiles() const
    {
        using boost::multi_array_types::index_range;
        
        if (!_data) return nullTilesView();
        
        auto gridRect = populatedRect();
        return _data->grid[boost::indices
                [index_range(gridRect.left(), gridRect.right() + 1)]
                [index_range(gridRect.top(), gridRect.bottom() + 1)]
            ];
        
    }
    
    tiles_view_t getTiles(QRect bound) const
    {
        using boost::multi_array_types::index_range;
        
        if (!_data) return nullTilesView();
        
        auto gridRect = toGridRect(bound).intersected(populatedRect());
        return _data->grid[boost::indices
                [index_range(gridRect.left(), gridRect.right() + 1)]
                [index_range(gridRect.top(), gridRect.bottom() + 1)]
            ];
    }
    
    const QList<Tile> getTiles(QRegion overlap) const;
    
    // Gets all existing tiles in the buffer that intersect with path `overlap`.
    // For the purposes of calculating intersections, each tile is considered to
    // include the area extended in each direction by `margin`. This allows us
    // to query for tiles that technically do not intersect with `overlap` but
    // may e.g., be affected by antialiasing.
    const QList<Tile> getTiles(QPainterPath overlap, uint margin = 2) const;
    
    QList<MutableTileRef> editTiles(QRect bound);
    QList<MutableTileRef> editTiles(QRegion overlap);
    QList<MutableTileRef> editTiles(QPainterPath overlap, uint margin = 2);
    
    TilePainter paintTiles(QRect bound);
    TilePainter paintTiles(QRegion overlap);
    TilePainter paintTiles(QPainterPath overlap, uint margin = 2);
    
    modify_result_t subtract(QRegion region,
            DiffOptions diffOptions = DiffOption_Relaxed);
    
    modify_result_t intersect(QRegion region,
            DiffOptions diffOptions = DiffOption_Relaxed);
        
    modify_result_t transform(QTransform transform,
            DiffOptions diffOptions = DiffOption_Relaxed);
    
    modify_result_t merge(const TileSurface& source, 
            QPainter::CompositionMode compositionMode = QPainter::CompositionMode_SourceOver,
            bool opacity = 1.0,
            QRegion clippingRegion = QRegion(),
            DiffOptions diffOptions = DiffOption_Relaxed);
    
    modify_result_t revert(const modify_result_t& previousModification,
            DiffOptions diffOptions = DiffOption_Relaxed);
        
    // Gives the region of tiles that are *fully covered* by the given region, 
    // i.e., any partially covered tiles are excluded from the returned region.
    // This region describes theoretical tiles on any TileBuffer, independent of
    // any buffer instance or of which tiles are populated.
    static QRegion conformTight(QRegion region)
    {
        QRegion result;
        auto bound = region.boundingRect();
        
        int boundGridX = std::floor((double)bound.left() / TILE_SIDE_LENGTH);
        int boundGridY = std::floor((double)bound.top() / TILE_SIDE_LENGTH);
        
        uint w = std::ceil((double)(bound.right() + 1) / TILE_SIDE_LENGTH);
        uint h = std::ceil((double)(bound.bottom() + 1) / TILE_SIDE_LENGTH);
        
        for (int y = boundGridY; y - boundGridY < h; ++y)
        {
            for (int x = boundGridX; x - boundGridX < w; ++x)
            {
                QRect rect(x * TILE_SIDE_LENGTH, y * TILE_SIDE_LENGTH, TILE_SIDE_LENGTH, TILE_SIDE_LENGTH);
                if (region.contains(rect)) result += rect;
            }
        }
        
        return result;
    }
    
    // Gives the region of tiles that are fully or partially covered by the
    // given region, i.e., only completely uncovered tiles are exlcuded from the 
    // returned region. This region describes theoretical tiles on any 
    // TileBuffer, independent of any buffer instance or of which tiles are
    // populated.
    static QRegion conformLoose(QRegion region)
    {
        QRegion result;
        auto bound = region.boundingRect();
        
        int boundGridX = std::floor((double)bound.left() / TILE_SIDE_LENGTH);
        int boundGridY = std::floor((double)bound.top() / TILE_SIDE_LENGTH);
        
        uint w = std::ceil((double)(bound.right() + 1) / TILE_SIDE_LENGTH);
        uint h = std::ceil((double)(bound.bottom() + 1) / TILE_SIDE_LENGTH);
        
        for (int y = boundGridY; y - boundGridY < h; ++y)
        {
            for (int x = boundGridX; x - boundGridX < w; ++x)
            {
                QRect rect(x * TILE_SIDE_LENGTH, y * TILE_SIDE_LENGTH, TILE_SIDE_LENGTH, TILE_SIDE_LENGTH);
                if (region.intersects(rect)) result += rect;
            }
        }
        
        return result;
    }
    
private:
    static tiles_view_t nullTilesView()
    {
        using boost::multi_array_types::index_range;
        return data_t::grid_t(nullptr, boost::extents[0][0])[
                boost::indices[index_range(0,0)][index_range(0,0)]
            ];
    }
    
    QRect toGridRect(QRect r) const // static?
    {
        assert(_data);
        // TODO do this with integer math only?
        
        int gridX = std::floor((double)r.left() / TILE_SIDE_LENGTH);
        int gridY = std::floor((double)r.top() / TILE_SIDE_LENGTH);
        
        uint w = std::ceil((double)(r.right() + 1) / TILE_SIDE_LENGTH) - gridX;
        uint h = std::ceil((double)(r.bottom() + 1) / TILE_SIDE_LENGTH) - gridY;
        
        return QRect(gridX - _data->gridOffsetX, gridY - _data->gridOffsetY, w, h);
    }
    
    void initData()
    {
        if (_data)
            _data.detach();
        else
            _data = new data_t;
    }
    
    // Gives a rectangle in grid coordinates of tiles that are currently
    // populated
    QRect populatedRect() const
    { 
        assert(_data); 
        return aux_tilesurface::calcPopulatedRect(_data->populatedXCoords, _data->populatedYCoords); 
    }
    
    QPoint gridPositionOf(const Tile* tiles) const
    {
        assert(_data);
        return QPoint(
                (tiles - _data->grid.origin()) / _data->grid.strides()[0],
                (tiles - _data->grid.origin()) % _data->grid.strides()[0]
            );
    }
    
    // Ensures that the grid contains tiles covering `rect` for editing (where
    // `rect` is in canvas coordinates). If the grid is not large enough, this 
    // causes it to be rebuilt with *extra* space.
    void reserveForEdit(QRect rect);
    
    // Ensures that the grid contains tiles covering `rect` (where `rect` is in
    // grid coordinates). If the grid is not large enough, this rebuilds it to
    // exactly the united rectangle of its previous state and the new rectangle.
    void reserveGrid(QRect gridRect);
    
    void populate(Tile* tile);
    
    void incrementPopulated(int gridX, int gridY);
    void decrementPopulated(int gridX, int gridY);
    
    QExplicitlySharedDataPointer<data_t> _data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TileSurface::DiffOptions);

namespace aux_tilesurface { 
    
struct ModifyResult {
    
    QFuture<QRegion> async;
    
    QRegion totalRegion;
    
    // Region of the canvas 
    QRegion addedRegion;
    QTransform reverseTransform;
    
    const TileSurface diff;
}; 
    
}

}
