/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IRAISEERRORPRESENTER_HPP
#define IRAISEERRORPRESENTER_HPP

#include <QSharedPointer>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {

class IErrorPresenter;

class IRaiseErrorPresenter : public virtual IAmQObject
{
public:
    virtual ~IRaiseErrorPresenter() = default;
    
signals:
    virtual void raiseError(QSharedPointer<IErrorPresenter> error) = 0;
};

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IRaiseErrorPresenter, "org.addle.IRaiseErrorPresenter")

#endif // IRAISEERRORPRESENTER_HPP