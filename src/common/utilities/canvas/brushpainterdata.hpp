#ifndef BRUSHPAINTERDATA_HPP
#define BRUSHPAINTERDATA_HPP

#include <QSharedData>
#include <QSharedDataPointer>

#include <QColor>
#include <QPointF>
#include <QPainterPath>

#include "idtypes/brushid.hpp"

class BrushPainterData
{
    struct BrushPainterDataInner : QSharedData
    {
        BrushPainterDataInner(QColor color, double size, QPointF point)
            : color(color), size(size), startPoint(point), hasEndPoint(false)
        {
        }
        
        BrushPainterDataInner(QColor color, double size, QPointF startPoint, QPointF endPoint)
            : color(color), size(size), startPoint(startPoint), endPoint(endPoint), hasEndPoint(true)
        {
        }

        QColor color;
        double size;

        QPointF startPoint;
        bool hasEndPoint;
        QPointF endPoint;
    };
    
public:

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
    QPointF getStartPoint() const { return _data->startPoint; }
    bool hasEndPoint() const { return _data->hasEndPoint; }
    QPointF getEndPoint() const { return _data->hasEndPoint ? _data->endPoint : QPointF(); }

    double getSize() const { return _data->size; }

private:
    QSharedDataPointer<BrushPainterDataInner> _data;
};

#endif // BRUSHPAINTERDATA_HPP