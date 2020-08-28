
#ifndef IAPPLICATIONERRORPRESENTER_HPP
#define IAPPLICATIONERRORPRESENTER_HPP

#include <QSharedPointer>

#include "interfaces/traits.hpp"
#include "ierrorpresenter.hpp"

namespace Addle {

class IApplicationErrorView;
class AddleException;
class IApplicationErrorPresenter : public IErrorPresenter
{
public:
    virtual ~IApplicationErrorPresenter() = default;

    virtual void initialize(QSharedPointer<AddleException> error) = 0;

    virtual IApplicationErrorView& view() const = 0;
};

DECL_MAKEABLE(IApplicationErrorPresenter)
DECL_EXPECTS_INITIALIZE(IApplicationErrorPresenter)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IApplicationErrorPresenter, "org.addle.IApplicationErrorPresenter");

#endif // IAPPLICATIONERRORPRESENTER_HPP