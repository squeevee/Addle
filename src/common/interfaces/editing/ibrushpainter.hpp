#ifndef IBRUSHPAINTER_HPP
#define IBRUSHPAINTER_HPP

#include <QPointF>
#include <QPainterPath>
#include <QColor>
#include <QSharedPointer>

#include "idtypes/brushid.hpp"

#include "utilities/canvas/brushpainterinfo.hpp"

#include "interfaces/traits/by_id_traits.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"

class IRasterSurface;
class IBrushPainter
{
public: 
    virtual ~IBrushPainter() = default;

    virtual BrushId getId() const = 0;

    virtual void initialize(
        QSharedPointer<IRasterSurface> buffer,
        QColor color = QColor(),
        double size = 0
    ) = 0;

    virtual BrushPainterInfo getInfo() = 0;

    virtual QSharedPointer<IRasterSurface> getBuffer() = 0;
    virtual void setBuffer(QSharedPointer<IRasterSurface> buffer) = 0;

    virtual QColor getColor() = 0;
    virtual void setColor(QColor color) = 0;

    static constexpr double DEFAULT_MINIMUM_SIZE = 0.01;
    static constexpr double DEFAULT_MAXIMUM_SIZE = 1000.0;

    virtual double getSize() = 0;
    virtual void setSize(double size) = 0;

    virtual QPainterPath getIndicatorShape() = 0;
    
    virtual bool isPreview() const = 0;
    virtual void setPreview(bool isPreview) = 0;

    virtual void setPosition(QPointF pos) = 0;
    virtual void moveTo(QPointF pos) = 0;

signals: 
    virtual void colorChanged(QColor color) = 0;
    virtual void sizeChanged(QColor color) = 0;
};

DECL_MAKEABLE_BY_ID(IBrushPainter, BrushId);
DECL_EXPECTS_INITIALIZE(IBrushPainter);
DECL_IMPLEMENTED_AS_QOBJECT(IBrushPainter);

#endif // IBRUSHPAINTER_HPP