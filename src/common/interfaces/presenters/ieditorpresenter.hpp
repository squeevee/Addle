#ifndef IEDITORPRESENTER_HPP
#define IEDITORPRESENTER_HPP

#include <QObject>

#include "idocumentpresenter.hpp"

#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "ierrorpresenter.hpp"
#include "ihaveundostackpresenter.hpp"

class IEditorView;
//class IToolOptionsPresenter;

class IEditorPresenter : public virtual IDocumentPresenter, public IHaveUndoStackPresenter
{
public:
    virtual ~IEditorPresenter() {}

    virtual IEditorView* getEditorView() = 0;

    virtual ILayerPresenter* getSelectedLayer() = 0;
    virtual void setSelectedLayer(int index) = 0;

    struct DefaultTools
    {
        static const ToolId SELECT;
        static const ToolId BRUSH;
        static const ToolId ERASER;
        static const ToolId FILL;
        static const ToolId TEXT;
        static const ToolId SHAPES;
        static const ToolId STICKERS;
        static const ToolId EYEDROP;
        static const ToolId NAVIGATE;
        static const ToolId MEASURE;
    };

public slots:
    virtual void newDocument() = 0;
};

DECL_MAKEABLE(IEditorPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IEditorPresenter)

#endif // IEDITORPRESENTER_HPP