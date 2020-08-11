#ifndef ISTICKERSTOOLPRESENTER_HPP
#define ISTICKERSTOOLPRESENTER_HPP

#include <QList>

#include "../assets/istickerpresenter.hpp"
#include "interfaces/traits.hpp"


namespace Addle {

class IStickersToolPresenter //: public virtual IToolWithAssetsPresenter
{
public:
    static const ToolId STICKERS_TOOL_ID;

    virtual ~IStickersToolPresenter() = default;
};

DECL_MAKEABLE(IStickersToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IStickersToolPresenter)

} // namespace Addle
#endif // ISTICKERSTOOLPRESENTER_HPP