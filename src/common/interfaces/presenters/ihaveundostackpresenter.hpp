#ifndef IHAVEUNDOSTACKPRESENTER_HPP
#define IHAVEUNDOSTACKPRESENTER_HPP

#include <QSharedPointer>

#include "interfaces/presenters/operationpresenters/iundooperationpresenter.hpp"

#include "interfaces/traits/compat.hpp"
#include "interfaces/traits/qobject_trait.hpp"

class ADDLE_COMMON_EXPORT IHaveUndoStackPresenter
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

#endif // IHAVEUNDOSTACKPRESENTER_HPP