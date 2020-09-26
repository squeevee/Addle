#ifndef IMESSAGECONTEXT_HPP
#define IMESSAGECONTEXT_HPP

#include <QSharedPointer>

#include "interfaces/iamqobject.hpp"

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
};

} // namespace Addle

#endif // IMESSAGECONTEXT_HPP
