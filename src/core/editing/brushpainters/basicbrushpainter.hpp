#ifndef BASICBRUSHPAINTER_HPP
#define BASICBRUSHPAINTER_HPP

#include <QObject>

#include "interfaces/editing/ibrushpainter.hpp"
#include "interfaces/models/ibrushmodel.hpp"

#include "helpers/commonbrushpainterhelper.hpp"

class BasicBrushPainter : public QObject, public IBrushPainter
{
    Q_OBJECT
public:

    BasicBrushPainter() : _commonHelper(*this) {}
    virtual ~BasicBrushPainter() = default;

    void initialize(QSharedPointer<IRasterSurface> buffer, QColor color, double size);

    static const BrushId ID;
    BrushId getId() const { return ID; }

    static const BrushPainterInfo INFO;
    BrushPainterInfo getInfo() { return INFO; }

    QSharedPointer<IRasterSurface> getBuffer() { return _commonHelper.buffer; }
    void setBuffer(QSharedPointer<IRasterSurface> buffer) { _commonHelper.buffer = buffer; }

    QColor getColor() { return _commonHelper.getColor(); }
    void setColor(QColor color) { _commonHelper.setColor(color); }

    double getSize() { return _commonHelper.getSize(); }
    void setSize(double size) { _commonHelper.setSize(size); }

    QPainterPath getIndicatorShape() { return QPainterPath(); }
    
    bool isPreview() const { return _commonHelper.isPreview; }
    void setPreview(bool isPreview) { _commonHelper.isPreview = isPreview; }

    void setPosition(QPointF pos);
    void moveTo(QPointF pos);

signals: 
    void colorChanged(QColor color);
    void sizeChanged(QColor color);

private:
    CommonBrushPainterHelper _commonHelper;
};

#endif // BASICBRUSHPAINTER_HPP