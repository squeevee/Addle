#ifndef IEDITORVIEW_HPP
#define IEDITORVIEW_HPP

#include "imainview.hpp"

#include "interfaces/traits.hpp"


namespace Addle {

class IMainEditorPresenter;
class IViewPortPresenter;
class ICanvasPresenter;
class IMainEditorView: public virtual IMainView
{
public:
    virtual ~IMainEditorView() = default;

    virtual void initialize(IMainEditorPresenter* presenter) = 0;

    virtual IMainEditorPresenter* presenter() = 0;
};

DECL_MAKEABLE(IMainEditorView)
DECL_EXPECTS_INITIALIZE(IMainEditorView)

} // namespace Addle
#endif // IEDITORVIEW_HPP