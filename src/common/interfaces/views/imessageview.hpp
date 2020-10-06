#ifndef IMESSAGEVIEW_HPP
#define IMESSAGEVIEW_HPP

#include "interfaces/iamqobject.hpp"
#include "interfaces/traits.hpp"

#include "iviewfor.hpp"

namespace Addle {

class IMessagePresenter;
typedef IViewForShared<IMessagePresenter> IMessageView;

DECL_MAKEABLE(IMessageView);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IMessageView, "org.addle.IMessageView");

#endif // IMESSAGEVIEW_HPP
