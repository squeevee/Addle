#ifndef IEDITORPRESENTER_HPP
#define IEDITORPRESENTER_HPP

#include <QObject>

#include "idocumentpresenter.hpp"

#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"
#include "ierrorpresenter.hpp"

class IEditorView;
class ICanvasPresenter;
//class IToolOptionsPresenter;

class IEditorPresenter : public virtual IDocumentPresenter, public virtual IMakeable
{
public:
    virtual ~IEditorPresenter() {}

    virtual IEditorView* getEditorView() = 0;

    //virtual IToolOptionsPresenter::ToolRole getSelectedTool() = 0;
    //virtual IToolOptionsPresenter* getSelectedToolOptionsPresenter() = 0;
    //virtual void selectTool(IToolOptionsPresenter::ToolRole tool) = 0;

    // virtual QColor getColor1() = 0;
    // virtual void setColor1(QColor color) = 0;

    // virtual QColor getColor2() = 0;
    // virtual void setColor2(QColor color) = 0;

//signals:
    //virtual void selectedToolChanged(IToolOptionsPresenter::ToolRole tool) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IEditorPresenter)

#endif // IEDITORPRESENTER_HPP