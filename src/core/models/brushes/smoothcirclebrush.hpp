#ifndef SMOOTHCIRCLEBRUSH_HPP
#define SMOOTHCIRCLEBRUSH_HPP

#include <QPainter>
#include "interfaces/models/brushes/corebrushes.hpp"

#include "helpers/basebrushrenderer.hpp"

class SmoothCircleBrush : public CoreBrushes::ISmoothCircleBrush
{
    class SmoothCircleBrushRenderer : public BaseBrushRenderer
    {
    public:
        SmoothCircleBrushRenderer(SmoothCircleBrush& owner)
            : BaseBrushRenderer(owner)
        {
        }

        SmoothCircleBrushRenderer(SmoothCircleBrush& owner, double size, QPointF position, QColor color)
            : BaseBrushRenderer(owner, size, position, color)
        {
        }

        QRectF getBoundingRect()
        {
            return QRectF();
        }

        void render(QPainter& painter)
        {

        }
    };

public: 
    BrushId getId() { return Id; }

    bool isSizeInvariant() { return false; }
    bool isColorInvariant() { return false; }
    bool isAliased() { return false; }

    double getMinimumSize() { return DEFAULT_MINIMUM_SIZE; }
    double getMaximumSize() { return DEFAULT_MAXIMUM_SIZE; }

    
    IBrushRenderer* makeRenderer()
    {
        return new SmoothCircleBrushRenderer(*this);
    }

    IBrushRenderer* makeRenderer(double size, QPointF position, QColor color)
    {
        return new SmoothCircleBrushRenderer(*this, size, position, color);
    }
};

#endif // SMOOTHCIRCLEBRUSH_HPP