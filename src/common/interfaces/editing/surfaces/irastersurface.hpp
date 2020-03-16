#ifndef IRASTERSURFACE_HPP
#define IRASTERSURFACE_HPP

#include "../icanpaintbuffer.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/metaobjectinfo.hpp"

#include <QImage>

class IRasterPreview;
class ILayer;
class IRasterSurface : public ICanPaintBuffer
{
public:
    virtual ~IRasterSurface() = default;

    virtual void initialize() = 0;
    virtual void initialize(QImage image) = 0;

    virtual void render(QPainter& painter, QRect region) = 0;

signals:
    virtual void changed(QRect region) = 0;
};

DECL_EXPECTS_INITIALIZE(IRasterSurface);
DECL_IMPLEMENTED_AS_QOBJECT(IRasterSurface);
DECL_MAKEABLE(IRasterSurface);

#endif // IRASTERSURFACE_HPP