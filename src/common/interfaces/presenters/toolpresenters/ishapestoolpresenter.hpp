#ifndef ISHAPESTOOLPRESENTER_HPP
#define ISHAPESTOOLPRESENTER_HPP

#include <QList>

#include "interfaces/traits/compat.hpp"
#include "../assets/ishapepresenter.hpp"
#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class ADDLE_COMMON_EXPORT IShapesToolPresenter : public virtual IToolPresenter
{
public:
    static const ToolId ID;

    virtual ~IShapesToolPresenter() = default;
};

DECL_MAKEABLE(IShapesToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IShapesToolPresenter)

#endif // ISHAPESTOOLPRESENTER_HPP