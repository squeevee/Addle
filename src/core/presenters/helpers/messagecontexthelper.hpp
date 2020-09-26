#ifndef MESSAGECONTEXTHELPER_HPP
#define MESSAGECONTEXTHELPER_HPP

#include <QSharedPointer>
#include <QObject>

#include "utilities/helpercallback.hpp"

#include "interfaces/presenters/imessagecontext.hpp"
#include "interfaces/presenters/messages/imessagepresenter.hpp"
#include "interfaces/views/imessageview.hpp"

#include "utilities/qobject.hpp"

namespace Addle {

class MessageContextHelper : public QObject
{
    Q_OBJECT
public:
    MessageContextHelper(IMessageContext& context)
        : _context(context)
    {
    }
    virtual ~MessageContextHelper() = default;

    void postMessage(QSharedPointer<IMessagePresenter> message)
    {
        message->setContext(&_context);
        if (message->isUrgent())
        {
            //connect_interface(message, SIGNAL(shown()), this, SLOT(onMessageShown()));
            //connect_interface(message, SIGNAL(closed()), this, SLOT(onMessageClosed()));
        }

        onMessagePosted(message);
    }

    HelperArgCallback<QSharedPointer<IMessagePresenter>> onMessagePosted;

public slots:
    void onMessageShown()
    {
        // callbacks to block UI
    }

    void onMessageClosed()
    {
        // callbacks to unblock UI
    }

private:
    IMessageContext& _context;
};

} // namespace Addle

#endif // MESSAGECONTEXTHELPER_HPP
