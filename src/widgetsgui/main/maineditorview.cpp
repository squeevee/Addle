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

#include <QtDebug>

#include "zoomrotatewidget.hpp"
#include "viewportscrollwidget.hpp"

#include "utils.hpp"

#include "interfaces/presenters/imessagecontext.hpp"
#include "interfaces/presenters/tools/itoolpresenter.hpp"
#include "interfaces/services/iapplicationservice.hpp"
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


MainEditorView::MainEditorView(IMainEditorPresenter& presenter)
     : _presenter(presenter),
     _tlvHelper(this, std::bind(&MainEditorView::setupUi, this))
{
    _tlvHelper.onOpened.bind(&MainEditorView::tlv_opened, this);
    _tlvHelper.onClosed.bind(&MainEditorView::tlv_closed, this);
//     
//     _messageViewHelper.onUrgentViewMade.bind(&MainEditorView::onUrgentMessageMade, this);
//     
//     connect_interface(&_presenter.messageContext(), SIGNAL(messagePosted(QSharedPointer<Addle::IMessagePresenter>)),
//                               this, SLOT(onMessagePosted(QSharedPointer<Addle::IMessagePresenter>)));
//                             
//     connect_interface(&_presenter, SIGNAL(undoStateChanged()),
//                               this, SLOT(onUndoStateChanged()));
// 
//     connect_interface(&_presenter, SIGNAL(documentPresenterChanged(QSharedPointer<Addle::IDocumentPresenter>)),
//                               this, SLOT(onDocumentChanged(QSharedPointer<Addle::IDocumentPresenter>)));   
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
        &_presenter,
        IMainEditorPresenter::Meta::Properties::empty,
        PropertyBinding::ReadOnly,
        BindingConverter::negate()
    );

    _viewPort = new ViewPort(_presenter.viewPortPresenter());
    _viewPortScrollWidget = new ViewPortScrollWidget(_presenter.viewPortPresenter(), this);
    _viewPortScrollWidget->setViewPort(_viewPort);
    QMainWindow::setCentralWidget(_viewPortScrollWidget);
    _viewPort->setFocus();
     
    connect_interface(&_presenter, SIGNAL(isEmptyChanged(bool)), this, SLOT(onPresenterEmptyChanged(bool)));
 
    _statusBar = new QStatusBar(this);
    QMainWindow::setStatusBar(_statusBar);

    _zoomRotateWidget = new ZoomRotateWidget(_presenter.viewPortPresenter(), this);
    _statusBar->addPermanentWidget(_zoomRotateWidget);
    _zoomRotateWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _action_open = new QAction(this);
    _action_open->setIcon(QIcon::fromTheme("open"));
    _action_open->setToolTip(qtTrId("ui.open.description"));
    connect(_action_open, &QAction::triggered, this, &MainEditorView::onAction_open);

    _action_save = new QAction(this);
    _action_save->setIcon(QIcon::fromTheme("save"));
    _action_save->setToolTip(qtTrId("ui.save.description"));
    _action_save->setDisabled(true);
    connect(_action_save, &QAction::triggered, this, &MainEditorView::onAction_save);

    _optionGroup_toolSelection = new OptionGroup(this);
    new PropertyBinding(
        _optionGroup_toolSelection,
        WidgetProperties::value,
        &_presenter,
        IMainEditorPresenter::Meta::Properties::currentTool
    );
 
    _action_new = new QAction(this);
    _action_new->setIcon(QIcon::fromTheme("new"));
    _action_new->setToolTip(qtTrId("ui.new.description"));
    connect_interface(_action_new, SIGNAL(triggered()), &_presenter, SLOT(newDocument()));

    _action_undo = new QAction(this);
    _action_undo->setIcon(QIcon::fromTheme("undo"));
    _action_undo->setToolTip(qtTrId("ui.undo.description"));
    _action_undo->setEnabled(_presenter.canUndo());
    connect_interface(_action_undo, SIGNAL(triggered()), &_presenter, SLOT(undo()));

    _action_redo = new QAction(this);
    _action_redo->setIcon(QIcon::fromTheme("redo"));
    _action_redo->setToolTip(qtTrId("ui.redo.description"));
    _action_redo->setEnabled(_presenter.canRedo());
    connect_interface(_action_redo, SIGNAL(triggered()), &_presenter, SLOT(redo()));
    
    _toolBar_documentActions->addAction(_action_new);
    _toolBar_documentActions->addAction(_action_open);
    _toolBar_documentActions->addAction(_action_save);
    _toolBar_documentActions->addSeparator();
    _toolBar_documentActions->addAction(_action_undo);
    _toolBar_documentActions->addAction(_action_redo);

    ToolSetupHelper setupHelper(
        this,
        _presenter,
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

    _toolBar_editorToolSelection->addAction(new QAction(QIcon::fromTheme("select-tool"), "", this));

    _toolBar_editorToolSelection->addSeparator();

    _toolBar_editorToolSelection->addAction(_action_selectBrushTool);
    _toolBar_editorToolSelection->addAction(_action_selectEraserTool);
    _toolBar_editorToolSelection->addAction(new QAction(QIcon::fromTheme("fill-tool"), "", this));

    _toolBar_editorToolSelection->addSeparator();

    _toolBar_editorToolSelection->addAction(new QAction(QIcon::fromTheme("text-tool"), "", this));
    _toolBar_editorToolSelection->addAction(new QAction(QIcon::fromTheme("shapes-tool"), "", this));
    _toolBar_editorToolSelection->addAction(new QAction(QIcon::fromTheme("stickers-tool"), "", this));

    _toolBar_editorToolSelection->addSeparator();

    _toolBar_editorToolSelection->addAction(_action_selectNavigateTool);
    _toolBar_editorToolSelection->addAction(new QAction(QIcon::fromTheme("eyedrop-tool"), "", this));
    _toolBar_editorToolSelection->addAction(new QAction(QIcon::fromTheme("measure-tool"), "", this));

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    _layersManager = new LayersManager(this);
    addDockWidget(Qt::RightDockWidgetArea, _layersManager);

    _colorSelector = new ColorSelector(_presenter.colorSelection(), this);

    addDockWidget(Qt::BottomDockWidgetArea, _colorSelector);

    _fileDialogHelper = new FileDialogHelper(this);
}

void MainEditorView::onUndoStateChanged()
{
//     try
//     {
//         TODO: replace with PropertyBindings
// 
//         _action_undo->setEnabled(_presenter.canUndo());
//         _action_redo->setEnabled(_presenter.canRedo());
//     }
//     ADDLE_SLOT_CATCH
}

void MainEditorView::onAction_open()
{
//     try
//     {
//         QSharedPointer<FileRequest> request(new FileRequest(FileRequest::Load));
//         _fileDialogHelper->setRequest(request);
//         
//         _presenter.loadDocument(request);
//         
//         QUrl file = QFileDialog::getOpenFileUrl(
//             this, 
//             qtTrId("ui.open-document.title"),
//             QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))
//         );
// 
//         if (!file.isEmpty())
//             _presenter->loadDocument(file);
//     }
//     ADDLE_SLOT_CATCH
}

void MainEditorView::onAction_save()
{
    try
    {
//         QSharedPointer<FileRequest> request(new FileRequest(FileRequest::Save));
//         _fileDialogHelper->setRequest(request);
// 
//         _presenter->saveDocument(request);
    }
    ADDLE_SLOT_CATCH
}

void MainEditorView::onUrgentMessageMade(ITopLevelView* view)
{
    QWidget* viewWidget = qobject_interface_cast<QWidget*>(view);
    ADDLE_ASSERT(viewWidget);
    
    auto wf = viewWidget->windowFlags() | Qt::Window;
    viewWidget->setParent(this);
    viewWidget->setWindowFlags(wf);
    viewWidget->setWindowModality(Qt::WindowModal);
}

void MainEditorView::onToolBarNeedsShown()
{
    try
    {
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
        if (!empty && _viewPort)
            _viewPort->setFocus();
    }
    ADDLE_SLOT_CATCH
}

void MainEditorView::onDocumentChanged(QSharedPointer<IDocumentPresenter> document)
{
    try
    {
        _layersManager->setPresenter(document);

        _action_save->setEnabled(static_cast<bool>(document));
    }
    ADDLE_SLOT_CATCH
}

void MainEditorView::closeEvent(QCloseEvent* event)
{
    try
    {
        // the presenter will probably have something to say about this

        event->accept();
        emit tlv_closed();
    }
    ADDLE_EVENT_CATCH(event)
}
