#ifndef IFILLTOOLPRESENTER_HPP
#define IFILLTOOLPRESENTER_HPP

#include "isimilarregiontoolpresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/servicelocator/imakeable.hpp"
#include "interfaces/servicelocator/imakeable.hpp"

class IFillToolPresenter : public virtual ISimilarRegionToolPresenter, public virtual IMakeable
{
public:
    static const ToolId FILL_TOOL_ID;

    virtual ~IFillToolPresenter() = default;
};

DECL_IMPLEMENTED_AS_QOBJECT(IFillToolPresenter)

#endif // IFILLTOOLPRESENTER_HPP