/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "brushstroke.hpp"
#include "servicelocator.hpp"

#include "interfaces/editing/ibrushengine.hpp"

#include "utils.hpp"

using namespace Addle;

BrushStroke::BrushStroke(BrushId id,
        QColor color,
        double size,
        QSharedPointer<IRasterSurface> buffer)
    : _id(id), _brush(ServiceLocator::get<IBrush>(id)), _buffer(buffer)
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

double BrushStroke::size() const
{
    return _painterStates.top().size;
}

void BrushStroke::setSize(double size)
{
    _painterStates.top().size = size;
}


QRect BrushStroke::bound() const
{
    return _painterStates.top().bound;
}

void BrushStroke::setBound(QRect bound)
{
    _painterStates.top().bound = bound;
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
    double length = 0;
    if (!_painterStates.top().positions.isEmpty())
        length = distance(_painterStates.top().positions.last(), pos);

    _painterStates.top().positions.append(pos);
    _painterStates.top().length += length;
}

void BrushStroke::clear()
{
    _painterStates.top().positions.clear();
    _painterStates.top().length = 0;

    _painterStates.top().lastPositionPainted = QPointF();
    _painterStates.top().lastLengthPainted = 0;
    _painterStates.top().markedPainted = false;
}

double BrushStroke::length() const
{
    return _painterStates.top().length;
}


QPointF BrushStroke::lastPositionPainted() const
{
    return _painterStates.top().lastPositionPainted;
}

double BrushStroke::lastLengthPainted() const
{
    return _painterStates.top().lastLengthPainted;
}

QRect BrushStroke::lastPaintedBound() const
{
    return _painterStates.top().lastPaintedBound;
}

bool BrushStroke::isMarkedPainted() const 
{
    return _painterStates.top().markedPainted;
}

void BrushStroke::markPainted()
{
    if (_painterStates.top().positions.isEmpty()) return;

    _painterStates.top().lastPositionPainted = _painterStates.top().positions.last();
    _painterStates.top().lastLengthPainted = _painterStates.top().length;
    _painterStates.top().lastPaintedBound = _painterStates.top().bound;
    _painterStates.top().markedPainted = true;
}

QVariantHash BrushStroke::engineState() const
{
    return _painterStates.top().engineState;
}

QVariantHash& BrushStroke::engineState()
{
    return _painterStates.top().engineState;
}

void BrushStroke::paint()
{
    ServiceLocator::get<IBrushEngine>(_brush.engineId()).paint(*this);
}