/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef UNDOSTACKHELPER_HPP
#define UNDOSTACKHELPER_HPP

#include "interfaces/presenters/operations/iundooperationpresenter.hpp"
#include "interfaces/presenters/ihaveundostackpresenter.hpp"

#include "utilities/helpercallback.hpp"

#include <QStack>
namespace Addle {

class UndoStackHelper 
{
public:
    bool canUndo() const { return !_undoStack.isEmpty(); }
    bool canRedo() const { return !_redoStack.isEmpty(); }

    void push(QSharedPointer<IUndoOperationPresenter> operation)
    {
        _undoStack.push(operation);
        _redoStack.clear();
        operation->do_();
        emit undoStateChanged();
    }

    void undo()
    {
        QSharedPointer<IUndoOperationPresenter> operation = _undoStack.pop();
        _redoStack.push(operation);
        operation->undo();
        emit undoStateChanged();
    }

    void redo()
    {
        QSharedPointer<IUndoOperationPresenter> operation = _redoStack.pop();
        _undoStack.push(operation);
        operation->do_();
        emit undoStateChanged();
    }

    HelperCallback undoStateChanged;

    QStack<QSharedPointer<IUndoOperationPresenter>> _undoStack;
    QStack<QSharedPointer<IUndoOperationPresenter>> _redoStack;
};

} // namespace Addle
#endif // UNDOSTACKHELPER_HPP