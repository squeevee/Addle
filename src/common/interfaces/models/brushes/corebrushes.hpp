#ifndef ICOREBRUSHES_HPP
#define ICOREBRUSHES_HPP

#include "ibrush.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/makeable_trait.hpp"

namespace CoreBrushes
{

class ISmoothCircleBrush : public IBrush
{
public:
    static const BrushId Id;
};

} // namespace CoreBrushes

DECL_MAKEABLE(CoreBrushes::ISmoothCircleBrush)

#endif // ICOREBRUSHES_HPP