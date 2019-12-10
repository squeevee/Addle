#ifndef IEDITORVIEWPORT_HPP
#define IEDITORVIEWPORT_HPP

#include "iviewport.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/initialize_traits.hpp"


class IEditorViewPort : public virtual IViewPort
{
public:
    virtual ~IEditorViewPort() = default;
};

DECL_MAKEABLE(IEditorViewPort)
DECL_EXPECTS_INITIALIZE(IEditorViewPort)
DECL_IMPLEMENTED_AS_QOBJECT(IEditorViewPort)

#endif // IEDITORVIEWPORT_HPP