/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "maineditorview.hpp"

#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QCloseEvent>

#include "zoomrotatewidget.hpp"
#include "viewportscrollwidget.hpp"

#include "utils.hpp"

#include "interfaces/presenters/tools/itoolpresenter.hpp"
#include "interfaces/services/iapplicationsservice.hpp"
#include "interfaces/views/imessageview.hpp"

#include "layers/layersmanager.hpp"

#include "colorselector.hpp"
#include "viewport.hpp"

#include "utilities/qobject.hpp"
#include "utilities/presenter/propertybinding.hpp"
#include "utilities/widgetproperties.hpp"
#include "utilities/genericeventfilter.hpp"

#include "helpers/toolsetuphelper.hpp"

#include "tooloptionsbars/brushtooloptionsbar.hpp"
#include "tooloptionsbars/navigatetooloptionsbar.hpp"

#include "utilities/filedialoghelper.hpp"

using namespace Addle;

void MainEditorView::initialize(IMainEditorPresenter& presenter)
{
    const Initializer init(_initHelper);

    _presenter = &presenter;
    
    connect_interface(_presenter, SIGNAL(messagePosted(QSharedPointer<IMessagePresenter>)),
                              this, SLOT(onMessagePosted(QSharedPointer<IMessagePresenter>)));
                            
    connect_interface(_presenter, SIGNAL(undoStateChanged()),
                              this, SLOT(onUndoStateChanged()));

    connect_interface(_presenter, SIGNAL(documentPresenterChanged(QSharedPointer<IDocumentPresenter>)),
                              this, SLOT(onDocumentChanged(QSharedPointer<IDocumentPresenter>)));   
}

void MainEditorView::setupUi()
{
    _menuBar = new QMenuBar(this);
    QMainWindow::setMenuBar(_menuBar);

    _toolBar_documentActions = new QToolBar(this);
    _toolBar_documentActions->setWindowTitle(qtTrId("ui.document-actions.title"));
    QMainWindow::addToolBar(Qt::ToolBarArea::TopToolBarArea, _toolBar_documentActions);

    _toolBar_editorToolSelection = new QToolBar(this);
    _toolBar_editorToolSelection->setWindowTitle(qtTrId("ui.tool-selection.title"));
    QMainWindow::addToolBar(Qt::ToolBarArea::LeftToolBarArea, _toolBar_editorToolSelection);

    new PropertyBinding(
        _toolBar_editorToolSelection,
        WidgetProperties::enabled,
        qobject_interface_cast(_presenter),
        IMainEditorPresenter::Meta::Properties::empty,
        PropertyBinding::ReadOnly,
        BindingConverter::negate()
    );

    _viewPort = new ViewPort(_presenter->viewPortPresenter());
    _viewPortScrollWidget = new ViewPortScrollWidget(_presenter->viewPortPresenter(), this);
    _viewPortScrollWidget->setViewPort(_viewPort);
    QMainWindow::setCentralWidget(_viewPortScrollWidget);
    _viewPort->setFocus();
    
    connect_interface(_presenter, SIGNAL(isEmptyChanged(bool)), this, SLOT(onPresenterEmptyChanged(bool)));

    _statusBar = new QStatusBar(this);
    QMainWindow::setStatusBar(_statusBar);

    _zoomRotateWidget = new ZoomRotateWidget(_presenter->viewPortPresenter(), this);
    _statusBar->addPermanentWidget(_zoomRotateWidget);
    _zoomRotateWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _action_open = new QAction(this);
    _action_open->setIcon(ADDLE_ICON("open"));
    _action_open->setToolTip(qtTrId("ui.open.description"));
    connect(_action_open, &QAction::triggered, this, &MainEditorView::onAction_open);

    _action_save = new QAction(this);
    _action_save->setIcon(ADDLE_ICON("save"));
    _action_save->setToolTip(qtTrId("ui.save.description"));
    _action_save->setDisabled(true);
    connect(_action_save, &QAction::triggered, this, &MainEditorView::onAction_save);

    _optionGroup_toolSelection = new OptionGroup(this);
    new PropertyBinding(
        _optionGroup_toolSelection,
        WidgetProperties::value,
        qobject_interface_cast(_presenter),
        IMainEditorPresenter::Meta::Properties::currentTool
    );

    _action_new = new QAction(this);
    _action_new->setIcon(ADDLE_ICON("new"));
    _action_new->setToolTip(qtTrId("ui.new.description"));
    connect_interface(_action_new, SIGNAL(triggered()), _presenter, SLOT(newDocument()));

    _action_undo = new QAction(this);
    _action_undo->setIcon(ADDLE_ICON("undo"));
    _action_undo->setToolTip(qtTrId("ui.undo.description"));
    _action_undo->setEnabled(_presenter->canUndo());
    connect_interface(_action_undo, SIGNAL(triggered()), _presenter, SLOT(undo()));

    _action_redo = new QAction(this);
    _action_redo->setIcon(ADDLE_ICON("redo"));
    _action_redo->setToolTip(qtTrId("ui.redo.description"));
    _action_redo->setEnabled(_presenter->canRedo());
    connect_interface(_action_redo, SIGNAL(triggered()), _presenter, SLOT(redo()));
    
    _toolBar_documentActions->addAction(_action_new);
    _toolBar_documentActions->addAction(_action_open);
    _toolBar_documentActions->addAction(_action_save);
    _toolBar_documentActions->addSeparator();
    _toolBar_documentActions->addAction(_action_undo);
    _toolBar_documentActions->addAction(_action_redo);

    ToolSetupHelper setupHelper(
        this,
        *_presenter,
        _optionGroup_toolSelection
    );

    setupHelper.addTool(
        CoreTools::Brush,
        &_action_selectBrushTool,
        &_optionsToolBar_brush
    );

    setupHelper.addTool(
        CoreTools::Eraser,
        &_action_selectEraserTool,
        &_optionsToolBar_eraser
    );

    setupHelper.addTool(
        CoreTools::Navigate,
        &_action_selectNavigateTool,
        &_optionsToolBar_navigate
    );

    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("select-tool"), "", this));

    _toolBar_editorToolSelection->addSeparator();

    _toolBar_editorToolSelection->addAction(_action_selectBrushTool);
    _toolBar_editorToolSelection->addAction(_action_selectEraserTool);
    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("fill-tool"), "", this));

    _toolBar_editorToolSelection->addSeparator();

    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("text-tool"), "", this));
    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("shapes-tool"), "", this));
    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("stickers-tool"), "", this));

    _toolBar_editorToolSelection->addSeparator();

    _toolBar_editorToolSelection->addAction(_action_selectNavigateTool);
    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("eyedrop-tool"), "", this));
    _toolBar_editorToolSelection->addAction(new QAction(ADDLE_ICON("measure-tool"), "", this));

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    _layersManager = new LayersManager(this);
    addDockWidget(Qt::RightDockWidgetArea, _layersManager);

    _colorSelector = new ColorSelector(_presenter->colorSelection(), this);

    addDockWidget(Qt::BottomDockWidgetArea, _colorSelector);

    _fileDialogHelper = new FileDialogHelper(this);
    
    _uiIsSetup = true;
}

void MainEditorView::onUndoStateChanged()
{
    try
    {
        ASSERT_INIT();
        //TODO: replace with PropertyBindings

        _action_undo->setEnabled(_presenter->canUndo());
        _action_redo->setEnabled(_presenter->canRedo());
    }
    ADDLE_SLOT_CATCH
}

void MainEditorView::onAction_open()
{
    try
    {
        ASSERT_INIT();
        QSharedPointer<FileRequest> request(new FileRequest(FileRequest::Load));
        _fileDialogHelper->setRequest(request);
        
        _presenter->loadDocument(request);
        
//         QUrl file = QFileDialog::getOpenFileUrl(
//             this, 
//             qtTrId("ui.open-document.title"),
//             QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))
//         );
// 
//         if (!file.isEmpty())
//             _presenter->loadDocument(file);
    }
    ADDLE_SLOT_CATCH
}

void MainEditorView::onAction_save()
{
    try
    {
        ASSERT_INIT();
//         QSharedPointer<FileRequest> request(new FileRequest(FileRequest::Save));
//         _fileDialogHelper->setRequest(request);
// 
//         _presenter->saveDocument(request);
    }
    ADDLE_SLOT_CATCH
}

void MainEditorView::onMessagePosted(QSharedPointer<IMessagePresenter> message)
{
    try
    {
        ASSERT_INIT();
        auto view = ServiceLocator::make<IMessageView>(message);
        
        view->show();
    }
    ADDLE_SLOT_CATCH
}

void MainEditorView::onToolBarNeedsShown()
{
    try
    {
        ASSERT_INIT();
        QToolBar* toolBar = qobject_cast<QToolBar*>(sender());

        toolBar->show();
        addToolBar(toolBar);
    }
    ADDLE_SLOT_CATCH
}

void MainEditorView::onToolBarNeedsHidden()
{
    try
    {
        ASSERT_INIT();
        QToolBar* toolBar = qobject_cast<QToolBar*>(sender());

        toolBar->hide();
        removeToolBar(toolBar);
    }
    ADDLE_SLOT_CATCH
}

void MainEditorView::onPresenterEmptyChanged(bool empty)
{
    try
    {
        ASSERT_INIT();
        if (!empty && _viewPort)
            _viewPort->setFocus();
    }
    ADDLE_SLOT_CATCH
}

void MainEditorView::onDocumentChanged(QSharedPointer<IDocumentPresenter> document)
{
    try
    {
        ASSERT_INIT();
        _layersManager->setPresenter(document);

        _action_save->setEnabled(static_cast<bool>(document));
    }
    ADDLE_SLOT_CATCH
}

void MainEditorView::closeEvent(QCloseEvent* event)
{
    try
    {
        ASSERT_INIT();
        // the presenter will probably have something to say about this

        event->accept();
        emit tlv_closed();
    }
    ADDLE_EVENT_CATCH(event)
}


void MainEditorView::tlv_show()
{
    try
    {
        ASSERT_INIT();
        if (!_uiIsSetup)
            setupUi();
        QWidget::show();
    }
    ADDLE_SLOT_CATCH
}

void MainEditorView::tlv_close()
{
    try
    {
        ASSERT_INIT();
        QWidget::close();
    }
    ADDLE_SLOT_CATCH
}
