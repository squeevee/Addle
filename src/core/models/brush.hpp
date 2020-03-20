#ifndef BRUSH_HPP
#define BRUSH_HPP

#include "interfaces/models/ibrush.hpp"

class Brush : public IBrush
{
public:
    virtual ~Brush() = default;

    void initialize(BrushId id, QSharedPointer<IBrushPainter> painter);

    BrushId getId() { return _id; }
    QSharedPointer<IBrushPainter> getPainter() { return _painter; }

private:
    BrushId _id;
    QSharedPointer<IBrushPainter> _painter;
};

#endif // BRUSH_HPP