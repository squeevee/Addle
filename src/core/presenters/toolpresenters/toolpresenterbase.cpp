#include "toolpresenterbase.hpp"

#include "idtypes/brushid.hpp"

#include "utilities/canvas/canvasmouseevent.hpp"

void ToolPresenterBase::initialize_p(IMainEditorPresenter* mainEditorPresenter)
{
    _initHelper.initializeBegin();

    _mainEditorPresenter = mainEditorPresenter;
    _viewPortPresenter = _mainEditorPresenter->getViewPortPresenter();
    _canvasPresenter = _mainEditorPresenter->getCanvasPresenter();

    _initHelper.initializeEnd();
}

void ToolPresenterBase::select()
{
    _mainEditorPresenter->selectTool(getId());
}

QCursor ToolPresenterBase::getCursorHint()
{
    return QCursor();
}

bool ToolPresenterBase::event(QEvent* e)
{
    if (e->type() == CanvasMouseEvent::getType())
    {
        CanvasMouseEvent* canvasMouseEvent = static_cast<CanvasMouseEvent*>(e);

        if (canvasMouseEvent->button() == Qt::RightButton)
        {
            // cancel tool operation
            _mouseHelper.disengage();
            return true;
        }

        switch (canvasMouseEvent->action())
        {
        case CanvasMouseEvent::down:
            if (_mouseHelper.engage(canvasMouseEvent->pos()))
                onEngage();
            break;

        case CanvasMouseEvent::move:
            if (_mouseHelper.move(canvasMouseEvent->pos()))
                onMove();
            break;

        case CanvasMouseEvent::up:
            if (_mouseHelper.move(canvasMouseEvent->pos()))
                onMove();
            if (_mouseHelper.disengage())
                onDisengage();
            break;
        }

        return true;
    }

    return QObject::event(e);
}