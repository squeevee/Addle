#ifndef IEDITORVIEW_HPP
#define IEDITORVIEW_HPP

#include "idocumentview.hpp"

#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/servicelocator/imakeable.hpp"
#include "interfaces/presenters/ieditorpresenter.hpp"

class IEditorView: public virtual IDocumentView, public virtual IMakeable
{
public:
    virtual ~IEditorView() = default;

    virtual void initialize(IEditorPresenter* presenter) = 0;

    virtual IEditorPresenter* getEditorPresenter() = 0;
};

DECL_EXPECTS_INITIALIZE(IEditorView)
DECL_INIT_DEPENDENCY(IEditorView, IEditorPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IEditorView)

#endif // IEDITORVIEW_HPP