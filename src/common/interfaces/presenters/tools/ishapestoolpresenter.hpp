#ifndef ISHAPESTOOLPRESENTER_HPP
#define ISHAPESTOOLPRESENTER_HPP

#include <QList>

#include "../assets/ishapepresenter.hpp"
#include "itoolpresenter.hpp"
#include "interfaces/traits.hpp"


namespace Addle {

class IShapesToolPresenter : public virtual IToolPresenter
{
public:
    static const ToolId SHAPES_TOOL_ID;

    virtual ~IShapesToolPresenter() = default;
};

DECL_MAKEABLE(IShapesToolPresenter)
//DECL_IMPLEMENTED_AS_QOBJECT(IShapesToolPresenter)

} // namespace Addle
#endif // ISHAPESTOOLPRESENTER_HPP