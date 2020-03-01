#include "basedocumentview.hpp"

#include "widgetsgui/widgets/viewportwidget.hpp"
#include "widgetsgui/widgets/zoomrotatewidget.hpp"

#include "servicelocator.hpp"

#include <QCoreApplication>

#include <QSignalBlocker>

#include <QAbstractButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"
#include "interfaces/views/iviewport.hpp"
#include "interfaces/services/iapplicationsservice.hpp"
#include "utilities/qt_extensions/qobject.hpp"

void BaseDocumentView::initialize(IDocumentPresenter* presenter)
{
    _initHelper.initializeBegin();

    _presenter = presenter;
    _viewPortPresenter = _presenter->getViewPortPresenter();
    
    connect_interface(_presenter, SIGNAL(raiseError(QSharedPointer<IErrorPresenter>)), this, SLOT(onPresenterErrorRaised(QSharedPointer<IErrorPresenter>)));
    connect_interface(_presenter, SIGNAL(documentChanged(QSharedPointer<IDocument>)), this, SLOT(onDocumentChanged()));

    _initHelper.initializeEnd();
}

void BaseDocumentView::setupUi()
{
    _initHelper.assertInitialized();

    QMainWindow::resize(800, 679);

    _menuBar = new QMenuBar(this);
    QMainWindow::setMenuBar(_menuBar);

    _toolBar_documentActions = new QToolBar(this);
    _toolBar_documentActions->setWindowTitle(tr("document-actions"));
    QMainWindow::addToolBar(Qt::ToolBarArea::TopToolBarArea, _toolBar_documentActions);

    _toolBar_toolSelection = new QToolBar(this);
    _toolBar_toolSelection->setWindowTitle(tr("tool-selection"));
    QMainWindow::addToolBar(Qt::ToolBarArea::LeftToolBarArea, _toolBar_toolSelection);

    _viewPortWidget = new ViewPortWidget(*_viewPortPresenter, this);
    QMainWindow::setCentralWidget(_viewPortWidget);

    _statusBar = new QStatusBar(this);
    QMainWindow::setStatusBar(_statusBar);

    _zoomRotateWidget = new ZoomRotateWidget(*_viewPortPresenter, this);
    _statusBar->addPermanentWidget(_zoomRotateWidget);
    _zoomRotateWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _action_open = new QAction(this);
    _action_open->setIcon(QIcon(":/icons/open.png"));
    connect(_action_open, &QAction::triggered, this, &BaseDocumentView::onAction_open);

    _actionGroup_toolSelection = new PropertyActionGroup(this, *_presenter, "currentTool");
}

void BaseDocumentView::start()
{
    _initHelper.assertInitialized();
    setupUi();
    QWidget::show();
}


void BaseDocumentView::onAction_open()
{
    _presenter->onActionLoadDocument(QFileDialog::getOpenFileName(
        this, 
        "Open image",
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
    ));
}

void BaseDocumentView::onDocumentChanged()
{
    _viewPortWidget->updateScrollBars();
    _zoomRotateWidget->updateChildren();
}

void BaseDocumentView::onPresenterErrorRaised(QSharedPointer<IErrorPresenter> error)
{
    _initHelper.assertInitialized();

    QMessageBox message(this);
    message.setText(error->getMessage());
    if (error->getSeverity() == IErrorPresenter::warning)
    {
        message.setIcon(QMessageBox::Icon::Warning);
    }

    message.exec();
}
