#ifndef ILAYER_HPP
#define ILAYER_HPP

#include <QPainter>

#include <QRect>
#include <QPoint>
#include <QImage>
#include <QPaintDevice>

#include "utilities/model/layerbuilder.hpp"
#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"


namespace Addle {


class IRasterSurface;
class IDocument;
class ILayer : public virtual IAmQObject
{
public:
    virtual ~ILayer() {}

    virtual void initialize() = 0;
    virtual void initialize(LayerBuilder& builder) = 0;

    virtual bool isEmpty() = 0;
    
    virtual QRect boundary() = 0;
    virtual QPoint topLeft() = 0;
    virtual void setTopLeft(QPoint) = 0;

    virtual QSharedPointer<IRasterSurface> rasterSurface() = 0;
};

DECL_MAKEABLE(ILayer)
DECL_EXPECTS_INITIALIZE(ILayer)


} // namespace Addle
#endif // ILAYER_HPP