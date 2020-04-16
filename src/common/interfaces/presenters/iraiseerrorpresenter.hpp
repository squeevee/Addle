#ifndef IRAISEERRORPRESENTER_HPP
#define IRAISEERRORPRESENTER_HPP

#include "interfaces/traits/qobject_trait.hpp"

#include <QSharedPointer>
#include "ierrorpresenter.hpp"
#include "interfaces/traits/compat.hpp"

class ADDLE_COMMON_EXPORT IRaiseErrorPresenter
{
public:
    virtual ~IRaiseErrorPresenter() = default;
signals:
    virtual void raiseError(QSharedPointer<IErrorPresenter> error) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IRaiseErrorPresenter)
Q_DECLARE_METATYPE(QSharedPointer<IErrorPresenter>)

#endif // IRAISEERRORPRESENTER_HPP