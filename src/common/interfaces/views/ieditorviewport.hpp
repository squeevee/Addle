#ifndef IEDITORVIEWPORT_HPP
#define IEDITORVIEWPORT_HPP

#include "iviewport.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"


class IEditorViewPort : public virtual IViewPort, public IMakeable
{
public:
    virtual ~IEditorViewPort() = default;
};

DECL_EXPECTS_INITIALIZE(IEditorViewPort)
DECL_IMPLEMENTED_AS_QOBJECT(IEditorViewPort)

#endif // IEDITORVIEWPORT_HPP