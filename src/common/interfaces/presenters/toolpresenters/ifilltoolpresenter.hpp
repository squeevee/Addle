#ifndef IFILLTOOLPRESENTER_HPP
#define IFILLTOOLPRESENTER_HPP

#include "isimilarregiontoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class IFillToolPresenter : public virtual ISimilarRegionToolPresenter
{
public:
    static const ToolId FILL_TOOL_ID;

    virtual ~IFillToolPresenter() = default;
};

DECL_MAKEABLE(IFillToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IFillToolPresenter)

#endif // IFILLTOOLPRESENTER_HPP