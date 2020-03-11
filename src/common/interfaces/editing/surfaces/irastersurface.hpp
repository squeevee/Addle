#ifndef IRASTERSURFACE_HPP
#define IRASTERSURFACE_HPP

#include "../icanpaintbuffer.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/metaobjectinfo.hpp"

class IRasterPreview;
class ILayer;
class IRasterSurface : public ICanPaintBuffer
{
public:
    virtual ~IRasterSurface() = default;

    virtual void render(QPainter& painter, QRect region) = 0;

signals:
    virtual void changed(QRect region) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IRasterSurface);
DECL_MAKEABLE(IRasterSurface);

#endif // IRASTERSURFACE_HPP