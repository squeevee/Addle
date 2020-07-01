#include "maineditorview.hpp"

#include "zoomrotatewidget.hpp"
#include "viewportscrollwidget.hpp"

#include "servicelocator.hpp"

#include "utilities/addle_icon.hpp"
#include "utilities/addle_text.hpp"
#include <QCoreApplication>

#include <QSignalBlocker>

#include <QAbstractButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

#include "viewport.hpp"

#include "interfaces/presenters/tools/itoolpresenter.hpp"
#include "interfaces/services/iapplicationsservice.hpp"
#include "utilities/qtextensions/qobject.hpp"

#include "utilities/presenter/propertybinding.hpp"

#include "widgetsgui/utilities/widgetproperties.hpp"

#include "helpers/toolsetuphelper.hpp"

#include "tooloptionsbars/brushtooloptionsbar.hpp"
#include "tooloptionsbars/navigatetooloptionsbar.hpp"

#include "colorselector.hpp"

void MainEditorView::initialize(IMainEditorPresenter* presenter)
{
    _initHelper.initializeBegin();

    _presenter = presenter;

    connect_interface(_presenter, SIGNAL(raiseError(QSharedPointer<IErrorPresenter>)),
        this, SLOT(onPresenterErrorRaised(QSharedPointer<IErrorPresenter>)));
    connect_interface(_presenter, SIGNAL(undoStateChanged()), this, SLOT(onUndoStateChanged()));
    
    _initHelper.initializeEnd();
}

void MainEditorView::setupUi()
{
    _menuBar = new QMenuBar(this);
    QMainWindow::setMenuBar(_menuBar);

    _toolBar_documentActions = new QToolBar(this);
    _toolBar_documentActions->setWindowTitle(ADDLE_TEXT("ui.document-actions.title"));
    QMainWindow::addToolBar(Qt::ToolBarArea::TopToolBarArea, _toolBar_documentActions);

    _toolBar_editorToolSelection = new QToolBar(this);
    _toolBar_editorToolSelection->setWindowTitle(ADDLE_TEXT("ui.tool-selection.title"));
    QMainWindow::addToolBar(Qt::ToolBarArea::LeftToolBarArea, _toolBar_editorToolSelection);

    new PropertyBinding(
        _toolBar_editorToolSelection,
        WidgetProperties::enabled,
        qobject_interface_cast(_presenter),
        IMainEditorPresenter::Meta::Properties::empty,
        PropertyBinding::ReadOnly,
        BindingConverter::negate()
    );

    IViewPortPresenter* viewPortPresenter = _presenter->getViewPortPresenter();

    _viewPort = new ViewPort(viewPortPresenter);
    _viewPortScrollWidget = new ViewPortScrollWidget(*viewPortPresenter, this);
    _viewPortScrollWidget->setViewPort(_viewPort);
    QMainWindow::setCentralWidget(_viewPortScrollWidget);
    _viewPort->setFocus();
    
    connect_interface(_presenter, SIGNAL(isEmptyChanged(bool)), this, SLOT(onPresenterEmptyChanged(bool)));

    _statusBar = new QStatusBar(this);
    QMainWindow::setStatusBar(_statusBar);

    _zoomRotateWidget = new ZoomRotateWidget(*viewPortPresenter, this);
    _statusBar->addPermanentWidget(_zoomRotateWidget);
    _zoomRotateWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _action_open = new QAction(this);
    _action_open->setIcon(ADDLE_ICON("open"));
    _action_open->setToolTip(ADDLE_TEXT("ui.open.toolTip"));
    connect(_action_open, &QAction::triggered, this, &MainEditorView::onAction_open);

    _optionGroup_toolSelection = new OptionGroup(this);
    new PropertyBinding(
        _optionGroup_toolSelection,
        WidgetProperties::value,
        qobject_interface_cast(_presenter),
        IMainEditorPresenter::Meta::Properties::currentTool
    );

    _action_new = new QAction(this);
    _action_new->setIcon(ADDLE_ICON("new"));
    _action_new->setToolTip(ADDLE_TEXT("ui.new.toolTip"));
    connect_interface(_action_new, SIGNAL(triggered()), _presenter, SLOT(newDocument()));

    _action_undo = new QAction(this);
    _action_undo->setIcon(ADDLE_ICON("undo"));
    _action_undo->setToolTip(ADDLE_TEXT("ui.undo.toolTip"));
    _action_undo->setEnabled(_presenter->canUndo());
    connect_interface(_action_undo, SIGNAL(triggered()), _presenter, SLOT(undo()));

    _action_redo = new QAction(this);
    _action_redo->setIcon(ADDLE_ICON("redo"));
    _action_redo->setToolTip(ADDLE_TEXT("ui.redo.toolTip"));
    _action_redo->setEnabled(_presenter->canRedo());
    connect_interface(_action_redo, SIGNAL(triggered()), _presenter, SLOT(redo()));
    
    _toolBar_documentActions->addAction(_action_new);
    _toolBar_documentActions->addAction(_action_open);
    _toolBar_documentActions->addSeparator();
    _toolBar_documentActions->addAction(_action_undo);
    _toolBar_documentActions->addAction(_action_redo);

    ToolSetupHelper setupHelper(
        this,
        *_presenter,
        _optionGroup_toolSelection
    );


    setupHelper.addTool(
        IMainEditorPresenter::DefaultTools::BRUSH,
        &_action_selectBrushTool,
        &_optionsToolBar_brush
    );

    setupHelper.addTool(
        IMainEditorPresenter::DefaultTools::NAVIGATE,
        &_action_selectNavigateTool,
        &_optionsToolBar_navigate
    );

    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("select-tool"), "", this));

    _toolBar_editorToolSelection->addSeparator();


    _toolBar_editorToolSelection->addAction(_action_selectBrushTool);
    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("eraser-tool"), "", this));
    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("fill-tool"), "", this));

    _toolBar_editorToolSelection->addSeparator();

    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("text-tool"), "", this));
    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("shapes-tool"), "", this));
    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("stickers-tool"), "", this));

    _toolBar_editorToolSelection->addSeparator();

    _toolBar_editorToolSelection->addAction(_action_selectNavigateTool);
    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("eyedrop-tool"), "", this));
    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("measure-tool"), "", this));

    _colorSelector = new ColorSelector(_presenter->colorSelection(), this);

    addDockWidget(Qt::BottomDockWidgetArea, _colorSelector);
}

void MainEditorView::onUndoStateChanged()
{
    //TODO: replace with PropertyBindings
    _initHelper.check();

    _action_undo->setEnabled(_presenter->canUndo());
    _action_redo->setEnabled(_presenter->canRedo());
}

void MainEditorView::start()
{
    _initHelper.check();
    setupUi();
    QWidget::show();
}

void MainEditorView::onAction_open()
{
    QUrl file = QFileDialog::getOpenFileUrl(
        this, 
        "Open image",
        QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))
    );

    if (!file.isEmpty())
        _presenter->loadDocument(file);
}

void MainEditorView::onPresenterErrorRaised(QSharedPointer<IErrorPresenter> error)
{
    _initHelper.check();

    QMessageBox message(this);
    message.setText(error->getMessage());
    if (error->getSeverity() == IErrorPresenter::warning)
    {
        message.setIcon(QMessageBox::Icon::Warning);
    }

    message.exec();
}

void MainEditorView::onToolBarNeedsShown()
{
    QToolBar* toolBar = qobject_cast<QToolBar*>(sender());

    toolBar->show();
    addToolBar(toolBar);
}

void MainEditorView::onToolBarNeedsHidden()
{
    QToolBar* toolBar = qobject_cast<QToolBar*>(sender());

    toolBar->hide();
    removeToolBar(toolBar);
}

void MainEditorView::onPresenterEmptyChanged(bool empty)
{
    if (!empty && _viewPort)
        _viewPort->setFocus();
}