#ifndef ITEXTTOOLPRESENTER_HPP
#define ITEXTTOOLPRESENTER_HPP

#include "interfaces/traits/compat.hpp"
#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class ADDLE_COMMON_EXPORT ITextToolPresenter : public virtual IToolPresenter
{
public:
    static const ToolId ID;

    virtual ~ITextToolPresenter() = default;
};

DECL_MAKEABLE(ITextToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(ITextToolPresenter)


#endif // ITEXTTOOLPRESENTER_HPP