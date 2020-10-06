#ifndef IMESSAGEPRESENTER_HPP
#define IMESSAGEPRESENTER_HPP

#include <QString>
#include "interfaces/views/imessageview.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {

class IMessageContext;
class IMessagePresenter : public virtual IAmQObject
{
public:
    enum Tone
    {
        Normal,
        Success,
        Issue,
        Problem
    };

    virtual ~IMessagePresenter() = default;

    virtual IMessageContext* context() const = 0;

    virtual void setContext(IMessageContext* context) = 0;
    
    virtual bool isUrgent() const = 0;
    virtual Tone tone() const = 0;

    virtual QString text() const = 0;
};

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IMessagePresenter, "org.addle.IMessagePresenter");
Q_DECLARE_METATYPE(QSharedPointer<Addle::IMessagePresenter>);

#endif // IMESSAGEPRESENTER_HPP
