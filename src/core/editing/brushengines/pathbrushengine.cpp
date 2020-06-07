#include "pathbrushengine.hpp"
#include "globalconstants.hpp"

#include "interfaces/editing/irastersurface.hpp"
#include <QPainter>
#include <QPen>
#include <QtDebug>
#include "utilities/mathutils.hpp"

const BrushEngineId PathBrushEngine::ID = GlobalConstants::CoreBrushEngines::PathEngine;

QPainterPath PathBrushEngine::indicatorShape(const BrushStroke& painter) const
{
    return QPainterPath();
}

void PathBrushEngine::paint(BrushStroke& brushStroke) const
{
    if (brushStroke.positions().isEmpty()) return;

    if (brushStroke.positions().size() == 1)
    {
        const QPointF pos = brushStroke.positions().last();
        const double size = brushStroke.getSize();
        const QRect nibBound = coarseBoundRect(pos, brushStroke.getSize());

        const double halfSize = size / 2;
        //QPainterPath path(pos);

        auto handle = brushStroke.getBuffer()->getPaintHandle(nibBound);
        QPainter& painter = handle.getPainter();

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.setPen(Qt::NoPen);
        painter.setBackground(Qt::transparent);
        painter.setBrush(brushStroke.color());

        painter.eraseRect(nibBound);
        painter.drawEllipse(QRectF(
            pos - QPointF(halfSize, halfSize),
            QSizeF(size, size)
        ));

        brushStroke.engineState()["lastNibBound"] = nibBound;
    }
    else 
    {
        const QPointF pos = brushStroke.positions().last();
        const double size = brushStroke.getSize();
        const QRect lastNibBound = brushStroke.engineState()["lastNibBound"].toRect();
        // const QImage flattened = brushStroke.engineState()["flattened"].value<QImage>();

        QRect nibBound = coarseBoundRect(pos, size);
        QRect bound = nibBound.united(lastNibBound);

        QPainterPath path(brushStroke.positions().first());
        QList<QPointF> posList = brushStroke.positions();
        for (auto i = posList.begin() + 1; i != posList.end(); ++i)
        {
            path.lineTo(*i);
        }

        auto handle = brushStroke.getBuffer()->getPaintHandle(bound);
        QPainter& painter = handle.getPainter();

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.setClipRect(bound);
        painter.setPen(QPen(
            brushStroke.color(),
            size,
            Qt::SolidLine,
            Qt::RoundCap,
            Qt::RoundJoin
        ));
        painter.setBackground(Qt::transparent);

        painter.eraseRect(bound);
        // if (!flattened.isNull() && bound.intersects(QRect(flattened.offset(), flattened.size())))
        // {
        //     painter.drawImage(flattened.offset(), flattened);
        // }
        painter.drawPath(path);

        brushStroke.engineState()["lastNibBound"] = nibBound;
    }
}
