#ifndef IFILLTOOLPRESENTER_HPP
#define IFILLTOOLPRESENTER_HPP

#include "interfaces/traits.hpp"


namespace Addle {


class IFillToolPresenter
{
public:
    static const ToolId FILL_TOOL_ID;

    virtual ~IFillToolPresenter() = default;
};

DECL_MAKEABLE(IFillToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IFillToolPresenter)

} // namespace Addle
#endif // IFILLTOOLPRESENTER_HPP