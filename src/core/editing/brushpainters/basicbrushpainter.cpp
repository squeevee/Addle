#include "basicbrushpainter.hpp"

#include <QPainter>
#include <QPen>
#include "utilities/mathutils.hpp"

const BrushId BasicBrushPainter::ID = IBrushModel::CoreBrushes::BasicBrush;

const BrushPainterInfo BasicBrushPainter::INFO = BrushPainterInfo(
    /* isSizeInvariant: */ false,
    /* isColorInvariant: */ false,
    /* isPixelAliased: */ false,
    /* minSize: */ DEFAULT_MINIMUM_SIZE,
    /* maxSize: */ DEFAULT_MAXIMUM_SIZE
);


void BasicBrushPainter::initialize(QSharedPointer<IRasterSurface> buffer, QColor color, double size)
{
    _commonHelper.initialize(buffer, size, color);
}

// QRect BasicBrushPainter::boundingRect(const BrushPainterData& data) const
// {
//     if (!data.hasStartPoint())
//         return QRect();

//     double halfSize = data.getSize() / 2;
//     QRectF fine(
//         data.getStartPoint() - QPoint(halfSize, halfSize),
//         QSize(data.getSize(), data.getSize())
//     );

//     if (data.hasEndPoint())
//     {
//         fine = fine.united(QRectF(
//             data.getEndPoint() - QPoint(halfSize, halfSize),
//             QSize(data.getSize(), data.getSize())
//         ));
//     }

//     return coarseBoundRect(fine);
// }

// void BasicBrushPainter::paint(BrushPainterData& data, QImage& buffer) const
// {
//     if (!data.hasStartPoint()) return;

//     QPainter painter(&buffer);
//     painter.setRenderHints(QPainter::Antialiasing);
//     painter.setTransform(data.getOntoBufferTransform());
//     double halfSize = data.getSize() / 2;

//     if (data.hasEndPoint())
//     {
//         QPainterPath inner(data.getStartPoint());
//         inner.lineTo(data.getEndPoint());

//         QPainterPath path = QPainterPathStroker(QPen(
//             QBrush(),
//             data.getSize(),
//             Qt::SolidLine,
//             Qt::RoundCap
//         )).createStroke(inner);

//         painter.fillPath(
//             path,
//             data.getColor()
//         );
//     }
//     else 
//     {
//         QPainterPath path;
//         path.addEllipse(data.getStartPoint(), halfSize, halfSize);

//         painter.fillPath(path, data.getColor());
//     }
// }

void BasicBrushPainter::setPosition(QPointF pos)
{

}

void BasicBrushPainter::moveTo(QPointF pos)
{

}