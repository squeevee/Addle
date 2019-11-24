#ifndef IHAVETOOLSPRESENTER_HPP
#define IHAVETOOLSPRESENTER_HPP

#include <QList>
#include "common/interfaces/traits/qobject_trait.hpp"

class IToolPresenter;
class IHaveToolsPresenter
{
public:
    virtual ~IHaveToolsPresenter() = default;

    virtual void setCurrentTool(IToolPresenter* tool) = 0;
    virtual IToolPresenter* getCurrentTool() = 0;
    virtual QList<IToolPresenter*> getTools() = 0;

signals:
    virtual void currentToolChanged(IToolPresenter* tool) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IHaveToolsPresenter)

#endif //IHAVETOOLSPRESENTER_HPP
