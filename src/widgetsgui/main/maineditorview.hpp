#ifndef BASEDOCUMENTVIEW_HPP
#define BASEDOCUMENTVIEW_HPP

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QGridLayout>
#include <QScrollBar>
#include <QAction>
#include <QActionGroup>

#include "widgetsgui/utilities/optiongroup.hpp"

#include "interfaces/views/imaineditorview.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/ierrorpresenter.hpp"

#include "utilities/initializehelper.hpp"
#include "utilities/qtextensions/qobject.hpp"

class BrushToolOptionsBar;
class NavigateToolOptionsBar;

class ViewPort;
class ViewPortScrollWidget;
class ZoomRotateWidget;

class MainEditorView : public QMainWindow, public virtual IMainEditorView
{
    Q_OBJECT

public:
    MainEditorView() : _initHelper(this)
    {
    }
    virtual ~MainEditorView() = default;

    void initialize(IMainEditorPresenter* presenter);

    IMainEditorPresenter* getPresenter() { _initHelper.check(); return _presenter; }

public slots:
    void start();

private slots:
    void onAction_open();

    void onPresenterErrorRaised(QSharedPointer<IErrorPresenter> error);

    void onUndoStateChanged();

    void onToolBarNeedsShown();
    void onToolBarNeedsHidden();

    void onPresenterEmptyChanged(bool);

private:
    void setupUi();

    IMainEditorPresenter* _presenter;

    QMenuBar* _menuBar;
    QToolBar* _toolBar_documentActions;
    QToolBar* _toolBar_editorToolSelection;
    //QToolBar* _toolBar_viewerToolSelection;
    QStatusBar* _statusBar;

    ViewPort* _viewPort;
    ViewPortScrollWidget* _viewPortScrollWidget;
    ZoomRotateWidget* _zoomRotateWidget;

    QAction* _action_new;
    QAction* _action_open;
    QAction* _action_undo;
    QAction* _action_redo;
    
    QAction* _action_close;

    OptionGroup* _optionGroup_toolSelection;

    //SelectToolAction* action_selectSelectTool;
    QToolBar* _optionsToolBar_select;

    OptionAction* _action_selectBrushTool;
    BrushToolOptionsBar* _optionsToolBar_brush;

    //SelectToolAction* action_selectEraserTool;
    QToolBar* _optionsToolBar_eraser;

    //SelectToolAction* action_selectFillTool;
    QToolBar* _optionsToolBar_fill;

    //SelectToolAction* action_selectTextTool;
    QToolBar* _optionsToolBar_text;

    //SelectToolAction* action_selectShapesTool;
    QToolBar* _optionsToolBar_shapes;

    //SelectToolAction* action_selectStickersTool;
    QToolBar* _optionsToolBar_stickers;

    //SelectToolAction* action_selectEyedropTool;
    QToolBar* _optionsToolBar_eyedrop;

    OptionAction* _action_selectNavigateTool;
    NavigateToolOptionsBar* _optionsToolBar_navigate;

    //SelectToolAction* action_selectMeasureTool;
    QToolBar* _optionsToolBar_measure;

    QToolBar* _optionsToolBar_currentTool = nullptr;

    InitializeHelper<MainEditorView> _initHelper;
};

#endif // BASEDOCUMENTVIEW_HPP
