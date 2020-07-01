#ifndef NAVIGATETOOLPRESENTER_HPP
#define NAVIGATETOOLPRESENTER_HPP

#include "compat.hpp"
#include "toolpresenterbase.hpp"
#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

class ADDLE_CORE_EXPORT NavigateToolPresenter : public ToolPresenterBase, public virtual INavigateToolPresenter
{
    Q_OBJECT
    Q_PROPERTY(
        NavigateOperationOptions navigateOperation
        READ getNavigateOperation
        WRITE setNavigateOperation
        NOTIFY navigateOperationChanged
    )
public:
    NavigateToolPresenter()
        : _initHelper(this)
    {
        _icon = QIcon(":/icons/navigate.png");
    }
    virtual ~NavigateToolPresenter() = default;

    void initialize(IMainEditorPresenter* owner);

    ToolId getId() { return INavigateToolPresenterAux::ID; }

    NavigateOperationOptions getNavigateOperation() { _initHelper.check(); return _operation; }

public slots:
    void setNavigateOperation(NavigateOperationOptions operation);
signals:
    void navigateOperationChanged(NavigateOperationOptions operation);

protected:
    void onMove();

private:

    NavigateOperationOptions _operation = INavigateToolPresenterAux::DEFAULT_NAVIGATE_OPERATION_OPTION;

    InitializeHelper<NavigateToolPresenter> _initHelper;
};

#endif // NAVIGATETOOLPRESENTER_HPP