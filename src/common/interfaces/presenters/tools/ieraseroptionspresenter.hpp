#ifndef IERASEROPTIONSPRESENTER_HPP
#define IERASEROPTIONSPRESENTER_HPP

#include "ibrushlikeoptionspresenter.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"


class IEraserOptionsPresenter : public virtual IBrushLikeOptionsPresenter, public virtual IMakeable
{
};

DECL_IMPLEMENTED_AS_QOBJECT(IEraserOptionsPresenter)

#endif // IERASEROPTIONSPRESENTER_HPP