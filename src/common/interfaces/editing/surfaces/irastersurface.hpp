#ifndef IRASTERSURFACE_HPP
#define IRASTERSURFACE_HPP

#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/metaobjectinfo.hpp"

#include "utilities/image/rasterpainthandle.hpp"

#include <QPainter>
#include <QImage>

//class IRasterPreview;
class IRenderStep;

/**
 * An "infinite" surface containing raster data. Paint operations must use
 * RasterPaintHandle to request the area on which they will operate.
 */
class IRasterSurface
{
public:
    // These may be used by makers of RasterSurface to indicate that special
    // behaviors or optimizations are desired.
    enum InitFlag
    {
        None = 0x0,



        CopyFromLinked = 0x20,

    };
    Q_DECLARE_FLAGS(InitFlags, InitFlag);

    virtual ~IRasterSurface() = default;

    virtual void initialize(
        QRect area = QRect(),
        QPainter::CompositionMode compositionMode = QPainter::CompositionMode_SourceOver,
        InitFlags flags = None
    ) = 0;
    virtual void initialize(
        QImage image,
        QPoint offset = QPoint(),
        QPainter::CompositionMode compositionMode = QPainter::CompositionMode_SourceOver,
        InitFlags flags = None
    ) = 0;

    virtual void setCompositionMode(QPainter::CompositionMode mode) = 0;
    virtual QPainter::CompositionMode getCompositionMode() const = 0;

    virtual void link(QSharedPointer<IRasterSurface> other) = 0;
    virtual void unlink() = 0;

    virtual QRect getArea() const = 0;
    virtual QSize getSize() const = 0;

    virtual QImage copy(QPoint* offset = nullptr, QRect area = QRect()) const = 0;

    //virtual void render(QPainter& painter, QRect area) const = 0;

    virtual void clear() = 0;

    virtual QSharedPointer<IRenderStep> getRenderStep() = 0;

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