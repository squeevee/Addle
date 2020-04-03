#ifndef BRUSHPAINTERDATA_HPP
#define BRUSHPAINTERDATA_HPP

#include <QSharedData>
#include <QSharedDataPointer>

#include <QColor>
#include <QPointF>
#include <QPainterPath>
#include <QTransform>

#include "idtypes/brushid.hpp"

class BrushPainterData
{
    struct BrushPainterDataInner : QSharedData
    {
        BrushPainterDataInner() = default;

        BrushPainterDataInner(QColor color, double size, QPointF point)
            : color(color), size(size), startPoint(point), hasStartPoint(true), hasEndPoint(false)
        {
        }
        
        BrushPainterDataInner(QColor color, double size, QPointF startPoint, QPointF endPoint)
            : color(color), size(size), startPoint(startPoint), hasStartPoint(true), endPoint(endPoint), hasEndPoint(true)
        {
        }

        QColor color;
        double size = 0;

        bool hasStartPoint = false;
        QPointF startPoint;

        bool hasEndPoint = false;
        QPointF endPoint;

        bool isPreview = false;

        QTransform ontoBufferTransform;
    };
    
public:

    BrushPainterData()
        : _data(new BrushPainterDataInner)
    {
    }

    BrushPainterData(QColor color, double size, QPointF point)
        : _data(new BrushPainterDataInner(color, size, point))
    {
    }

    BrushPainterData(QColor color, double size, QPointF startPoint, QPointF endPoint)
        : _data(new BrushPainterDataInner(color, size, startPoint, endPoint))
    {
    }

    BrushPainterData(const BrushPainterData& other)
        : _data(other._data)
    {
    }

    QColor getColor() const { return _data->color; }
    void setColor(QColor color) { _data->color = color; }

    bool hasStartPoint() const { return _data->hasStartPoint; }
    QPointF getStartPoint() const { return _data->hasStartPoint ? _data->startPoint : QPointF(); }
    void setStartPoint(QPointF point) { _data->startPoint = point; _data->hasStartPoint = true; }

    bool hasEndPoint() const { return _data->hasEndPoint; }
    QPointF getEndPoint() const { return _data->hasEndPoint ? _data->endPoint : QPointF(); }
    void setEndPoint(QPointF point) { _data->endPoint = point; _data->hasEndPoint = true; }

    double getSize() const { return _data->size; }
    void setSize(double size) { _data->size = size; }

    bool isPreview() const { return _data->isPreview; }
    void setPreview(bool preview) { _data->isPreview = preview; }

    QTransform getOntoBufferTransform() const { return _data->ontoBufferTransform; }
    void setOntoBufferTransform(QTransform transform) { _data->ontoBufferTransform = transform; }

    inline bool operator==(const BrushPainterData& other) const
    {
        return _data->color == other._data->color
            && _data->size == other._data->size 
            && _data->hasStartPoint == other._data->hasStartPoint
            && _data->startPoint == other._data->startPoint
            && _data->hasEndPoint == other._data->hasEndPoint
            && _data->endPoint == other._data->endPoint
            && _data->isPreview == other._data->isPreview
            && _data->ontoBufferTransform == other._data->ontoBufferTransform;
    }

    inline bool operator!=(const BrushPainterData& other) const
    {
        return !(other == *this);
    }

private:
    QSharedDataPointer<BrushPainterDataInner> _data;
};

#endif // BRUSHPAINTERDATA_HPP