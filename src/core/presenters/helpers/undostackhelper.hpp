#ifndef UNDOSTACKHELPER_HPP
#define UNDOSTACKHELPER_HPP

#include "interfaces/editing/iundoableoperation.hpp"
#include "interfaces/presenters/ihaveundostackpresenter.hpp"

#include <QStack>

class UndoStackHelper 
{
public:
    UndoStackHelper(IHaveUndoStackPresenter& presenter)
        : _presenter(presenter)
    {
    }

    QStack<QSharedPointer<IUndoableOperation>> getUndoStack() { return _undoStack; }
    QStack<QSharedPointer<IUndoableOperation>> getRedoStack() { return _redoStack; }

    bool canUndo() { return !_undoStack.isEmpty(); }
    bool canRedo() { return !_redoStack.isEmpty(); }

    void doOperation(QSharedPointer<IUndoableOperation> undoable)
    {
        _undoStack.push(undoable);
        _redoStack.clear();
        undoable->doOperation();
        emit _presenter.undoStateChanged();
    }

    void undo()
    {
        QSharedPointer<IUndoableOperation> undoable = _undoStack.pop();
        _redoStack.push(undoable);
        undoable->undoOperation();
        emit _presenter.undoStateChanged();
    }

    void redo()
    {
        QSharedPointer<IUndoableOperation> undoable = _redoStack.pop();
        _undoStack.push(undoable);
        undoable->doOperation();
        emit _presenter.undoStateChanged();
    }

private:
    QStack<QSharedPointer<IUndoableOperation>> _undoStack;
    QStack<QSharedPointer<IUndoableOperation>> _redoStack;

    IHaveUndoStackPresenter& _presenter;
};

#endif // UNDOSTACKHELPER_HPP