#ifndef IEDITORVIEW_HPP
#define IEDITORVIEW_HPP

#include "imainview.hpp"

#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class IMainEditorPresenter;
class IViewPortPresenter;
class ICanvasPresenter;
class IMainEditorView: public virtual IMainView
{
public:
    virtual ~IMainEditorView() = default;

    virtual void initialize(IMainEditorPresenter* presenter) = 0;

    virtual IMainEditorPresenter* getPresenter() = 0;
};

DECL_MAKEABLE(IMainEditorView)
DECL_EXPECTS_INITIALIZE(IMainEditorView)

#endif // IEDITORVIEW_HPP