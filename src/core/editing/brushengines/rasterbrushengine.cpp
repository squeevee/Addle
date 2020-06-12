#include <QtDebug>

#include "rasterbrushengine.hpp"
#include "globalconstants.hpp"
#include "utilities/mathutils.hpp"

#include "interfaces/editing/irastersurface.hpp"
#include "interfaces/editing/rasterengineparams.hpp"
#include "utilities/editing/brushstroke.hpp"

#include "servicelocator.hpp"

#include <cmath>

#include <QRadialGradient>

const BrushEngineId RasterBrushEngine::ID = GlobalConstants::CoreBrushEngines::RasterEngine;

static double gradientTaper(double t, double hardness)
{
    return t < 0.5 ?
        2 * t * t :
        1 - pow(-2 * t + 2, 2) / 2;
}

QPainterPath RasterBrushEngine::indicatorShape(const BrushStroke& painter) const
{
    return QPainterPath();
}

void RasterBrushEngine::paint(BrushStroke& brushStroke) const
{
    RasterEngineParams params(ServiceLocator::get<IBrushModel>(brushStroke.id()));

    QPointF pos = brushStroke.positions().last();
    double size = brushStroke.getSize();
    QColor color = brushStroke.color();

    QRect bound = coarseBoundRect(pos, size);
    
    if (brushStroke.positions().size() == 1)
    {
        auto handle = brushStroke.getBuffer()->getPaintHandle(bound);
        QPainter& painter = handle.getPainter();

        paint_p(painter, params, pos, color, size);
    }
    else
    {
        double spacing = params.spacing() * size;
        if (brushStroke.lengthSincePaint() < spacing) return;
        
        bound = bound.united(brushStroke.lastPaintedBound());

        auto handle = brushStroke.getBuffer()->getPaintHandle(bound);
        QPainter& painter = handle.getPainter();

        int steps = ceil(distance(brushStroke.lastPositionPainted(), pos) / spacing);

        if (steps == 1)
        {
            paint_p(painter, params, pos, color, size);
        }
        else 
        {
            const QLineF line(brushStroke.lastPositionPainted(), pos);
            for (int i = 0; i < steps; i++)
            {
                const qreal t = (qreal)(i + 1) / steps;
                paint_p(painter, params, line.pointAt(t), color, size);
            }
        }
    }

    brushStroke.setBound(bound);
    brushStroke.markPainted();
}

void RasterBrushEngine::paint_p(
        QPainter& painter,
        const RasterEngineParams& params,
        QPointF pos,
        QColor color,
        double size
    )
{
    switch (params.mode())
    {
    case RasterEngineParams::Gradient:
        paintGradient(painter, pos, color, size, params.hardness());
        break;
    }
}

void RasterBrushEngine::paintGradient(
        QPainter& painter,
        QPointF pos,
        QColor color,
        double size,
        double hardness
    )
{
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRadialGradient grad(pos, size / 2);
    grad.setColorAt(0, color);

    const int steps = qMin((int)(size / 4), 25);
    if (steps >= 3)
    {
        for (int i = 0; i < steps; i++)
        {
            const double t = (double)(i + 1) / (steps + 1);

            QColor stop = color;
            stop.setAlphaF((1 - gradientTaper(t, hardness)) * color.alphaF());
            grad.setColorAt(t, stop);
        }
    }

    grad.setColorAt(1.0, Qt::transparent);

    painter.setPen(Qt::NoPen);
    painter.setBrush(grad);
    painter.drawEllipse(pos, size / 2, size / 2);
}