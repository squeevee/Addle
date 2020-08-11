#include "canvaspresenter.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/tools/itoolpresenter.hpp"

#include "utilities/qtextensions/qobject.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"
using namespace Addle;

void CanvasPresenter::initialize(IMainEditorPresenter* mainEditorPresenter)
{
    _initHelper.initializeBegin();

    _mainEditorPresenter = mainEditorPresenter;

    connect_interface(
        _mainEditorPresenter,
        SIGNAL(currentToolChanged(ToolId)),
        this,
        SLOT(onEditorToolChanged())
    );

    _initHelper.initializeEnd();
}

QCursor CanvasPresenter::cursor()
{
    _initHelper.check();

    if (_mainEditorPresenter->currentToolPresenter())
        return _mainEditorPresenter->currentToolPresenter()->cursor();
    else
        return QCursor(Qt::ArrowCursor);
}

bool CanvasPresenter::event(QEvent* e)
{
    if (e->type() == CanvasMouseEvent::type())
    {
        IToolPresenter* tool = _mainEditorPresenter->currentToolPresenter();
        if (tool && qobject_interface_cast(tool)->event(e) && e->isAccepted())
            return true;

        // handle as the canvas
    }

    return QObject::event(e);
}

void CanvasPresenter::setHasMouse(bool value)
{
    if (_hasMouse != value)
    {
        _hasMouse = value;
        emit hasMouseChanged(_hasMouse);
    }
}

void CanvasPresenter::onEditorToolChanged()
{
    _initHelper.check();

    if (_connection_toolCursor)
        disconnect(_connection_toolCursor);
    
    IToolPresenter* tool = _mainEditorPresenter->currentToolPresenter();
    if (tool)
    {
        _connection_toolCursor = connect_interface(
            tool,
            SIGNAL(cursorChanged(QCursor)),
            this,
            SLOT(onToolCursorChanged())
        );

        emit cursorChanged(cursor());
    }
}


void CanvasPresenter::onToolCursorChanged()
{
    _initHelper.check();

    emit cursorChanged(cursor());
}