#include "maineditorview.hpp"

#include "zoomrotatewidget.hpp"
#include "viewportscrollwidget.hpp"

#include "servicelocator.hpp"

#include <QCoreApplication>

#include <QSignalBlocker>

#include <QAbstractButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QApplication>

#include "viewport.hpp"

#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"
#include "interfaces/services/iapplicationsservice.hpp"
#include "utilities/qtextensions/qobject.hpp"

#include "utilities/presenter/propertybinding.hpp"

#include "widgetsgui/utilities/widgetproperties.hpp"

#include "helpers/editortoolshelper.hpp"

MainEditorView::~MainEditorView()
{
    if (_editorTools) delete _editorTools;
}

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
    _toolBar_documentActions->setWindowTitle(tr("document-actions"));
    QMainWindow::addToolBar(Qt::ToolBarArea::TopToolBarArea, _toolBar_documentActions);

    _toolBar_toolSelection = new QToolBar(this);
    _toolBar_toolSelection->setWindowTitle(tr("tool-selection"));
    QMainWindow::addToolBar(Qt::ToolBarArea::LeftToolBarArea, _toolBar_toolSelection);

    new PropertyBinding(
        _toolBar_toolSelection,
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

    _statusBar = new QStatusBar(this);
    QMainWindow::setStatusBar(_statusBar);

    _zoomRotateWidget = new ZoomRotateWidget(*viewPortPresenter, this);
    _statusBar->addPermanentWidget(_zoomRotateWidget);
    _zoomRotateWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _action_open = new QAction(this);
    _action_open->setIcon(QIcon(":/icons/open.png"));
    connect(_action_open, &QAction::triggered, this, &MainEditorView::onAction_open);

    _optionGroup_toolSelection = new OptionGroup(this);
    // new PropertyBinding(
    //     _optionGroup_toolSelection,
    //     WidgetProperties::value,
    //     qobject_interface_cast(_presenter),
    //     IDocumentPresenter::Meta::Properties::currentTool
    // );

    _action_new = new QAction(this);
    _action_new->setIcon(QIcon(":/icons/new.png"));
    connect_interface(_action_new, SIGNAL(triggered()), _presenter, SLOT(newDocument()));

    _action_undo = new QAction(this);
    _action_undo->setIcon(QIcon(":/icons/undo.png"));
    _action_undo->setEnabled(_presenter->canUndo());
    connect_interface(_action_undo, SIGNAL(triggered()), _presenter, SLOT(undo()));

    _action_redo = new QAction(this);
    _action_redo->setIcon(QIcon(":/icons/redo.png"));
    _action_redo->setEnabled(_presenter->canRedo());
    connect_interface(_action_redo, SIGNAL(triggered()), _presenter, SLOT(redo()));
    
    _toolBar_documentActions->addAction(_action_new);
    _toolBar_documentActions->addAction(_action_open);
    _toolBar_documentActions->addSeparator();
    _toolBar_documentActions->addAction(_action_undo);
    _toolBar_documentActions->addAction(_action_redo);

    QPalette p = QApplication::palette();
    // QColor backgroundColor = p.color(QPalette::Window);
    // backgroundColor.setAlpha(172);
    // p.setColor(QPalette::Base, backgroundColor);
    // p.setColor(QPalette::Window, backgroundColor);
    // p.setColor(QPalette::Base, Qt::transparent);
    // p.setColor(QPalette::Window, Qt::transparent);
    setPalette(p);

    updateTools();
}

void MainEditorView::updateTools()
{
    switch(_presenter->getMode())
    {
        case IMainEditorPresenter::Editor:
        {
            _editorTools = new EditorToolsHelper(*this);
            // if (_viewerTools)
            // {
            //     delete _viewerTools;
            //     _viewerTools = nullptr;
            // }
            break;
        }
        //TODO
    }
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

// void MainEditorView::resizeEvent(QResizeEvent* event)
// {
//     clearMask();
//     QRect viewPortRect = _viewPort->rect();
//     setMask(mask().subtracted(QRegion(viewPortRect)));
// }

void MainEditorView::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.setClipRegion(QRegion(rect()).subtracted(QRect(_viewPort->pos() + _viewPortScrollWidget->pos(), _viewPort->size())));

    QColor backgroundColor = palette().color(QPalette::Base);
    backgroundColor.setAlpha(172);
    p.fillRect(rect(), backgroundColor);
}