#ifndef ISHAPESTOOLPRESENTER_HPP
#define ISHAPESTOOLPRESENTER_HPP

#include <QList>

#include "../assetpresenters/ishapepresenter.hpp"
#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class IShapesToolPresenter : public virtual IToolPresenter
{
public:
    static const ToolId SHAPES_TOOL_ID;

    virtual ~IShapesToolPresenter() = default;
};

DECL_MAKEABLE(IShapesToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IShapesToolPresenter)

#endif // ISHAPESTOOLPRESENTER_HPP