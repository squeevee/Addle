#ifndef EDITORVIEW_HPP
#define EDITORVIEW_HPP

#include <QSharedPointer>

#include "interfaces/views/ieditorview.hpp"
#include "basedocumentview.hpp"

#include "widgetsgui/widgets/tooloptionsbars/navigatetooloptionsbar.hpp"
#include "widgetsgui/widgets/tooloptionsbars/brushtooloptionsbar.hpp"

#include "utilities/initializehelper.hpp"

#include "interfaces/presenters/tools/iselecttoolpresenter.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "interfaces/presenters/tools/ierasertoolpresenter.hpp"
#include "interfaces/presenters/tools/ifilltoolpresenter.hpp"
#include "interfaces/presenters/tools/itexttoolpresenter.hpp"
#include "interfaces/presenters/tools/ishapestoolpresenter.hpp"
#include "interfaces/presenters/tools/istickerstoolpresenter.hpp"
#include "interfaces/presenters/tools/ieyedroptoolpresenter.hpp"
#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/tools/imeasuretoolpresenter.hpp"

class EditorView : public BaseDocumentView, public virtual IEditorView
{
    Q_OBJECT
public:
    EditorView() : _initHelper(this)
    {
    }
    virtual ~EditorView() = default;

    void initialize(IEditorPresenter* presenter);

    IEditorPresenter* getEditorPresenter() { _initHelper.assertInitialized(); return _presenter; }

protected:
    void setupUi();

private:
    IEditorPresenter* _presenter;

    SelectToolAction* _action_selectSelectTool;
    ISelectToolPresenter* _toolPresenter_select;
    QToolBar* _optionsToolBar_select;

    SelectToolAction* _action_selectBrushTool;
    IBrushToolPresenter* _toolPresenter_brush;
    BrushToolOptionsBar* _optionsToolBar_brush;

    SelectToolAction* _action_selectEraserTool;
    IEraserToolPresenter* _toolPresenter_eraser;
    QToolBar* _optionsToolBar_eraser;

    SelectToolAction* _action_selectFillTool;
    IFillToolPresenter* _toolPresenter_fill;
    QToolBar* _optionsToolBar_fill;

    SelectToolAction* _action_selectTextTool;
    ITextToolPresenter* _toolPresenter_text;
    QToolBar* _optionsToolBar_text;

    SelectToolAction* _action_selectShapesTool;
    IShapesToolPresenter* _toolPresenter_shapes;
    QToolBar* _optionsToolBar_shapes;

    SelectToolAction* _action_selectStickersTool;
    IStickersToolPresenter* _toolPresenter_stickers;
    QToolBar* _optionsToolBar_stickers;

    SelectToolAction* _action_selectEyedropTool;
    IEyedropToolPresenter* _toolPresenter_eyedrop;
    QToolBar* _optionsToolBar_eyedrop;

    SelectToolAction* _action_selectNavigateTool;
    INavigateToolPresenter* _toolPresenter_navigate;
    NavigateToolOptionsBar* _optionsToolBar_navigate;

    SelectToolAction* _action_selectMeasureTool;
    IMeasureToolPresenter* _toolPresenter_measure;
    QToolBar* _optionsToolBar_measure;

    InitializeHelper<EditorView> _initHelper;
};
#endif // EDITORVIEW_HPP