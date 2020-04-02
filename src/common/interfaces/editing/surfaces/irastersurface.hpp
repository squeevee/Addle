#ifndef IRASTERSURFACE_HPP
#define IRASTERSURFACE_HPP

#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/metaobjectinfo.hpp"

#include "utilities/image/rasterpainthandle.hpp"

#include <QImage>

//class IRasterPreview;

/**
 * An "infinite" surface containing raster data. Paint operations must use
 * RasterPaintHandle to request the area on which they will operate.
 */
class IRasterSurface
{
public:
    enum InitFlag
    {
        // These may be used by makers of RasterSurface to indicate that special
        // behaviors or optimizations are desired.

        None = 0x0,

        //AlphaChannelOnly = 0x10
        // ARGB32-premultiplied is the only format supported by the Raster
        // QPainter backend for "high performance" that has an alpha channel.
        // Sparing the extra memory will be appropriate in some places, but I 
        // suspect it will not be here.
    };
    Q_DECLARE_FLAGS(InitFlags, InitFlag);

    virtual ~IRasterSurface() = default;

    virtual void initialize(
        QRect area = QRect(),
        InitFlags flags = None
    ) = 0;
    virtual void initialize(
        QImage image,
        InitFlags flags = None
    ) = 0;

    virtual QRect getArea() const = 0;
    virtual QSize getSize() const = 0;

    virtual QImage copy(QRect area = QRect(), QPoint* offset = nullptr) const = 0;

    virtual void render(QPainter& painter, QRect area) const = 0;

    virtual RasterPaintHandle getPaintHandle(QRect area) = 0;
    virtual void merge(IRasterSurface& other) = 0;

signals:
    virtual void changed(QRect area) = 0;

protected:
    inline RasterPaintHandle getPaintHandle_p(QImage& buffer, QPoint bufferOffset, QRect area)
    {
        return RasterPaintHandle(*this, buffer, bufferOffset, area);
    }

    virtual void onHandleDestroyed(const RasterPaintHandle& handle) = 0;

    friend class RasterPaintHandle;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IRasterSurface::InitFlags)

DECL_EXPECTS_INITIALIZE(IRasterSurface);
DECL_IMPLEMENTED_AS_QOBJECT(IRasterSurface);
DECL_MAKEABLE(IRasterSurface);

#endif // IRASTERSURFACE_HPP