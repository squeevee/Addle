#ifndef RASTERBRUSHENGINE_HPP
#define RASTERBRUSHENGINE_HPP

#include "interfaces/editing/ibrushengine.hpp"

class RasterBrushEngine : public IBrushEngine
{
public:
    static const BrushEngineId ID;

    virtual ~RasterBrushEngine() = default;

    BrushEngineId id() const { return ID; }

    BrushInfo info(const IBrushModel& model) const { return BrushInfo(); }

    QPainterPath indicatorShape(const BrushStroke& painter) const;
    void paint(BrushStroke& painter) const;

private:
    static void paintGradient(QPainter& painter, QPointF pos, QColor color, double size);
};

#endif // RASTERBRUSHENGINE_HPP