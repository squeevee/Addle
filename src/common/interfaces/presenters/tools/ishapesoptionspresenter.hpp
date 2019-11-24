#ifndef ISHAPESOPTIONSPRESENTER_HPP
#define ISHAPESOPTIONSPRESENTER_HPP

#include <QList>

#include "../assets/ishapepresenter.hpp"
#include "itooloptionspresenter.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"

class IShapesOptionsPresenter : public virtual IToolOptionsPresenter, public virtual IMakeable
{
    
};

DECL_IMPLEMENTED_AS_QOBJECT(IShapesOptionsPresenter)

#endif // ISHAPESOPTIONSPRESENTER_HPP