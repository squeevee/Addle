#ifndef IFILLTOOLPRESENTER_HPP
#define IFILLTOOLPRESENTER_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"


namespace Addle {


class IFillToolPresenter
{
public:
    static const ToolId FILL_TOOL_ID;

    virtual ~IFillToolPresenter() = default;
};

DECL_MAKEABLE(IFillToolPresenter)


} // namespace Addle
#endif // IFILLTOOLPRESENTER_HPP