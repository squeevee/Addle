#include "canvaspresenter.hpp"

#include "utils.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/tools/itoolpresenter.hpp"

#include "utilities/qtextensions/qobject.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"

using namespace Addle;

void CanvasPresenter::initialize(IMainEditorPresenter& mainEditorPresenter)
{
    const Initializer init(_initHelper);

    _mainEditorPresenter = &mainEditorPresenter;

    connect_interface(
        _mainEditorPresenter,
        SIGNAL(currentToolChanged(ToolId)),
        this,
        SLOT(onEditorToolChanged())
    );
}

QCursor CanvasPresenter::cursor() const
{
    _initHelper.check();

    if (_mainEditorPresenter->currentToolPresenter())
        return _mainEditorPresenter->currentToolPresenter()->cursor();
    else
        return QCursor(Qt::ArrowCursor);
}

bool CanvasPresenter::event(QEvent* e)
{
    try 
    {
        _initHelper.check();
        
        if (e->type() == CanvasMouseEvent::type())
        {
            QSharedPointer<IToolPresenter> tool = _mainEditorPresenter->currentToolPresenter();
            if (tool && qobject_interface_cast(tool)->event(e) && e->isAccepted())
                return true;

            // handle as the canvas
        }
    }
    ADDLE_EVENT_CATCH

    return QObject::event(e);
}

void CanvasPresenter::setHasMouse(bool value)
{
    _initHelper.check();

    if (_hasMouse != value)
    {
        _hasMouse = value;
        emit hasMouseChanged(_hasMouse);
    }
}

void CanvasPresenter::onEditorToolChanged()
{
    try
    {
        _initHelper.check();

        if (_connection_toolCursor)
            disconnect(_connection_toolCursor);
        
        QSharedPointer<IToolPresenter> tool = _mainEditorPresenter->currentToolPresenter();
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
    ADDLE_SLOT_CATCH
}

void CanvasPresenter::onToolCursorChanged()
{
    try
    {
        _initHelper.check();
        emit cursorChanged(cursor());
    }
    ADDLE_SLOT_CATCH
}