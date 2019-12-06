#ifndef BASEBRUSHRENDERER_HPP
#define BASEBRUSHRENDERER_HPP

#include <QRect>
#include <QPointF>
#include <QPainter>

#include "data/brushtipid.hpp"
#include "utilities/mathutils.hpp"

class BaseBrushRenderer
{
public:
    virtual ~BaseBrushRenderer() = default;
    virtual BrushTipId getId() = 0;
    virtual bool snapToPixels() = 0;
    virtual bool sizeInvariant() { return false; }
    virtual QRect getBoundingRect (double brushSize, QPointF center) = 0;
    virtual QRect getBoundingRect (double brushSize, QLineF line)
    {
        return getBoundingRect(brushSize, line.p1()).united(getBoundingRect(brushSize, line.p2()));
    }
    virtual void render(double brushSize, QPointF center, QPainter& painter) = 0;
    virtual void render(double brushSize, QLineF line, QPainter& painter) = 0;
};

class BasePixelBrushRenderer : public BaseBrushRenderer
{
public:
    virtual ~BasePixelBrushRenderer() = default;

    bool snapToPixels() { return true; }
    virtual QRect getBoundingRect (double brushSize, QPointF center)
    {
        int properSize = qRound(brushSize);
        QPoint aCenter = pointFloor(center);
        return QRect( aCenter.x() - properSize / 2, aCenter.y() - properSize / 2, properSize, properSize);
    }

    virtual void render(double brushSize, QPointF center, QPainter& painter) = 0;
    virtual void render(double brushSize, QLineF line, QPainter& painter)
    {
        QPoint first = pointFloor(line.p1());
        QPoint second = pointFloor(line.p2());
        if (first == second)
            return;

        if (qAbs(first.x() - second.x()) > qAbs(first.y() - second.y()))
        {
            double slope = (double)(first.y() - second.y()) / (double)(first.x() - second.x());
            if(first.x() < second.x())
            {
                for (int x = first.x() + 1; x <= second.x(); x++)
                {
                    int y = qRound((x - first.x()) * slope) + first.y();
                    render(brushSize, QPointF(x,y), painter);
                }
            }
            else 
            {
                for (int x = first.x() - 1; x >= second.x(); x--)
                {
                    int y = qRound((x - first.x()) * slope) + first.y();
                    render(brushSize, QPointF(x,y), painter);
                }
            }
        }
        else 
        {
            double slope = (double)(first.x() - second.x()) / (double)(first.y() - second.y());
            if(first.y() < second.y())
            {
                for (int y = first.y() + 1; y <= second.y(); y++)
                {
                    int x = qRound((y - first.y()) * slope) + first.x();
                    render(brushSize, QPointF(x,y), painter);
                }
            }
            else 
            {
                for (int y = first.y() - 1; y >= second.y(); y--)
                {
                    int x = qRound((y - first.y()) * slope) + first.x();
                    render(brushSize, QPointF(x,y), painter);
                }
            }
        }
    }
};

class BaseSmoothBrushRenderer : public BaseBrushRenderer
{
public:
    virtual ~BaseSmoothBrushRenderer() = default;

    bool snapToPixels() { return false; }

    virtual QRect getBoundingRect(double brushSize, QPointF center)
    {
        QRectF fineBound = QRectF( center.x() - brushSize / 2, center.y() - brushSize / 2, brushSize, brushSize);
        return coarseBoundRect(fineBound);
    }
    virtual void render(double brushSize, QLineF line, QPainter& painter)
    {
        //todo
    }
};

#endif // BASEBRUSHRENDERER_HPP