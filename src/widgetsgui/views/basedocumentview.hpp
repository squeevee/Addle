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

#include "widgetsgui/widgets/selecttoolaction.hpp"

#include "interfaces/presenters/idocumentpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/views/idocumentview.hpp"

#include "utilities/initializehelper.hpp"
#include "utilities/qt_extensions/qobject.hpp"

class ViewPortWidget;
class ZoomRotateWidget;
class BaseDocumentView : public QMainWindow, public virtual IDocumentView
{
    Q_OBJECT

public:
    BaseDocumentView() : _initHelper(this)
    {
    }
    virtual ~BaseDocumentView() = default;

    IDocumentPresenter* getDocumentPresenter() { _initHelper.assertInitialized(); return _presenter; }

public slots:
    void start();

protected:

    void initialize(IDocumentPresenter* presenter);
    virtual void setupUi();

    QMenuBar* _menuBar;
    QToolBar* _toolBar_documentActions;
    QToolBar* _toolBar_toolSelection;
    QToolBar* _toolBar_toolOptions;
    QStatusBar* _statusBar;

    ViewPortWidget* _viewPortWidget;
    ZoomRotateWidget* _zoomRotateWidget;

    QAction* _action_open;
    QAction* _action_close;

    QActionGroup* _actionGroup_toolSelection;

    template<class ToolBarType, class PresenterType>
    void addTool(ToolId tool, SelectToolAction** actionptr, ToolBarType** optionsptr, PresenterType** presenterptr)
    {
        PresenterType* toolPresenter = dynamic_cast<PresenterType*>(_presenter->getToolPresenter(tool));
        *presenterptr = toolPresenter;

        SelectToolAction* action = new SelectToolAction(*toolPresenter, this);
        *actionptr = action;

        action->setCheckable(true);
        _toolBar_toolSelection->addAction(action);
        _actionGroup_toolSelection->addAction(action);

        ToolBarType* optionsToolBar = new ToolBarType(*toolPresenter, this);
        *optionsptr = optionsToolBar;
    }

protected slots:
    virtual void onAction_open();

    virtual void onDocumentChanged();
    virtual void onPresenterErrorRaised(QSharedPointer<IErrorPresenter> error);

private:
    IDocumentPresenter* _presenter;
    IViewPortPresenter* _viewPortPresenter;

    InitializeHelper<BaseDocumentView> _initHelper;
};

#endif // BASEDOCUMENTVIEW_HPP
