#ifndef MESSAGEVIEWHELPER_HPP
#define MESSAGEVIEWHELPER_HPP

#include <QSharedPointer>

#include "interfaces/presenters/messages/imessagepresenter.hpp"
#include "interfaces/views/imessageview.hpp"
#include "interfaces/views/itoplevelview.hpp"

#include "utilities/errors.hpp"
#include "utilities/qobject.hpp"
#include "utilities/helpercallback.hpp"

#include "servicelocator.hpp"

namespace Addle {
    
class MessageViewHelper 
{
public:
    // TODO: make HelperArgCallback work with references (though pointers are
    // probably just fine in this case)
    HelperArgCallback<IMessageView*> onAnyMessageViewMade; 
    HelperArgCallback<IMessageView*> onNonUrgentViewMade;
    HelperArgCallback<ITopLevelView*> onUrgentViewMade;
    
    inline void onMessagePosted(QSharedPointer<IMessagePresenter> message)
    {
        auto view = ServiceLocator::make<IMessageView>(message);
        onAnyMessageViewMade(view);
        
        if (message->isUrgent())
        {
            auto tlv = qobject_interface_cast<ITopLevelView*>(view);
            ADDLE_ASSERT(tlv);
            
            onUrgentViewMade(tlv);
            tlv->tlv_open();
        }
        else
        {
            onNonUrgentViewMade(view);
        }
    }
};
    
} // namespace Addle

#endif // MESSAGEVIEWHELPER_HPP
