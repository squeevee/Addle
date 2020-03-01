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
        : ToolPresenterBase(ToolPathHelper::TrackingOptions::endpoints),
        _initHelper(this)
    {
        _icon = QIcon(":/icons/navigate.png");
    }
    virtual ~NavigateToolPresenter() = default;

    void initialize(IDocumentPresenter* owner);

    ToolId getId() { return NAVIGATE_TOOL_ID; }

    NavigateOperationOptions getNavigateOperation() { _initHelper.assertInitialized(); return _operation; }

public slots:
    void setNavigateOperation(NavigateOperationOptions operation)
    {
        _initHelper.assertInitialized();
        _operation = operation;
        emit navigateOperationChanged(_operation);
    }

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