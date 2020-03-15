#include "navigatetoolpresenter.hpp"
#include <QtDebug>
#include <QMetaEnum>

void NavigateToolPresenter::initialize(IMainEditorPresenter* owner)
{
    _initHelper.initializeBegin();

    ToolPresenterBase::initialize_p(owner);

    _propertyDecorationHelper.initializeEnumProperty<NavigateOperationOptions>("navigateOperation");

    _initHelper.initializeEnd();
}

void NavigateToolPresenter::setNavigateOperation(NavigateToolPresenter::NavigateOperationOptions operation)
{
    _initHelper.check();
    _operation = operation;
    emit navigateOperationChanged(_operation);
}


void NavigateToolPresenter::onPointerEngage()
{
    _initHelper.check();
}

void NavigateToolPresenter::onPointerMove()
{
    _initHelper.check();

    // switch (_operation)
    // {
    // case NavigateOperationOptions::gripPan:
    //     _viewPortPresenter->gripPan(_toolPathHelper.getFirstCanvasPosition(), _toolPathHelper.getLastCanvasPosition());
    //     break;
    // case NavigateOperationOptions::gripPivot:
    //     _viewPortPresenter->gripPivot(_toolPathHelper.getFirstCanvasPosition(), _toolPathHelper.getLastCanvasPosition());
    //     break;
    // }
}
