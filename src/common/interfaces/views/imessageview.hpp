#ifndef IMESSAGEVIEW_HPP
#define IMESSAGEVIEW_HPP

#include "itoplevelview.hpp"

#include "interfaces/presenters/messages/imessagepresenter.hpp"
#include "interfaces/presenters/messages/inotificationpresenter.hpp"
#include "interfaces/presenters/messages/ifileissuepresenter.hpp"

namespace Addle {

class IMessagePresenter;
class IMessageView : public ITopLevelView
{
public:
    virtual ~IMessageView() = default;

    virtual void initialize(QSharedPointer<IMessagePresenter> presenter) = 0;
    
    virtual QSharedPointer<IMessagePresenter> presenter() const = 0;
};

DECL_MAKEABLE(IMessageView);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IMessageView, "org.addle.IMessageView");

#endif // IMESSAGEVIEW_HPP
