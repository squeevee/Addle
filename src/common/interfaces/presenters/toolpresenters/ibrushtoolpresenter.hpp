#ifndef IBRUSHTOOLPRESENTER_HPP
#define IBRUSHTOOLPRESENTER_HPP

#include "ibrushliketoolpresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/presenters/ieditorpresenter.hpp"

class IBrushToolPresenter : public virtual IBrushLikeToolPresenter
{
public:
    static const ToolId BRUSH_TOOL_ID;

    struct DefaultBrushes
    {
        static const BrushId Basic;
    };

    static const BrushId DEFAULT_BRUSH; // DefaultBrushes::SmoothCircle
    
    virtual ~IBrushToolPresenter() = default;

    virtual void initialize(IEditorPresenter* owner) = 0;
};

DECL_MAKEABLE(IBrushToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IBrushToolPresenter)
DECL_EXPECTS_INITIALIZE(IBrushToolPresenter)

#endif // IBRUSHTOOLPRESENTER_HPP