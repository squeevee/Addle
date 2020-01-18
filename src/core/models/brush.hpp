#ifndef BRUSH_HPP
#define BRUSH_HPP

#include "interfaces/models/ibrush.hpp"

class Brush : public IBrush
{
public:
    virtual ~Brush() = default;

    void initialize(BrushId id, QSharedPointer<IBrushRenderer> renderer);

    BrushId getId() { return _id; }
    QSharedPointer<IBrushRenderer> getRenderer() { return _renderer; }

private:
    BrushId _id;
    QSharedPointer<IBrushRenderer> _renderer;
};

#endif // BRUSH_HPP