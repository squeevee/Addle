#ifndef IBRUSH_HPP
#define IBRUSH_HPP

#include "data/brushid.hpp"

#include <QPainter>

class IBrush 
{
public:
    class IBrushRenderer;

    virtual ~IBrush() = default;
    
    virtual BrushId getId() = 0;

    virtual bool isSizeInvariant() = 0;
    virtual bool isColorInvariant() = 0;
    virtual bool isAliased() = 0;

    static constexpr double DEFAULT_MINIMUM_SIZE = 0.01;
    static constexpr double DEFAULT_MAXIMUM_SIZE = 1000.0;

    virtual double getMinimumSize() = 0;
    virtual double getMaximumSize() = 0;

    virtual IBrushRenderer* makeRenderer() = 0;
    virtual IBrushRenderer* makeRenderer(double size, QPointF position, QColor color) = 0;

    class IBrushRenderer
    {
    public:
        virtual ~IBrushRenderer() = default;

        virtual IBrush& getOwner() = 0;

        virtual double getSize() = 0;
        virtual void setSize(double size) = 0;

        virtual QPointF getPosition() = 0;
        virtual void setPosition(QPointF pos) = 0;

        virtual QColor getColor() = 0;
        virtual void setColor(QColor color) = 0;

        virtual QRectF getBoundingRect() = 0;
        virtual void render(QPainter& painter) = 0;
    };
};

#endif // IBRUSH_HPP