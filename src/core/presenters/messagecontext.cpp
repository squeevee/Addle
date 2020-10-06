#include "messagecontext.hpp"
#include "interfaces/presenters/messages/imessagepresenter.hpp"

using namespace Addle;

void MessageContext::postMessage(QSharedPointer<IMessagePresenter> message)
{
    message->setContext(this);
    if (message->isUrgent())
    {
        //connect_interface(message, SIGNAL(shown()), this, SLOT(onMessageShown()));
        //connect_interface(message, SIGNAL(closed()), this, SLOT(onMessageClosed()));
    }

    messagePosted(message);
}
