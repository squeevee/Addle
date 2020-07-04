#ifndef RASTERBRUSHENGINE_HPP
#define RASTERBRUSHENGINE_HPP

#include "interfaces/editing/ibrushengine.hpp"
#include "compat.hpp"

class RasterEngineParams;

class ADDLE_CORE_EXPORT RasterBrushEngine : public IBrushEngine
{
public:
    static const BrushEngineId ID;

    virtual ~RasterBrushEngine() = default;

    BrushEngineId id() const { return ID; }

    //BrushInfo info(const IBrushModel& model) const { return BrushInfo(); }

    QPainterPath indicatorShape(const BrushStroke& painter) const;
    void paint(BrushStroke& painter) const;

private:
    static void paint_p(QPainter& painter, const RasterEngineParams& params, QPointF pos, QColor color, double size);
    static void paintGradient(QPainter& painter, QPointF pos, QColor color, double size, double hardness);
};

#endif // RASTERBRUSHENGINE_HPP