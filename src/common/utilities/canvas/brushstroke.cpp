#include "brushstroke.hpp"
#include "servicelocator.hpp"

BrushStroke::BrushStroke(BrushId id,
        QColor color,
        double size,
        QSharedPointer<IRasterSurface> buffer)
    : _brush(ServiceLocator::get<IBrushModel>(id)), _buffer(buffer)
{
    _painterStates.push(PainterState(color, size));
}

void BrushStroke::conform()
{
    // if the painter state is outside some bounds set by _model.info(), quietly
    // change the painter to conform to those bounds.
}

void BrushStroke::save()
{
    _painterStates.push(_painterStates.top());
}

void BrushStroke::restore()
{
    if (_painterStates.size() == 1) return;
    _painterStates.pop();
}

QColor BrushStroke::color() const
{
    return _painterStates.top().color;
}

void BrushStroke::setColor(QColor color)
{
    _painterStates.top().color = color;
}

double BrushStroke::getSize() const
{
    return _painterStates.top().size;
}

void BrushStroke::setSize(double size)
{
    _painterStates.top().size = size;
}

bool BrushStroke::isPreview() const
{
    return _painterStates.top().isPreview;
}

void BrushStroke::setPreview(bool isPreview)
{
    _painterStates.top().isPreview = isPreview;
}

QList<QPointF> BrushStroke::positions() const
{
    return _painterStates.top().positions;
}

void BrushStroke::moveTo(QPointF pos)
{
    _painterStates.top().positions.append(pos);
}

void BrushStroke::clear()
{
    _painterStates.top().positions.clear();
}

QVariantHash BrushStroke::engineState() const
{
    return _painterStates.top().engineState;
}

QVariantHash& BrushStroke::engineState()
{
    return _painterStates.top().engineState;
}