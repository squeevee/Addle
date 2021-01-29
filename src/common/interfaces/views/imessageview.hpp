#ifndef IMESSAGEVIEW_HPP
#define IMESSAGEVIEW_HPP

#include "interfaces/iamqobject.hpp"
#include "interfaces/traits.hpp"

#include "iviewbase.hpp"

namespace Addle {

class IMessagePresenter;
using IMessageView = IViewBase<IMessagePresenter, aux_IViewBase::shared_presenter_tag>;

ADDLE_DECL_MAKEABLE(IMessageView);

namespace aux_IMessageView {
    using namespace aux_IViewBase; 
        // enables factory parameter "presenter"
}

ADDLE_VIEW_FACTORY_PARAMETERS(IMessageView)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IMessageView, "org.addle.IMessageView");

#endif // IMESSAGEVIEW_HPP
