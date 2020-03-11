#ifndef IVIEWPORT_HPP
#define IVIEWPORT_HPP

#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/initialize_traits.hpp"

#include "interfaces/presenters/iviewportpresenter.hpp"

#include <QWeakPointer>

class IViewPort
{
public: 
    virtual ~IViewPort() = default;

    virtual void initialize(IViewPortPresenter* presenter, ICanvasPresenter* canvasPresenter) = 0;
};

DECL_MAKEABLE(IViewPort);
DECL_EXPECTS_INITIALIZE(IViewPort);

#endif // IVIEWPORT_HPP