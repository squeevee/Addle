#ifndef IHAVEUNDOSTACKPRESENTER_HPP
#define IHAVEUNDOSTACKPRESENTER_HPP

#include <QSharedPointer>

#include "interfaces/presenters/operations/iundooperationpresenter.hpp"

#include "interfaces/traits.hpp"
namespace Addle {

class IHaveUndoStackPresenter
{
public: 
    virtual ~IHaveUndoStackPresenter() = default;

    virtual bool canUndo() = 0;
    virtual bool canRedo() = 0;

    virtual void push(QSharedPointer<IUndoOperationPresenter> operation) = 0;

public slots: 
    virtual void undo() = 0;
    virtual void redo() = 0;

signals: 
    virtual void undoStateChanged() = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IHaveUndoStackPresenter)

} // namespace Addle
#endif // IHAVEUNDOSTACKPRESENTER_HPP