#ifndef IUNDOOPERATIONPRESENTER_HPP
#define IUNDOOPERATIONPRESENTER_HPP

#include <QString>
#include <QSharedPointer>

#include "interfaces/traits/qobject_trait.hpp"

class IUndoOperationPresenter
{
public: 
    virtual ~IUndoOperationPresenter() = default;

    virtual QString getText() = 0;

    virtual bool isFinalized() = 0;
    virtual void finalize() = 0;

public slots: 
    virtual void do_() = 0;
    virtual void undo() = 0;

signals: 
    virtual void isFinalizedChanged(bool isFinalized) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IUndoOperationPresenter);

#endif // IUNDOOPERATIONPRESENTER_HPP