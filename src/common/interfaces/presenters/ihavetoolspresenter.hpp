#ifndef IHAVETOOLSPRESENTER_HPP
#define IHAVETOOLSPRESENTER_HPP

#include <QList>
#include "interfaces/traits.hpp"
#include "idtypes/toolid.hpp"
namespace Addle {

class IToolPresenter;
class IHaveToolsPresenter
{
public:
    virtual ~IHaveToolsPresenter() = default;

    virtual ToolId currentTool() = 0;
    virtual void selectTool(ToolId tool) = 0;
    virtual QList<ToolId> tools() = 0;

    virtual IToolPresenter* toolPresenter(ToolId id) = 0;
    virtual IToolPresenter* currentToolPresenter() = 0;

signals:
    virtual void currentToolChanged(ToolId tool) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IHaveToolsPresenter)

} // namespace Addle
#endif //IHAVETOOLSPRESENTER_HPP
