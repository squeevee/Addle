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

    static const BrushTipId DEFAULT_BRUSH_TIP; // CommonBrushTips::HardCircle
    static const SizeOption DEFAULT_BRUSH_SIZE = SizeOption::_25px;
    
    virtual ~IBrushToolPresenter() = default;

    virtual void initialize(IEditorPresenter* owner) = 0;
};

DECL_MAKEABLE(IBrushToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IBrushToolPresenter)
DECL_EXPECTS_INITIALIZE(IBrushToolPresenter)

#endif // IBRUSHTOOLPRESENTER_HPP