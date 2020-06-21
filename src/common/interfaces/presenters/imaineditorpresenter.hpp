#ifndef IMAINEDITORPRESENTER_HPP
#define IMAINEDITORPRESENTER_HPP

#include "compat.hpp"
#include <QObject>

#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/metaobjectinfo.hpp"

#include "ierrorpresenter.hpp"
#include "ihavedocumentpresenter.hpp"
#include "ihaveundostackpresenter.hpp"
#include "ierrorpresenter.hpp"
#include "ihavetoolspresenter.hpp"
#include "iraiseerrorpresenter.hpp"

class IPalettePresenter;
class ILayerPresenter;
class IMainEditorView;
class ICanvasPresenter;
class IViewPortPresenter;

namespace IMainEditorPresenterAux
{
    enum Mode
    {
        Editor,
        Viewer
    };

    struct ADDLE_COMMON_EXPORT DefaultTools
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
}

class IMainEditorPresenter
    : public virtual IHaveDocumentPresenter,
    public virtual IHaveToolsPresenter,
    public virtual IHaveUndoStackPresenter,
    public virtual IRaiseErrorPresenter
{
public:
    INTERFACE_META(IMainEditorPresenter);
    
    virtual ~IMainEditorPresenter() {}

    typedef IMainEditorPresenterAux::Mode Mode;

    virtual void initialize(Mode mode) = 0;

    virtual IMainEditorView* getView() = 0;

    virtual ICanvasPresenter* getCanvasPresenter() = 0;
    virtual IViewPortPresenter* getViewPortPresenter() = 0;

    virtual void setMode(Mode mode) = 0;
    virtual Mode getMode() = 0;

    virtual QWeakPointer<ILayerPresenter> getSelectedLayer() = 0;
    virtual void selectLayer(QWeakPointer<ILayerPresenter> layer) = 0;
    virtual void selectLayerAt(int index) = 0;

    virtual QSharedPointer<IPalettePresenter> palette() const = 0;
    
    typedef IMainEditorPresenterAux::DefaultTools DefaultTools;

signals:
    virtual void documentPresenterChanged(IDocumentPresenter* documentPresenter) = 0;
    virtual void selectedLayerChanged(QWeakPointer<ILayerPresenter> layer) = 0;
    virtual void isEmptyChanged(bool) = 0;
};

DECL_MAKEABLE(IMainEditorPresenter)
DECL_EXPECTS_INITIALIZE(IMainEditorPresenter);
DECL_IMPLEMENTED_AS_QOBJECT(IMainEditorPresenter)

DECL_INTERFACE_META_PROPERTIES(
    IMainEditorPresenter,
    DECL_INTERFACE_PROPERTY(currentTool)
    DECL_INTERFACE_PROPERTY(empty)
)

#endif // IMAINEDITORPRESENTER_HPP