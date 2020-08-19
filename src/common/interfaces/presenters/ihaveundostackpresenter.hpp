/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IHAVEUNDOSTACKPRESENTER_HPP
#define IHAVEUNDOSTACKPRESENTER_HPP

#include <QSharedPointer>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {

class IUndoOperationPresenter;

class IHaveUndoStackPresenter : public virtual IAmQObject
{
public: 
    virtual ~IHaveUndoStackPresenter() = default;

    virtual bool canUndo() const = 0;
    virtual bool canRedo() const = 0;

    virtual void push(QSharedPointer<IUndoOperationPresenter> operation) = 0;

public slots: 
    virtual void undo() = 0;
    virtual void redo() = 0;

signals: 
    virtual void undoStateChanged() = 0;
};

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IHaveUndoStackPresenter, "org.addle.IHaveUndoPresenter")

#endif // IHAVEUNDOSTACKPRESENTER_HPP