#include "basicbrushpainter.hpp"

#include <QPainter>
#include <QPen>
#include "utilities/mathutils.hpp"

QRect BasicBrushPainter::boundingRect(const BrushPainterData& segment) const
{
    if (!segment.hasStartPoint())
        return QRect();

    double halfSize = segment.getSize() / 2;
    QRectF fine(
        segment.getStartPoint() - QPoint(halfSize, halfSize),
        QSize(segment.getSize(), segment.getSize())
    );

    if (segment.hasEndPoint())
    {
        fine = fine.united(QRectF(
            segment.getEndPoint() - QPoint(halfSize, halfSize),
            QSize(segment.getSize(), segment.getSize())
        ));
    }

    return coarseBoundRect(fine);
}

void BasicBrushPainter::paint(BrushPainterData& segment, QImage& buffer) const
{
    if (!segment.hasStartPoint()) return;

    QPainter painter(&buffer);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setTransform(segment.getOntoBufferTransform());
    double halfSize = segment.getSize() / 2;

    if (segment.hasEndPoint())
    {
        QPainterPath inner(segment.getStartPoint());

        inner.lineTo(segment.getEndPoint());
        painter.fillPath(
            QPainterPathStroker(QPen(
                QBrush(),
                segment.getSize(),
                Qt::SolidLine,
                Qt::RoundCap
            )).createStroke(inner),
            segment.getColor()
        );
    }
    else 
    {
        QPainterPath path;
        path.addEllipse(segment.getStartPoint(), halfSize, halfSize);

        painter.fillPath(path, segment.getColor());
    }
}