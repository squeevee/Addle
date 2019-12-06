#ifndef ITEXTTOOLPRESENTER_HPP
#define ITEXTTOOLPRESENTER_HPP

#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/servicelocator/imakeable.hpp"

class ITextToolPresenter : public virtual IToolPresenter, public virtual IMakeable
{
public:
    static const ToolId TEXT_TOOL_ID;

    virtual ~ITextToolPresenter() = default;
};

DECL_IMPLEMENTED_AS_QOBJECT(ITextToolPresenter)


#endif // ITEXTTOOLPRESENTER_HPP