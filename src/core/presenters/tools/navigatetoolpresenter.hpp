#ifndef NAVIGATETOOLPRESENTER_HPP
#define NAVIGATETOOLPRESENTER_HPP

#include "toolpresenterbase.hpp"
#include "common/interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "common/interfaces/presenters/iviewportpresenter.hpp"

#include "../helpers/draghelper.hpp"

class NavigateToolPresenter : public ToolPresenterBase, public virtual INavigateToolPresenter
{
    Q_OBJECT
public:
    NavigateToolPresenter() : _initHelper(this) { }
    virtual ~NavigateToolPresenter() = default;

    void initialize(IDocumentPresenter* owner);

    NavigateOperationOptions getNavigateOperation() { _initHelper.assertInitialized(); return _operation; }
    void setNavigateOperation(NavigateOperationOptions operation)
    {
        _initHelper.assertInitialized();
        _operation = operation;
        emit navigateOperationChanged(_operation);
    }
    
    void pointerEngage(QPoint pos);
    void pointerMove(QPoint pos);
    void pointerDisengage(QPoint pos);

signals:
    virtual void navigateOperationChanged(NavigateOperationOptions operation);

private:

    NavigateOperationOptions _operation = DEFAULT_NAVIGATE_OPERATION_OPTION;

    IViewPortPresenter* _viewPortPresenter = nullptr;
    IDocumentPresenter* _documentPresenter;

    DragHelper _dragHelper;
    InitializeHelper<NavigateToolPresenter> _initHelper;

    QPointF _anchor;
};

#endif // NAVIGATETOOLPRESENTER_HPP