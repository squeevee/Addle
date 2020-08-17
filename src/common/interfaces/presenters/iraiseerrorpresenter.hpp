#ifndef IRAISEERRORPRESENTER_HPP
#define IRAISEERRORPRESENTER_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

#include <QSharedPointer>
#include "ierrorpresenter.hpp"
namespace Addle {

class IRaiseErrorPresenter : public virtual IAmQObject
{
public:
    virtual ~IRaiseErrorPresenter() = default;
signals:
    virtual void raiseError(QSharedPointer<IErrorPresenter> error) = 0;
};




} // namespace Addle
#endif // IRAISEERRORPRESENTER_HPP