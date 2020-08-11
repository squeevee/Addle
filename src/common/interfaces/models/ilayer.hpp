#ifndef ILAYER_HPP
#define ILAYER_HPP

#include <QPainter>

#include <QRect>
#include <QPoint>
#include <QImage>
#include <QPaintDevice>

#include "utilities/model/layerbuilder.hpp"
#include "interfaces/traits.hpp"


namespace Addle {


class IRasterSurface;
class IDocument;
class ILayer
{
public:
    virtual ~ILayer() {}

    virtual void initialize() = 0;
    virtual void initialize(LayerBuilder& builder) = 0;

    virtual bool isEmpty() = 0;
    
    virtual QRect getBoundary() = 0;
    virtual QPoint getTopLeft() = 0;
    virtual void setTopLeft(QPoint) = 0;

    virtual QSharedPointer<IRasterSurface> getRasterSurface() = 0;
};

DECL_MAKEABLE(ILayer)
DECL_EXPECTS_INITIALIZE(ILayer)
DECL_IMPLEMENTED_AS_QOBJECT(ILayer)

} // namespace Addle
#endif // ILAYER_HPP