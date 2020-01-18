#ifndef BRUSHPATHSEGMENT_HPP
#define BRUSHPATHSEGMENT_HPP

#include <QSharedData>
#include <QSharedDataPointer>

#include <QColor>
#include <QPointF>

#include "brushid.hpp"

// TODO: at some point, if dynamic brushes are supported this will also include
// things like velocity, pressure, angle, random seed, and possibly an abstract
// structure for brush-specific state

class BrushPathSegment
{
    struct BrushPathSegmentData : QSharedData
    {
        BrushPathSegmentData(QColor color, double size, QPointF point)
            : color(color), size(size), startPoint(point), hasEndPoint(false)
        {
        }
        
        BrushPathSegmentData(QColor color, double size, QPointF startPoint, QPointF endPoint)
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

    BrushPathSegment(QColor color, double size, QPointF point)
        : _data(new BrushPathSegmentData(color, size, point))
    {
    }

    BrushPathSegment(QColor color, double size, QPointF startPoint, QPointF endPoint)
        : _data(new BrushPathSegmentData(color, size, startPoint, endPoint))
    {
    }

    BrushPathSegment(const BrushPathSegment& other)
        : _data(other._data)
    {
    }

    QColor getColor() const { return _data->color; }
    QPointF getStartPoint() const { return _data->startPoint; }
    bool hasEndPoint() const { return _data->hasEndPoint; }
    QPointF getEndPoint() const { return _data->hasEndPoint ? _data->endPoint : QPointF(); }

private:
    QSharedDataPointer<BrushPathSegmentData> _data;
};

#endif // BRUSHPATHSEGMENT_HPP