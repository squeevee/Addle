#ifndef ICANVASVIEW_HPP
#define ICANVASVIEW_HPP

#include "common/interfaces/servicelocator/imakeable.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"

class ICanvasPresenter;
class ICanvasView: public virtual IMakeable
{
public:
    virtual ~ICanvasView() = default;

    virtual void initialize(ICanvasPresenter* presenter) = 0;
public slots:
    virtual void update() = 0;
};

DECL_EXPECTS_INITIALIZE(ICanvasView)
DECL_IMPLEMENTED_AS_QOBJECT(ICanvasView)

#endif // ICANVASVIEW_HPP