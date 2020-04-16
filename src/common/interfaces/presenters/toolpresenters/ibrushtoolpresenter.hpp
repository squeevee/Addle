#ifndef IBRUSHTOOLPRESENTER_HPP
#define IBRUSHTOOLPRESENTER_HPP

#include "interfaces/traits/compat.hpp"
#include "ibrushliketoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class ADDLE_COMMON_EXPORT IBrushToolPresenter : public virtual IBrushLikeToolPresenter
{
public:
    static const ToolId ID;

    virtual ~IBrushToolPresenter() = default;

    virtual void initialize(IMainEditorPresenter* owner) = 0;
};

DECL_MAKEABLE(IBrushToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IBrushToolPresenter)
DECL_EXPECTS_INITIALIZE(IBrushToolPresenter)

#endif // IBRUSHTOOLPRESENTER_HPP