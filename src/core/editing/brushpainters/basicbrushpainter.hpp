#ifndef BASICBRUSHPAINTER_HPP
#define BASICBRUSHPAINTER_HPP

#include <QObject>

#include "interfaces/editing/ibrushpainter.hpp"
#include "interfaces/models/ibrushmodel.hpp"
#include "core/compat.hpp"

#include "helpers/brushpaintercommon.hpp"

class ADDLE_CORE_EXPORT BasicBrushPainter : public QObject, public IBrushPainter
{
    Q_OBJECT
public:

    BasicBrushPainter() : _common(*this) {}
    virtual ~BasicBrushPainter() = default;

    void initialize(QColor color, double size, QSharedPointer<IRasterSurface> buffer);

    static const BrushId ID;
    BrushId getId() const { return ID; }

    static const BrushPainterInfo INFO;
    BrushPainterInfo getInfo() { return INFO; }

    QSharedPointer<IRasterSurface> getBuffer() { return _common.buffer; }

    QColor getColor() { return _common.getColor(); }
    void setColor(QColor color) { _common.setColor(color); }

    double getSize() { return _common.getSize(); }
    void setSize(double size) { _common.setSize(size); }

    QPainterPath getIndicatorShape() { return QPainterPath(); }
    
    bool isPreview() const { return _common.isPreview; }
    void setPreview(bool isPreview) { _common.isPreview = isPreview; }

    void startFrom(QPointF pos);
    void moveTo(QPointF pos);

signals: 
    void colorChanged(QColor color);
    void sizeChanged(QColor color);

private:
    QRect boundingRect(QPointF pos);

    QPainterPath _path;
    QRect _lastNibBound;

    BrushPainterCommon _common;
};

#endif // BASICBRUSHPAINTER_HPP