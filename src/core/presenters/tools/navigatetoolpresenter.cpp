#include "navigatetoolpresenter.hpp"
#include <QtDebug>
#include <QMetaEnum>

#include "utilities/canvas/canvasmouseevent.hpp"
using namespace Addle;

using namespace INavigateToolPresenterAux;

void NavigateToolPresenter::initialize(IMainEditorPresenter* owner)
{
    _initHelper.initializeBegin();
    
    _owner = owner;
    _viewPort = _owner->getViewPortPresenter();

    _initHelper.initializeEnd();
}

void NavigateToolPresenter::setNavigateOperation(NavigateToolPresenter::NavigateOperationOptions operation)
{
    _initHelper.check();
    _operation = operation;
    _cache_cursor.recalculate();
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

QCursor NavigateToolPresenter::cursor_p()
{
    switch (_operation)
    {
    case gripPan:
    case gripPivot:
        if (_mouseHelper.isEngaged())
            return QCursor(Qt::ClosedHandCursor);
        else
            return QCursor(Qt::OpenHandCursor);
    }

    return QCursor();
}