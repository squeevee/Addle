#ifndef MESSAGECONTEXT_HPP
#define MESSAGECONTEXT_HPP

#include <QObject>
#include "interfaces/presenters/imessagecontext.hpp"

namespace Addle {

class MessageContext : public QObject, public IMessageContext
{
    Q_OBJECT
    Q_INTERFACES(Addle::IMessageContext)
    IAMQOBJECT_IMPL
public:
    virtual ~MessageContext() = default;
    
public slots:
    void postMessage(QSharedPointer<Addle::IMessagePresenter> message) override;
    
signals:
    void messagePosted(QSharedPointer<Addle::IMessagePresenter> message);
    void messageShown() override;
    void messageClosed() override;
};

} // namespace Addle

#endif // MESSAGECONTEXT_HPP
