#ifndef UNDOSTACKHELPER_HPP
#define UNDOSTACKHELPER_HPP

#include "interfaces/presenters/ihaveundostackpresenter.hpp"

#include <QStack>
namespace Addle {

class UndoStackHelper 
{
public:
    UndoStackHelper(IHaveUndoStackPresenter& presenter)
        : _presenter(presenter)
    {
    }

    bool canUndo() { return !_undoStack.isEmpty(); }
    bool canRedo() { return !_redoStack.isEmpty(); }

    void push(QSharedPointer<IUndoOperationPresenter> operation)
    {
        _undoStack.push(operation);
        _redoStack.clear();
        operation->do_();
        emit _presenter.undoStateChanged();
    }

    void undo()
    {
        QSharedPointer<IUndoOperationPresenter> operation = _undoStack.pop();
        _redoStack.push(operation);
        operation->undo();
        emit _presenter.undoStateChanged();
    }

    void redo()
    {
        QSharedPointer<IUndoOperationPresenter> operation = _redoStack.pop();
        _undoStack.push(operation);
        operation->do_();
        emit _presenter.undoStateChanged();
    }

    QStack<QSharedPointer<IUndoOperationPresenter>> _undoStack;
    QStack<QSharedPointer<IUndoOperationPresenter>> _redoStack;

private:
    IHaveUndoStackPresenter& _presenter;
};

} // namespace Addle
#endif // UNDOSTACKHELPER_HPP