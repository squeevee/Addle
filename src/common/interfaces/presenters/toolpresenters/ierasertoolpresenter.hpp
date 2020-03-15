#ifndef IERASERTOOLPRESENTER_HPP
#define IERASERTOOLPRESENTER_HPP

#include "ibrushliketoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"


class IEraserToolPresenter : public virtual IBrushLikeToolPresenter
{
public:
    static const ToolId ERASER_TOOL_ID;
        
    struct DefaultBrushes
    {
        static const BrushId SmoothCircle;
        static const BrushId AliasedCircle;
        static const BrushId Square;
    };

    virtual ~IEraserToolPresenter() = default;
};

DECL_MAKEABLE(IEraserToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IEraserToolPresenter)

#endif // IERASERTOOLPRESENTER_HPP