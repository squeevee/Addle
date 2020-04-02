#include "basicbrushpainter.hpp"

#include <QPainter>
#include <QPen>
#include "utilities/mathutils.hpp"

QRect BasicBrushPainter::boundingRect(const BrushPainterData& segment) const
{
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
    double halfSize = segment.getSize() / 2;
    QPainterPath inner(segment.getStartPoint());

    if (segment.hasEndPoint())
    {
        inner.lineTo(segment.getEndPoint());
    }

    QPainter painter(&buffer);
    painter.fillPath(
        QPainterPathStroker(QPen(
            QBrush(),
            halfSize,
            Qt::SolidLine,
            Qt::RoundCap
        )).createStroke(inner),
        segment.getColor()
    );
}