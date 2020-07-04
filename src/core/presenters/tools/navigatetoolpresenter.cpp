#include "navigatetoolpresenter.hpp"
#include <QtDebug>
#include <QMetaEnum>

#include "utilities/canvas/canvasmouseevent.hpp"

using namespace INavigateToolPresenterAux;

void NavigateToolPresenter::initialize(IMainEditorPresenter* owner,
        ICanvasPresenter* canvasPresenter,
        IViewPortPresenter* viewPort
    )
{
    _initHelper.initializeBegin();
    
    _owner = owner;
    _viewPort = viewPort;

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
        _viewPort->gripPan(_mouseHelper.getFirstPosition(), _mouseHelper.getLatestPosition());
        break;
    case gripPivot:
        _viewPort->gripPivot(_mouseHelper.getFirstPosition(), _mouseHelper.getLatestPosition());
        break;
    }
}