#ifndef IBASICBRUSHPAINTER_HPP
#define IBASICBRUSHPAINTER_HPP

#include "interfaces/traits/makeable_trait.hpp"

#include "ibrushpainter.hpp"

class IBasicBrushPainter : public IBrushPainter
{
public:
    ~IBasicBrushPainter() = default;

    static const BrushId Id;
};

DECL_MAKEABLE(IBasicBrushPainter)

#endif // IBASICBRUSHPAINTER_HPP