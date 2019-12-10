#ifndef IVIEWPORTVIEW_HPP
#define IVIEWPORTVIEW_HPP

#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

class IViewPort
{
public:
    virtual ~IViewPort() = default;

    virtual void initialize(IViewPortPresenter* presenter) = 0;
};

DECL_MAKEABLE(IViewPort)
DECL_EXPECTS_INITIALIZE(IViewPort)
DECL_INIT_DEPENDENCY(IViewPort, IViewPortPresenter)

#endif // IVIEWPORTVIEW_HPP