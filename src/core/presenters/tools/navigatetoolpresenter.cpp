#include "navigatetoolpresenter.hpp"
#include <QtDebug>
#include <QMetaEnum>

void NavigateToolPresenter::initialize(IDocumentPresenter* owner)
{
    _initHelper.initializeBegin();

    ToolPresenterBase::initialize_p(owner);

    _translationContext = "NavigateToolPresenter";

    initializeEnumOptionText<NavigateOperationOptions>("navigateOperation");

    _initHelper.initializeEnd();
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
