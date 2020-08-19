/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IUNDOOPERATIONPRESENTER_HPP
#define IUNDOOPERATIONPRESENTER_HPP

#include <QString>
#include <QSharedPointer>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"
namespace Addle {

class IUndoOperationPresenter : public virtual IAmQObject
{
public: 
    virtual ~IUndoOperationPresenter() = default;

    virtual QString text() = 0;

public slots: 
    virtual void do_() = 0;
    virtual void undo() = 0;
};



} // namespace Addle
#endif // IUNDOOPERATIONPRESENTER_HPP