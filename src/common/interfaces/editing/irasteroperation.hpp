#ifndef IRASTEROPERATION_HPP
#define IRASTEROPERATION_HPP

#include <QImage>
#include <QPainter>
#include <QBrush>
#include <QWeakPointer>

#include <memory>

#include "interfaces/models/ilayer.hpp"
#include "interfaces/servicelocator/imakeable.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"

class IRasterOperation : public IMakeable
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

    virtual ~IRasterOperation() = default;

    virtual void initialize(
        const QWeakPointer<ILayer>& layer,
        Mode mode = Mode::paint
    ) = 0;

    virtual std::unique_ptr<QPainter> getBufferPainter(QRect region) = 0;
    virtual void render(QPainter& painter, QRect region) = 0;

    virtual QRect getBoundingRect() = 0;

    virtual void doOperation() = 0;
    virtual void undoOperation() = 0;
};

DECL_EXPECTS_INITIALIZE(IRasterOperation)
DECL_IMPLEMENTED_AS_QOBJECT(IRasterOperation)

#endif // IRASTEROPERATION_HPP