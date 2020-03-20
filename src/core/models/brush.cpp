#include "brush.hpp"

void Brush::initialize(BrushId id, QSharedPointer<IBrushPainter> painter)
{
    _id = id;
    _painter = painter;
}