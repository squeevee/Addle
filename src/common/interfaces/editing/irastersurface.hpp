/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IRASTERSURFACE_HPP
#define IRASTERSURFACE_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

#include "interfaces/metaobjectinfo.hpp"

#include "utilities/image/rasterpainthandle.hpp"
#include "utilities/image/rasterbithandles.hpp"

#include <QPainter>
#include <QImage>
namespace Addle {

//class IRasterPreview;
class IRenderStep;

class IRasterDiff;

/**
 * An "infinite" surface containing raster data. Paint operations must use
 * RasterPaintHandle to request the area on which they will operate.
 */
class IRasterSurface : public virtual IAmQObject
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
    virtual QPainter::CompositionMode compositionMode() const = 0;
    
    virtual bool replaceMode() const = 0;
    virtual void setReplaceMode(bool replace) = 0;

    virtual void link(QSharedPointer<IRasterSurface> other) = 0;
    virtual void unlink() = 0;

    virtual QRect area() const = 0;

    //virtual void render(QPainter& painter, QRect area) const = 0;

    virtual void clear() = 0;

    virtual int alpha() const = 0;
    virtual void setAlpha(int alpha) = 0;

    virtual QSharedPointer<IRenderStep> renderStep() = 0;

    virtual RasterPaintHandle paintHandle(QRect area) = 0;

    virtual RasterBitReader bitReader(QRect area) const = 0;
    virtual RasterBitWriter bitWriter(QRect area) = 0;

signals:
    virtual void changed(QRect area) = 0;

protected:
    inline RasterPaintHandle paintHandle_p(QImage& buffer, QPoint bufferOffset, QRect area)
    {
        return RasterPaintHandle(*this, buffer, bufferOffset, area);
    }

    virtual void onPaintHandleDestroyed(const RasterPaintHandle& handle) = 0;

    inline RasterBitReader bitReader_p(const QImage& buffer, QPoint bufferOffset, QRect area) const
    {
        return RasterBitReader(*this, buffer, bufferOffset, area);
    }

    virtual void onBitReaderDestroyed(const RasterBitReader& handle) const = 0;

    inline RasterBitWriter bitWriter_p(QImage& buffer, QPoint bufferOffset, QRect area)
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
DECL_MAKEABLE(IRasterSurface);

} // namespace Addle
#endif // IRASTERSURFACE_HPP