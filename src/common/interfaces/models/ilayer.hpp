#ifndef ILAYER_HPP
#define ILAYER_HPP

#include <QRect>
#include <QPoint>
#include <QImage>
#include <QPaintDevice>

#include "data/layerbuilder.hpp"
#include "interfaces/servicelocator/imakeable.hpp"

#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"

class IDocument;
class IRasterOperation;
class ILayer: public virtual IMakeable
{
public:
    virtual ~ILayer() {}

    virtual void initialize() = 0;
    virtual void initialize(LayerBuilder& builder) = 0;

    virtual bool isEmpty() = 0;

    virtual void render(QPainter& painter, QRect area) = 0;

    virtual QRect getBoundary() = 0;
    virtual QPoint getTopLeft() = 0;
    virtual void setTopLeft(QPoint) = 0;

    virtual void applyRasterOperation(IRasterOperation* operation) = 0;
};

DECL_EXPECTS_INITIALIZE(ILayer)
DECL_IMPLEMENTED_AS_QOBJECT(ILayer)

#endif // ILAYER_HPP