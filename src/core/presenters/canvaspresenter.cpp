/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "canvaspresenter.hpp"

#include "utils.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/tools/itoolpresenter.hpp"

#include "utilities/qobject.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"

using namespace Addle;

CanvasPresenter::CanvasPresenter(std::unique_ptr<IRenderer> renderer)
    : _renderer(std::move(renderer))
{
}

// void CanvasPresenter::initialize(IMainEditorPresenter& mainEditorPresenter)
// {
//     const Initializer init(_initHelper);
// 
//     _mainEditorPresenter = &mainEditorPresenter;
// 
//     connect_interface(
//         _mainEditorPresenter,
//         SIGNAL(currentToolChanged(ToolId)),
//         this,
//         SLOT(onEditorToolChanged())
//     );
// }
// 
// QCursor CanvasPresenter::cursor() const
// {
//     ASSERT_INIT();
// 
//     if (_mainEditorPresenter->currentToolPresenter())
//         return _mainEditorPresenter->currentToolPresenter()->cursor();
//     else
//         return QCursor(Qt::ArrowCursor);
// }
// 
// bool CanvasPresenter::event(QEvent* e)
// {
//     try 
//     {
//         ASSERT_INIT();
//         
//         if (e->type() == CanvasMouseEvent::type())
//         {
//             QSharedPointer<IToolPresenter> tool = _mainEditorPresenter->currentToolPresenter();
//             if (tool && sendInterfaceEvent(tool, e) && e->isAccepted())
//                 return true;
// 
//             handle as the canvas
//         }
//     }
//     ADDLE_EVENT_CATCH(e)
// 
//     return QObject::event(e);
// }
// 
// void CanvasPresenter::setHasMouse(bool value)
// {
//     ASSERT_INIT();
// 
//     if (_hasMouse != value)
//     {
//         _hasMouse = value;
//         emit hasMouseChanged(_hasMouse);
//     }
// }
// 
// void CanvasPresenter::onEditorToolChanged()
// {
//     try
//     {
//         ASSERT_INIT();
// 
//         if (_connection_toolCursor)
//             disconnect(_connection_toolCursor);
//         
//         QSharedPointer<IToolPresenter> tool = _mainEditorPresenter->currentToolPresenter();
//         if (tool)
//         {
//             _connection_toolCursor = connect_interface(
//                 tool,
//                 SIGNAL(cursorChanged(QCursor)),
//                 this,
//                 SLOT(onToolCursorChanged())
//             );
// 
//             emit cursorChanged(cursor());
//         }
//     }
//     ADDLE_SLOT_CATCH
// }
// 
// void CanvasPresenter::onToolCursorChanged()
// {
//     try
//     {
//         ASSERT_INIT();
//         emit cursorChanged(cursor());
//     }
//     ADDLE_SLOT_CATCH
// }
