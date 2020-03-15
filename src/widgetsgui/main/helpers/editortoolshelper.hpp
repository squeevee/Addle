#ifndef EDITORTOOLSHELPER_HPP
#define EDITORTOOLSHELPER_HPP

#include <QToolBar>
#include "widgetsgui/utilities/optiongroup.hpp"

class MainEditorView;

class ISelectToolPresenter;
class IBrushToolPresenter;
class IEraserToolPresenter;
class IFillToolPresenter;
class ITextToolPresenter;
class IShapesToolPresenter;
class IStickersToolPresenter;
class IEyedropToolPresenter;
class INavigateToolPresenter;
class IMeasureToolPresenter;

class BrushToolOptionsBar;
class NavigateToolOptionsBar;

class EditorToolsHelper
{
public:
    EditorToolsHelper(MainEditorView& view);
    ~EditorToolsHelper() = default; //TODO

    //SelectToolAction* action_selectSelectTool;
    ISelectToolPresenter* toolPresenter_select;
    QToolBar* optionsToolBar_select;

    OptionAction* action_selectBrushTool;
    IBrushToolPresenter* toolPresenter_brush;
    BrushToolOptionsBar* optionsToolBar_brush;

    //SelectToolAction* action_selectEraserTool;
    IEraserToolPresenter* toolPresenter_eraser;
    QToolBar* optionsToolBar_eraser;

    //SelectToolAction* action_selectFillTool;
    IFillToolPresenter* toolPresenter_fill;
    QToolBar* optionsToolBar_fill;

    //SelectToolAction* action_selectTextTool;
    ITextToolPresenter* toolPresenter_text;
    QToolBar* optionsToolBar_text;

    //SelectToolAction* action_selectShapesTool;
    IShapesToolPresenter* toolPresenter_shapes;
    QToolBar* optionsToolBar_shapes;

    //SelectToolAction* action_selectStickersTool;
    IStickersToolPresenter* toolPresenter_stickers;
    QToolBar* optionsToolBar_stickers;

    //SelectToolAction* action_selectEyedropTool;
    IEyedropToolPresenter* toolPresenter_eyedrop;
    QToolBar* optionsToolBar_eyedrop;

    OptionAction* action_selectNavigateTool;
    INavigateToolPresenter* toolPresenter_navigate;
    NavigateToolOptionsBar* optionsToolBar_navigate;

    //SelectToolAction* action_selectMeasureTool;
    IMeasureToolPresenter* toolPresenter_measure;
    QToolBar* optionsToolBar_measure;

private:
    MainEditorView& _view;
};

#endif // EDITORTOOLSHELPER_HPP