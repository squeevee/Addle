#include "canvaspresenter.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"

#include "utilities/qtextensions/qobject.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"

void CanvasPresenter::initialize(IMainEditorPresenter* mainEditorPresenter)
{
    _initHelper.initializeBegin();

    _mainEditorPresenter = mainEditorPresenter;

    _initHelper.initializeEnd();
}

bool CanvasPresenter::event(QEvent* e)
{
    if (e->type() == CanvasMouseEvent::getType())
    {
        IToolPresenter* tool = _mainEditorPresenter->getCurrentToolPresenter();
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
