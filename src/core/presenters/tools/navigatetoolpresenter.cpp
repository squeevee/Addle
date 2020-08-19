/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "navigatetoolpresenter.hpp"
#include <QtDebug>
#include <QMetaEnum>

#include "utils.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"

using namespace Addle;

using namespace INavigateToolPresenterAux;

void NavigateToolPresenter::initialize(IMainEditorPresenter* owner)
{
    const Initializer init(_initHelper);
    
    _owner = owner;
    _viewPort = &_owner->viewPortPresenter();
}

bool NavigateToolPresenter::event(QEvent* e)
{
    try
    {
        _mouseHelper.event(e);
    }
    ADDLE_EVENT_CATCH

    return e->isAccepted() || QObject::event(e);
}

void NavigateToolPresenter::setNavigateOperation(NavigateToolPresenter::NavigateOperationOptions operation)
{
    try
    {
        _initHelper.check();
        _operation = operation;
        _cache_cursor.recalculate();
        emit navigateOperationChanged(_operation);
    }
    ADDLE_SLOT_CATCH
}

void NavigateToolPresenter::onMove()
{
    try
    {
        switch (_operation)
        {
        case gripPan:
            _viewPort->gripPan(_mouseHelper.firstPosition(), _mouseHelper.latestPosition());
            break;
        case gripPivot:
            _viewPort->gripPivot(_mouseHelper.firstPosition(), _mouseHelper.latestPosition());
            break;
        }
    }
    ADDLE_SLOT_CATCH
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