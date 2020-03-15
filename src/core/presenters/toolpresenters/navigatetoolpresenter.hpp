#ifndef NAVIGATETOOLPRESENTER_HPP
#define NAVIGATETOOLPRESENTER_HPP

#include "toolpresenterbase.hpp"
#include "interfaces/presenters/toolpresenters/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

class NavigateToolPresenter : public ToolPresenterBase, public virtual INavigateToolPresenter
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

    ToolId getId() { return NAVIGATE_TOOL_ID; }

    NavigateOperationOptions getNavigateOperation() { _initHelper.check(); return _operation; }

public slots:
    void setNavigateOperation(NavigateOperationOptions operation);
signals:
    void navigateOperationChanged(NavigateOperationOptions operation);

protected:
    void onPointerEngage();
    void onPointerMove();

private:

    NavigateOperationOptions _operation = DEFAULT_NAVIGATE_OPERATION_OPTION;

    InitializeHelper<NavigateToolPresenter> _initHelper;
};

#endif // NAVIGATETOOLPRESENTER_HPP