/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef BASEDOCUMENTVIEW_HPP
#define BASEDOCUMENTVIEW_HPP

#include "compat.hpp"
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QGridLayout>
#include <QScrollBar>
#include <QAction>
#include <QActionGroup>

#include <memory>

#include "utilities/optiongroup.hpp"

#include "interfaces/views/imaineditorview.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/errors/ierrorpresenter.hpp"

#include "utilities/initializehelper.hpp"
#include "utilities/qobject.hpp"

namespace Addle {

class BrushToolOptionsBar;
class NavigateToolOptionsBar;

class ViewPort;
class ViewPortScrollWidget;
class ZoomRotateWidget;
class ColorSelector;
class LayersManager;
class ADDLE_WIDGETSGUI_EXPORT MainEditorWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainEditorWindow(IMainEditorPresenter& presenter);
    virtual ~MainEditorWindow() = default;

private slots:
    void onAction_open();

    void onPresenterError(QSharedPointer<IErrorPresenter> error);

    void onUndoStateChanged();

    void onToolBarNeedsShown();
    void onToolBarNeedsHidden();

    void onPresenterEmptyChanged(bool);

    void onDocumentChanged(QSharedPointer<IDocumentPresenter> document);

protected:
    void closeEvent(QCloseEvent* event);

private:
    void setupUi();
    Q_SIGNAL void closeEventAccepted();

    IMainEditorPresenter& _presenter;

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

    OptionAction* _action_selectEraserTool;
    BrushToolOptionsBar* _optionsToolBar_eraser;

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

    LayersManager* _layersManager;
    ColorSelector* _colorSelector;

    friend class MainEditorView;
    friend class ToolSetupHelper;
};

// Adapter/wrapper that implements the IMainEditorView interface on behalf of
// MainEditorWindow
class ADDLE_WIDGETSGUI_EXPORT MainEditorView : public QObject, public IMainEditorView
{
    Q_OBJECT
    Q_INTERFACES(Addle::IMainEditorView Addle::ITopLevelView)
    IAMQOBJECT_IMPL    
public:
    virtual ~MainEditorView() = default;

    void initialize(IMainEditorPresenter& presenter);
    IMainEditorPresenter& presenter() const { ASSERT_INIT(); return *_presenter; }

public slots:
    void show();
    void close();

signals:
    void closed();

private:
    IMainEditorPresenter* _presenter = nullptr;
    std::unique_ptr<MainEditorWindow> _window;
    bool _uiIsSetup = false;

    InitializeHelper _initHelper;
};

} // namespace Addle

#endif // BASEDOCUMENTVIEW_HPP
