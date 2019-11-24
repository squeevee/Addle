#ifndef ISTICKERSOPTIONSPRESENTER_HPP
#define ISTICKERSOPTIONSPRESENTER_HPP

#include <QList>

#include "../assets/istickerpresenter.hpp"
#include "iassetoptionspresenter.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"


class IStickersOptionsPresenter : public virtual IAssetOptionsPresenter, public virtual IMakeable
{
};

DECL_IMPLEMENTED_AS_QOBJECT(IStickersOptionsPresenter)

#endif // ISTICKERSOPTIONSPRESENTER_HPP