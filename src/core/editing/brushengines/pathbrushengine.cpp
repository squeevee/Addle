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

QRect PathBrushEngine::boundingRect(QPointF pos, double size) const
{
    double halfSize = size / 2;

    return coarseBoundRect(
        QRectF(pos - QPointF(halfSize, halfSize), QSizeF(size, size))
    );
}

void PathBrushEngine::paint(BrushStroke& brushStroke) const
{
    if (brushStroke.positions().isEmpty()) return;

    if (brushStroke.positions().size() == 1)
    {
        const QPointF pos = brushStroke.positions().last();
        const double size = brushStroke.getSize();
        const QRect nibBound = boundingRect(pos, brushStroke.getSize());

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

        QRect nibBound = boundingRect(pos, size);
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
        painter.drawPath(path);

        brushStroke.engineState()["lastNibBound"] = nibBound;
    }
}

// void PathEngine::startFrom(QPointF pos)
// {
//     QRect nibBound = boundingRect(pos);

//     double halfSize = _common.getSize() / 2;
//     _path = QPainterPath(pos);

//     auto handle = _common.buffer->getPaintHandle(nibBound);
//     QPainter& painter = handle.getPainter();

//     painter.setRenderHint(QPainter::Antialiasing, true);
//     painter.setCompositionMode(QPainter::CompositionMode_Source);
//     painter.setPen(Qt::NoPen);
//     painter.setBackground(Qt::transparent);
//     painter.setBrush(_common.getColor());

//     painter.eraseRect(nibBound);
//     painter.drawEllipse(QRectF(
//         pos - QPointF(halfSize, halfSize),
//         QSizeF(_common.getSize(), _common.getSize())
//     ));

//     _lastNibBound = nibBound;
// }

// void PathEngine::moveTo(QPointF pos)
// {
//     QRect nibBound = boundingRect(pos);
//     QRect bound = nibBound.united(_lastNibBound);

//     _path.lineTo(pos);

//     auto handle = _common.buffer->getPaintHandle(bound);
//     QPainter& painter = handle.getPainter();

//     painter.setRenderHint(QPainter::Antialiasing, true);
//     painter.setCompositionMode(QPainter::CompositionMode_Source);
//     painter.setClipRect(bound);
//     painter.setPen(QPen(
//         _common.getColor(),
//         _common.getSize(),
//         Qt::SolidLine,
//         Qt::RoundCap,
//         Qt::RoundJoin
//     ));
//     painter.setBackground(Qt::transparent);

//     painter.eraseRect(bound);
//     painter.drawPath(_path);

//     // The linear cost of redrawing _path doesn't seem to be too big of a
//     // performance hit for most lines of reasonable size and complexity, but it
//     // does add up quickly enough to be noticeable.

//     // Since we only want to redraw rectangular sections of the path at a time,
//     // and the path is assumed to have uniform width, we should be able to
//     // optimize this quite a lot using a spacial tree.

//     // (I'll note that using small curved masks on a segment-by-segment basis 
//     // does not anti-alias well.)

//     _lastNibBound = nibBound;
// }