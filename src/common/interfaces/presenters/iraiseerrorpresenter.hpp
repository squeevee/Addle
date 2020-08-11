#ifndef IRAISEERRORPRESENTER_HPP
#define IRAISEERRORPRESENTER_HPP

#include "interfaces/traits.hpp"

#include <QSharedPointer>
#include "ierrorpresenter.hpp"

class IRaiseErrorPresenter
{
public:
    virtual ~IRaiseErrorPresenter() = default;
signals:
    virtual void raiseError(QSharedPointer<IErrorPresenter> error) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IRaiseErrorPresenter)
Q_DECLARE_METATYPE(QSharedPointer<IErrorPresenter>)

#endif // IRAISEERRORPRESENTER_HPP