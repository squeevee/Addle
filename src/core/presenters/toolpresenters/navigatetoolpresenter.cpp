#include "navigatetoolpresenter.hpp"
#include <QtDebug>
#include <QMetaEnum>

#include "utilities/canvas/canvasmouseevent.hpp"

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

void NavigateToolPresenter::onMove()
{
    switch (_operation)
    {
    case gripPan:
        _viewPortPresenter->gripPan(_mouseHelper.getFirstPosition(), _mouseHelper.getLatestPosition());
        break;
    case gripPivot:
        _viewPortPresenter->gripPivot(_mouseHelper.getFirstPosition(), _mouseHelper.getLatestPosition());
        break;
    }
}