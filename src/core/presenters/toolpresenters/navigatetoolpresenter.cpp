#include "navigatetoolpresenter.hpp"
#include <QtDebug>
#include <QMetaEnum>

void NavigateToolPresenter::initialize(IDocumentPresenter* owner)
{
    _initHelper.initializeBegin();

    ToolPresenterBase::initialize_p(owner);

    _propertyDecorationHelper.initializeEnumProperty<NavigateOperationOptions>("navigateOperation");

    _initHelper.initializeEnd();
}

void NavigateToolPresenter::setNavigateOperation(NavigateToolPresenter::NavigateOperationOptions operation)
{
    _initHelper.assertInitialized();
    _operation = operation;
    emit navigateOperationChanged(_operation);
}


void NavigateToolPresenter::onPointerEngage()
{
    _initHelper.assertInitialized();
}

void NavigateToolPresenter::onPointerMove()
{
    _initHelper.assertInitialized();

    switch (_operation)
    {
    case NavigateOperationOptions::gripPan:
        _viewPortPresenter->gripPan(_toolPathHelper.getFirstCanvasPosition(), _toolPathHelper.getLastCanvasPosition());
        break;
    case NavigateOperationOptions::gripPivot:
        _viewPortPresenter->gripPivot(_toolPathHelper.getFirstCanvasPosition(), _toolPathHelper.getLastCanvasPosition());
        break;
    }
}
