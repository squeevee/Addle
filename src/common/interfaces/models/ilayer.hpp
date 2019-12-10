#ifndef ILAYER_HPP
#define ILAYER_HPP

#include <QPainter>

#include <QRect>
#include <QPoint>
#include <QImage>
#include <QPaintDevice>

#include "data/layerbuilder.hpp"
#include "interfaces/traits/makeable_trait.hpp"

#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"

#include "interfaces/editing/ihavebufferpainter.hpp"

class IDocument;
class ILayer: public IHaveBufferPainter
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

    virtual BufferPainter getBufferPainter(QRect area) = 0;

signals: 
    virtual void renderChanged(QRect area) = 0;

};

DECL_MAKEABLE(ILayer)
DECL_EXPECTS_INITIALIZE(ILayer)
DECL_IMPLEMENTED_AS_QOBJECT(ILayer)

#endif // ILAYER_HPP