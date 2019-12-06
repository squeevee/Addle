#ifndef IERASERTOOLPRESENTER_HPP
#define IERASERTOOLPRESENTER_HPP

#include "ibrushliketoolpresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/servicelocator/imakeable.hpp"


class IEraserToolPresenter : public virtual IBrushLikeToolPresenter, public virtual IMakeable
{
public:
    static const ToolId ERASER_TOOL_ID;
    
    virtual ~IEraserToolPresenter() = default;
};

DECL_IMPLEMENTED_AS_QOBJECT(IEraserToolPresenter)

#endif // IERASERTOOLPRESENTER_HPP