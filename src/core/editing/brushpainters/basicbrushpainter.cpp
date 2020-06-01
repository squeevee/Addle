#include "basicbrushpainter.hpp"
#include "globalconstants.hpp"

#include <QPainter>
#include <QPen>
#include "utilities/mathutils.hpp"

const BrushId BasicBrushPainter::ID = GlobalConstants::CoreBrushes::BasicBrush;

const BrushPainterInfo BasicBrushPainter::INFO = BrushPainterInfo::defaultValues();

void BasicBrushPainter::initialize(QColor color, double size, QSharedPointer<IRasterSurface> buffer)
{
    _common.initialize(size, color, buffer);
}

QRect BasicBrushPainter::boundingRect(QPointF pos)
{
    double halfSize = _common.getSize() / 2;

    return coarseBoundRect(
        QRectF(pos - QPointF(halfSize, halfSize), QSizeF(_common.getSize(), _common.getSize()))
    );
}

void BasicBrushPainter::startFrom(QPointF pos)
{
    QRect nibBound = boundingRect(pos);

    double halfSize = _common.getSize() / 2;
    _path = QPainterPath(pos);

    auto handle = _common.buffer->getPaintHandle(nibBound);
    QPainter& painter = handle.getPainter();

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setPen(Qt::NoPen);
    painter.setBackground(Qt::transparent);
    painter.setBrush(_common.getColor());

    painter.eraseRect(nibBound);
    painter.drawEllipse(QRectF(
        pos - QPointF(halfSize, halfSize),
        QSizeF(_common.getSize(), _common.getSize())
    ));

    _lastNibBound = nibBound;
}

void BasicBrushPainter::moveTo(QPointF pos)
{
    QRect nibBound = boundingRect(pos);
    QRect bound = nibBound.united(_lastNibBound);

    _path.lineTo(pos);

    auto handle = _common.buffer->getPaintHandle(bound);
    QPainter& painter = handle.getPainter();

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setClipRect(bound);
    painter.setPen(QPen(
        _common.getColor(),
        _common.getSize(),
        Qt::SolidLine,
        Qt::RoundCap,
        Qt::RoundJoin
    ));
    painter.setBackground(Qt::transparent);

    painter.eraseRect(bound);
    painter.drawPath(_path);

    // The linear cost of redrawing _path doesn't seem to be too big of a
    // performance hit for most lines of reasonable size and complexity, but it
    // does add up quickly enough to be noticeable.

    // Since we only want to redraw rectangular sections of the path at a time,
    // and the path is assumed to have uniform width, we should be able to
    // optimize this quite a lot using a spacial tree.

    // (I'll note that using small curved masks on a segment-by-segment basis 
    // does not anti-alias well.)

    _lastNibBound = nibBound;
}