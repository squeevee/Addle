#ifndef IFILLOPTIONSPRESENTER_HPP
#define IFILLOPTIONSPRESENTER_HPP

#include "isimilarregionoptionspresenter.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"

class IFillOptionsPresenter : public virtual ISimilarRegionOptionsPresenter, public virtual IMakeable
{
};

DECL_IMPLEMENTED_AS_QOBJECT(IFillOptionsPresenter)

#endif // IFILLOPTIONSPRESENTER_HPP