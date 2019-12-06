#ifndef ICANVASVIEW_HPP
#define ICANVASVIEW_HPP

#include "interfaces/servicelocator/imakeable.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/initialize_traits.hpp"

class IDocumentPresenter;
class ICanvasView: public virtual IMakeable
{
public:
    virtual ~ICanvasView() = default;

    virtual void initialize(IDocumentPresenter* presenter) = 0;
public slots:
    virtual void update() = 0;
};

DECL_EXPECTS_INITIALIZE(ICanvasView)
DECL_IMPLEMENTED_AS_QOBJECT(ICanvasView)

#endif // ICANVASVIEW_HPP