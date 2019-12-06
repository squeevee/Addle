#ifndef ISTICKERSTOOLPRESENTER_HPP
#define ISTICKERSTOOLPRESENTER_HPP

#include <QList>

#include "../assets/istickerpresenter.hpp"
#include "itoolwithassetspresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/servicelocator/imakeable.hpp"

class IStickersToolPresenter : public virtual IToolWithAssetsPresenter, public virtual IMakeable
{
public:
    static const ToolId STICKERS_TOOL_ID;

    virtual ~IStickersToolPresenter() = default;
};

DECL_IMPLEMENTED_AS_QOBJECT(IStickersToolPresenter)

#endif // ISTICKERSTOOLPRESENTER_HPP