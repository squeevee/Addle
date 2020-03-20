#ifndef IRASTERSURFACE_HPP
#define IRASTERSURFACE_HPP

#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/metaobjectinfo.hpp"

#include "utilities/image/rasterpainthandle.hpp"

#include <QImage>

class IRasterPreview;
class IRasterSurface
{
public:
    enum InitFlag
    {
        None = 0x0,
        
    };
    Q_DECLARE_FLAGS(InitFlags, InitFlag);

    virtual ~IRasterSurface() = default;

    virtual void initialize(
        QImage::Format format = QImage::Format_ARGB32_Premultiplied,
        QRect area = QRect(),
        InitFlags flags = None
    ) = 0;
    virtual void initialize(
        QImage image,
        InitFlags flags = None
    ) = 0;

    virtual QRect getArea() const = 0;
    virtual QSize getSize() const = 0;

    virtual QImage copy(QRect area, QPoint* offset = nullptr) const = 0;

    virtual void render(QPainter& painter, QRect area) const = 0;

    virtual RasterPaintHandle getPaintHandle(QRect area) = 0;

signals:
    virtual void changed(QRect area) = 0;

protected:
    virtual void allocate(QRect area) = 0;

    friend class RasterPaintHandle;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IRasterSurface::InitFlags)

DECL_EXPECTS_INITIALIZE(IRasterSurface);
DECL_IMPLEMENTED_AS_QOBJECT(IRasterSurface);
DECL_MAKEABLE(IRasterSurface);

#endif // IRASTERSURFACE_HPP