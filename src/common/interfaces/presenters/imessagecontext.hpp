#ifndef IMESSAGECONTEXT_HPP
#define IMESSAGECONTEXT_HPP

#include <QSharedPointer>

#include "interfaces/iamqobject.hpp"
#include "interfaces/traits.hpp"

namespace Addle {

class IMessagePresenter;
class IMessageContext : public virtual IAmQObject
{
public:
    virtual ~IMessageContext() = default;
    
public slots:
    virtual void postMessage(QSharedPointer<IMessagePresenter> message) = 0;
    
signals:
    virtual void messagePosted(QSharedPointer<IMessagePresenter> message) = 0;
    virtual void messageShown() = 0;
    virtual void messageClosed() = 0;
};

DECL_MAKEABLE(IMessageContext)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IMessageContext, "org.addle.IMessageContext")

#endif // IMESSAGECONTEXT_HPP
