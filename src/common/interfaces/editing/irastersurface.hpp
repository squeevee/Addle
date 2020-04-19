#ifndef IRASTERSURFACE_HPP
#define IRASTERSURFACE_HPP

#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/metaobjectinfo.hpp"

#include "utilities/image/rasterpainthandle.hpp"
#include "utilities/image/rasterbithandles.hpp"

#include <QPainter>
#include <QImage>

//class IRasterPreview;
class IRenderStep;

class IRasterDiff;

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

    //virtual void render(QPainter& painter, QRect area) const = 0;

    virtual void clear() = 0;

    virtual QSharedPointer<IRenderStep> getRenderStep() = 0;

    virtual RasterPaintHandle getPaintHandle(QRect area) = 0;

    virtual RasterBitReader getBitReader(QRect area) const = 0;
    virtual RasterBitWriter getBitWriter(QRect area) = 0;

signals:
    virtual void changed(QRect area) = 0;

protected:
    inline RasterPaintHandle getPaintHandle_p(QImage& buffer, QPoint bufferOffset, QRect area)
    {
        return RasterPaintHandle(*this, buffer, bufferOffset, area);
    }

    virtual void onPaintHandleDestroyed(const RasterPaintHandle& handle) = 0;

    inline RasterBitReader getBitReader_p(const QImage& buffer, QPoint bufferOffset, QRect area) const
    {
        return RasterBitReader(*this, buffer, bufferOffset, area);
    }

    virtual void onBitReaderDestroyed(const RasterBitReader& handle) const = 0;

    inline RasterBitWriter getBitWriter_p(QImage& buffer, QPoint bufferOffset, QRect area)
    {
        return RasterBitWriter(*this, buffer, bufferOffset, area);
    }

    virtual void onBitWriterDestroyed(const RasterBitWriter& handle) = 0;

    friend class RasterPaintHandle;
    friend class RasterBitReader;
    friend class RasterBitWriter;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IRasterSurface::InitFlags)

DECL_EXPECTS_INITIALIZE(IRasterSurface);
DECL_IMPLEMENTED_AS_QOBJECT(IRasterSurface);
DECL_MAKEABLE(IRasterSurface);

#endif // IRASTERSURFACE_HPP