#pragma once

#include <QReadWriteLock>

#include "interfaces/editing/irastersurface2.hpp"
#include "utilities/image/rastertile.hpp"
#include "utilities/image/rastertileedithandle.hpp"

namespace Addle {
    
class RasterSurface2 : public QObject, public IRasterSurface2
{
    Q_OBJECT
    Q_INTERFACES(Addle::IRasterSurface2)
    IAMQOBJECT_IMPL
public:
    RasterSurface2();
    virtual ~RasterSurface2();
    
    IRasterSurface2* clone() const override;
    
    void clear() override
    {
        const QWriteLocker l(&_renderEntity->lock); 
        clear_p();
    }
    
    QRect boundingRect() const override
    {
        auto r = populatedGridRect();
        return r.isValid() ? toCanvasRect(r) : QRect();
    }

    QRegion region() const override { return _region; }
    
    QImage::Format format() const override { return _format; }
    
    tiles_view_t tiles() const override;
    
    const RasterTile tileAt(QPoint p) const override;
    
    tiles_view_t tilesAt(QRect rect) const override;
    const QList<RasterTile> tilesAt(QRegion region) const override;
    const QList<RasterTile> tilesAt(QPainterPath path, int margin = 2) const override;
    
    RasterTileEditHandle editTiles(QRect rect) override;
    RasterTileEditHandle editTiles(QRegion region) override;
    RasterTileEditHandle editTiles(QPainterPath path, int margin = 2) override;
    
    RasterTilePaintHandle paintTiles(QRect rect) override;
    RasterTilePaintHandle paintTiles(QRegion region) override;
    RasterTilePaintHandle paintTiles(QPainterPath path, int margin = 2) override;
    
    QRect reserved() const override
    {
        auto r = gridRect();
        return r.isValid() ? toCanvasRect(r) : QRect();
    }
    
    void reserve(QRect area) override
    { 
        const QWriteLocker l(&_renderEntity->lock); 
        reserve_p(area);
    }
    void squeeze() override;
    
    // IRenderable
    RenderRoutine renderRoutine() const override;
signals:
    void regionChanged(QRegion region) override;
    
    void renderRoutineChanged(RenderRoutineChangedEvent) override;
    void renderChanged(QRegion affected, DataTreeNodeAddress entity = {}) override;
    
private:
    struct RenderEntity : public aux_render::Entity
    {
        RenderEntity(RasterSurface2* surface);
        virtual ~RenderEntity() noexcept = default;
        
        const RasterSurface2* surface = nullptr;
        QRegion region;
        
        void draw_p(QPainter*, const QRegion&, bool*) const noexcept;
    };
    
    static constexpr auto TILE_SIDE_LENGTH = RasterTile::SIDE_LENGTH;
    static inline tiles_view_t nullTilesView();
    static inline QRect toGridRect(QRect r);
    static inline QRect toCanvasRect(QRect r);
    
    void clear_p();
    void reserve_p(QRect area);
    
    inline QRect gridRect() const { return QRect(_gridOffset, QSize(_grid.shape()[0], _grid.shape()[1])); }
    QRect populatedGridRect() const;
    
    QPoint gridPositionOfTile(const RasterTile*) const;
    
    void rebaseGrid(QRect newGrid);
        
    void incrementPopulation(QPoint p);
    void decrementPopulation(QPoint p);
    
    QImage::Format _format = QImage::Format_ARGB32;
    QVector<QRgb> _colorTable;
    
    QRegion _region;
    
    uint _populatedTileCount = 0;
    QVector<std::pair<int, uint>> _populatedTileCountByXCoord;
    QVector<std::pair<int, uint>> _populatedTileCountByYCoord;
    
    QPoint _gridOffset;
    grid_t _grid;
    
    QSharedPointer<RenderEntity> _renderEntity;
};

inline RasterSurface2::tiles_view_t RasterSurface2::nullTilesView()
{
    using boost::multi_array_types::index_range;
    return grid_t(nullptr, boost::extents[0][0])[
            boost::indices[index_range(0,0)][index_range(0,0)]
        ];
}

inline QRect RasterSurface2::toGridRect(QRect r)
{
    // TODO do this with integer math only?
    
    int gridX = std::floor((double)r.left() / TILE_SIDE_LENGTH);
    int gridY = std::floor((double)r.top() / TILE_SIDE_LENGTH);
    
    uint w = std::ceil((double)(r.right() + 1) / TILE_SIDE_LENGTH) - gridX;
    uint h = std::ceil((double)(r.bottom() + 1) / TILE_SIDE_LENGTH) - gridY;
    
    return QRect(gridX, gridY, w, h);
}

inline QRect RasterSurface2::toCanvasRect(QRect r)
{
    return QRect(r.topLeft() * (int)TILE_SIDE_LENGTH, r.size() * TILE_SIDE_LENGTH);
}

}
