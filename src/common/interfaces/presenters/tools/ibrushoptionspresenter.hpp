#ifndef IBRUSHOPTIONSPRESENTER_HPP
#define IBRUSHOPTIONSPRESENTER_HPP

#include "ibrushlikeoptionspresenter.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"

class IBrushOptionsPresenter : public virtual IBrushLikeOptionsPresenter, public virtual IMakeable
{
};

DECL_IMPLEMENTED_AS_QOBJECT(IBrushOptionsPresenter)

#endif // IBRUSHOPTIONSPRESENTER_HPP