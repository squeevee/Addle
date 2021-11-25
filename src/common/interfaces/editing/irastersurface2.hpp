#pragma once

#include <boost/multi_array.hpp>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"
#include "interfaces/rendering/irenderable.hpp"

namespace Addle {

class RasterTile;                   // #include "utilities/image/rastertile.hpp"
class RasterSurfaceEditResult;      // #include "utilities/image/rasteredit.hpp"
class RasterTileEditHandle;         // #include "utilities/image/rastertileedithandle.hpp"
class RasterTilePaintHandle;        // #include "utilities/image/rastertileedithandle.hpp"
    
// Replacement for TileSurface, where TileSurface was already essentially a 
// replacement for IRasterSurface, which replaced one or two even older classes 
// I think -- it's been a while.
class IRasterSurface2 : public IRenderable, public virtual IAmQObject
{
protected:
    using grid_t = boost::multi_array_ref<RasterTile, 2>;
public:
    using tiles_view_t = grid_t::const_array_view<2>::type;
    
    virtual ~IRasterSurface2() = default;
    virtual IRasterSurface2* clone() const = 0;
    
    virtual void clear() = 0;
    
    virtual QRect boundingRect() const = 0;
    virtual QRegion region() const = 0;
    
    virtual QImage::Format format() const = 0;
    
    virtual tiles_view_t tiles() const = 0;
    
    virtual const RasterTile tileAt(QPoint p) const = 0;
    
    virtual tiles_view_t tilesAt(QRect rect) const = 0;
    virtual const QList<RasterTile> tilesAt(QRegion region) const = 0;
    virtual const QList<RasterTile> tilesAt(QPainterPath path, int margin = 2) const = 0;
    
    virtual RasterTileEditHandle editTiles(QRect rect) = 0;
    virtual RasterTileEditHandle editTiles(QRegion region) = 0;
    virtual RasterTileEditHandle editTiles(QPainterPath path, int margin = 2) = 0;
    
    virtual RasterTilePaintHandle paintTiles(QRect rect) = 0;
    virtual RasterTilePaintHandle paintTiles(QRegion region) = 0;
    virtual RasterTilePaintHandle paintTiles(QPainterPath path, int margin = 2) = 0;
    
    virtual QRect reserved() const = 0;
    virtual void reserve(QRect area) = 0;
    virtual void squeeze() = 0;
    
signals:
    virtual void regionChanged(QRegion region) = 0;
};

ADDLE_DECL_MAKEABLE(IRasterSurface2)
    
}

Q_DECLARE_INTERFACE(Addle::IRasterSurface2, "org.addle.IRasterSurface2")
