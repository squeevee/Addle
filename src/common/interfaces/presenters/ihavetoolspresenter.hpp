#ifndef IHAVETOOLSPRESENTER_HPP
#define IHAVETOOLSPRESENTER_HPP

#include <QList>
#include "interfaces/traits/qobject_trait.hpp"
#include "idtypes/toolid.hpp"

class IToolPresenter;
class IHaveToolsPresenter
{
public:
    virtual ~IHaveToolsPresenter() = default;

    virtual ToolId getCurrentTool() = 0;
    virtual void selectTool(ToolId tool) = 0;
    virtual QList<ToolId> getTools() = 0;

    virtual IToolPresenter* getToolPresenter(ToolId id) = 0;
    virtual IToolPresenter* getCurrentToolPresenter() = 0;

signals:
    virtual void currentToolChanged(ToolId tool) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IHaveToolsPresenter)

#endif //IHAVETOOLSPRESENTER_HPP
