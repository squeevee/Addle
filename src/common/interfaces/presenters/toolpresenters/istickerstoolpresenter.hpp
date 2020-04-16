#ifndef ISTICKERSTOOLPRESENTER_HPP
#define ISTICKERSTOOLPRESENTER_HPP

#include <QList>

#include "interfaces/traits/compat.hpp"
#include "../assets/istickerpresenter.hpp"
#include "itoolwithassetspresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class ADDLE_COMMON_EXPORT IStickersToolPresenter : public virtual IToolWithAssetsPresenter
{
public:
    static const ToolId ID;

    virtual ~IStickersToolPresenter() = default;
};

DECL_MAKEABLE(IStickersToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IStickersToolPresenter)

#endif // ISTICKERSTOOLPRESENTER_HPP