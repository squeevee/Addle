#ifndef IBRUSHTOOLPRESENTER_HPP
#define IBRUSHTOOLPRESENTER_HPP

#include "compat.hpp"

#include "ibrushliketoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

namespace IBrushToolPresenterAux
{
    ADDLE_COMMON_EXPORT extern const ToolId ID;

    struct ADDLE_COMMON_EXPORT DefaultBrushes
    {
        static const BrushId Basic;
    };

    ADDLE_COMMON_EXPORT extern const BrushId DEFAULT_BRUSH; // DefaultBrushes::SmoothCircle
}

class IBrushToolPresenter : public virtual IBrushLikeToolPresenter
{
public:
    virtual ~IBrushToolPresenter() = default;

    virtual void initialize(IMainEditorPresenter* owner) = 0;
};

DECL_MAKEABLE(IBrushToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IBrushToolPresenter)
DECL_EXPECTS_INITIALIZE(IBrushToolPresenter)

#endif // IBRUSHTOOLPRESENTER_HPP