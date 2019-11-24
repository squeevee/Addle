#ifndef TOOLPRESENTER_HPP
#define TOOLPRESENTER_HPP

#include <QObject>

#include "common/interfaces/presenters/tools/itoolpresenter.hpp"

#include "common/utilities/initializehelper.hpp"

class ToolPresenterBase : public QObject, public virtual IToolPresenter
{
    Q_OBJECT
public:
    virtual ~ToolPresenterBase() = default;

    IHaveToolsPresenter* getOwner() { _initHelper.assertInitialized(); return _owner; }

protected:
    void initialize_p(IHaveToolsPresenter* owner);

private:
    InitializeHelper<ToolPresenterBase> _initHelper;
    IHaveToolsPresenter* _owner;
};

#endif // TOOLOPTIONSPRESENTER_HPP