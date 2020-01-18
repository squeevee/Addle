#include "brush.hpp"

void Brush::initialize(BrushId id, QSharedPointer<IBrushRenderer> renderer)
{
    _id = id;
    _renderer = renderer;
}