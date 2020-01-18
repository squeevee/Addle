#ifndef IBASICBRUSHRENDERER_HPP
#define IBASICBRUSHRENDERER_HPP

#include "interfaces/traits/makeable_trait.hpp"

#include "ibrushrenderer.hpp"

class IBasicBrushRenderer : public IBrushRenderer
{
public:
    ~IBasicBrushRenderer() = default;

    static const BrushId Id;
};

DECL_MAKEABLE(IBasicBrushRenderer)

#endif // IBASICBRUSHRENDERER_HPP