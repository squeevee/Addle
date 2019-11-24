#ifndef ITEXTOPTIONSPRESENTER_HPP
#define ITEXTOPTIONSPRESENTER_HPP

#include "itooloptionspresenter.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"


class ITextOptionsPresenter : public virtual IToolOptionsPresenter, public virtual IMakeable
{
};

DECL_IMPLEMENTED_AS_QOBJECT(ITextOptionsPresenter)


#endif // ITEXTOPTIONSPRESENTER_HPP