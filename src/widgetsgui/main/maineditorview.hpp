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

#include "helpers/editortoolshelper.hpp"

#include "utilities/initializehelper.hpp"
#include "utilities/qtextensions/qobject.hpp"

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
    virtual ~MainEditorView();

    void initialize(IMainEditorPresenter* presenter);

    IMainEditorPresenter* getPresenter() { _initHelper.check(); return _presenter; }

public slots:
    void start();

private slots:
    void onAction_open();

    void onPresenterErrorRaised(QSharedPointer<IErrorPresenter> error);

    void onUndoStateChanged();

private:
    void setupUi();
    void updateTools();

    IMainEditorPresenter* _presenter;

    QMenuBar* _menuBar;
    QToolBar* _toolBar_documentActions;
    QToolBar* _toolBar_toolSelection;
    QToolBar* _toolBar_toolOptions;
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

    EditorToolsHelper* _editorTools = nullptr;

    InitializeHelper<MainEditorView> _initHelper;

    friend class EditorToolsHelper;
};

#endif // BASEDOCUMENTVIEW_HPP
