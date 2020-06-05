#include <QtDebug>

#include "rasterbrushengine.hpp"
#include "globalconstants.hpp"
#include "utilities/mathutils.hpp"

#include "interfaces/editing/irastersurface.hpp"
#include "utilities/canvas/brushstroke.hpp"

#include <QRadialGradient>

const BrushEngineId RasterBrushEngine::ID = GlobalConstants::CoreBrushEngines::RasterEngine;

QPainterPath RasterBrushEngine::indicatorShape(const BrushStroke& painter) const
{
    return QPainterPath();
}

void RasterBrushEngine::paint(BrushStroke& brushStroke) const
{
    const QPointF pos = brushStroke.positions().last();
    const double size = brushStroke.getSize();
    const double interval = size * 0.1; // TODO

    QRect bound = coarseBoundRect(pos, size);

    if (brushStroke.positions().size() <= 1 || !brushStroke.engineState().contains("lastPositionDrawn"))
    {
        auto handle = brushStroke.getBuffer()->getPaintHandle(bound);
        QPainter& painter = handle.getPainter();

        paintGradient(painter, pos, brushStroke.color(), size);
    }
    else
    {
        QPointF lastPosDrawn = brushStroke.engineState()["lastPositionDrawn"].toPointF();
        if (near(pos, lastPosDrawn, interval)) return;

        bound = bound.united(coarseBoundRect(lastPosDrawn, size));

        auto handle = brushStroke.getBuffer()->getPaintHandle(bound);
        QPainter& painter = handle.getPainter();

        const QLineF line(pos, lastPosDrawn);
        int steps = ceil(line.length() / interval);

        for (int i = 0; i < steps; i++)
        {
            qreal f = 1.0 - (interval * (i + 1) / line.length());
            paintGradient(painter, line.pointAt(f), brushStroke.color(), size);
        }
    }

    brushStroke.engineState()["lastPositionDrawn"] = pos;
}

void RasterBrushEngine::paintGradient(QPainter& painter, QPointF pos, QColor color, double size)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    QRadialGradient grad(pos, size / 2);
    grad.setStops({ { 0.5, color }, { 1.0, Qt::transparent } });

    painter.setPen(Qt::NoPen);
    painter.setBrush(grad);
    painter.drawEllipse(pos, size / 2, size / 2);
}