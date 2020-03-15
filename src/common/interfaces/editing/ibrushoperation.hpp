#ifndef IBRUSHOPERATION_HPP
#define IBRUSHOPERATION_HPP

#include <QImage>
#include <QPainter>
#include <QBrush>
#include <QWeakPointer>

#include "interfaces/models/ilayer.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"

#include "utilities/canvas/brushpathsegment.hpp"
#include "brushrenderers/ibrushrenderer.hpp"

#include "irasteroperation.hpp"

class IBrushOperation : public IRasterOperation
{
public:

    enum Mode 
    {
        // In paint mode, the buffer supports ARGB color space and will be
        // merged with the layer using SourceOver (normal) composition mode
        paint,

        // In erase mode, the buffer supports alpha channel only, and will be
        // merged with the layer using DestinationOut (?) composition mode.
        // This means that opaque pixels on the buffer will become transparent
        // pixels on the layer.
        erase
    };

    virtual ~IBrushOperation() = default;

    virtual void initialize(
        QWeakPointer<ILayer>& layer,
        QSharedPointer<IBrushRenderer> brushRenderer,
        Mode mode = Mode::paint
    ) = 0;

    virtual void addPathSegment(const BrushPathSegment& pathSegment) = 0;
};

DECL_MAKEABLE(IBrushOperation)
DECL_EXPECTS_INITIALIZE(IBrushOperation)
DECL_IMPLEMENTED_AS_QOBJECT(IBrushOperation)

#endif // IBRUSHOPERATION_HPP