#ifndef ISHAPESTOOLPRESENTER_HPP
#define ISHAPESTOOLPRESENTER_HPP

#include <QList>

#include "../assets/ishapepresenter.hpp"
#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/servicelocator/imakeable.hpp"

class IShapesToolPresenter : public virtual IToolPresenter, public virtual IMakeable
{
public:
    static const ToolId SHAPES_TOOL_ID;

    virtual ~IShapesToolPresenter() = default;
};

DECL_IMPLEMENTED_AS_QOBJECT(IShapesToolPresenter)

#endif // ISHAPESTOOLPRESENTER_HPP