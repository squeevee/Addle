#ifndef IERASERTOOLPRESENTER_HPP
#define IERASERTOOLPRESENTER_HPP

#include "interfaces/traits/compat.hpp"

#include "ibrushliketoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"


class ADDLE_COMMON_EXPORT IEraserToolPresenter : public virtual IBrushLikeToolPresenter
{
public:
    static const ToolId ID;

    virtual ~IEraserToolPresenter() = default;
};

DECL_MAKEABLE(IEraserToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IEraserToolPresenter)

#endif // IERASERTOOLPRESENTER_HPP