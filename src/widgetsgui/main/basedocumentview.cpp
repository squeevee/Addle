#include "basedocumentview.hpp"

#include "viewportscrollwidget.hpp"
#include "zoomrotatewidget.hpp"

#include "servicelocator.hpp"

#include <QCoreApplication>

#include <QSignalBlocker>

#include <QAbstractButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

#include "interfaces/views/iviewport.hpp"
#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"
#include "interfaces/services/iapplicationsservice.hpp"
#include "utilities/qt_extensions/qobject.hpp"

#include "utilities/propertybinding.hpp"

#include "widgetsgui/utilities/widgetproperties.hpp"

void BaseDocumentView::initialize(IDocumentPresenter* presenter)
{
    _initHelper.initializeBegin();

    _presenter = presenter;
    _viewPortPresenter = _presenter->getViewPortPresenter();
    
    connect_interface(_presenter, SIGNAL(raiseError(QSharedPointer<IErrorPresenter>)),
        this, SLOT(onPresenterErrorRaised(QSharedPointer<IErrorPresenter>)));
    connect_interface(_presenter, SIGNAL(documentChanged(QSharedPointer<IDocument>)), 
        this, SLOT(onDocumentChanged()));

    _initHelper.initializeEnd();
}

void BaseDocumentView::setupUi()
{
    _initHelper.assertInitialized();

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
        IDocumentPresenter::Meta::Properties::empty,
        PropertyBinding::ReadOnly,
        BindingConverter::negate()
    );

    _viewPortScrollWidget = new ViewPortScrollWidget(*_viewPortPresenter, this);
    QMainWindow::setCentralWidget(_viewPortScrollWidget);

    _viewPort = ServiceLocator::make<IViewPort>(_viewPortPresenter, _presenter->getCanvasPresenter());
    _viewPortScrollWidget->setViewPort(_viewPort);

    _statusBar = new QStatusBar(this);
    QMainWindow::setStatusBar(_statusBar);

    _zoomRotateWidget = new ZoomRotateWidget(*_viewPortPresenter, this);
    _statusBar->addPermanentWidget(_zoomRotateWidget);
    _zoomRotateWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _action_open = new QAction(this);
    _action_open->setIcon(QIcon(":/icons/open.png"));
    connect(_action_open, &QAction::triggered, this, &BaseDocumentView::onAction_open);

    _optionGroup_toolSelection = new OptionGroup(this);
    new PropertyBinding(
        _optionGroup_toolSelection,
        WidgetProperties::value,
        qobject_interface_cast(_presenter),
        IDocumentPresenter::Meta::Properties::currentTool
    );
}

void BaseDocumentView::start()
{
    _initHelper.assertInitialized();
    setupUi();
    QWidget::show();
}


void BaseDocumentView::onAction_open()
{
    QUrl file = QFileDialog::getOpenFileUrl(
        this, 
        "Open image",
        QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))
    );

    if (!file.isEmpty())
        _presenter->loadDocument(file);
}

void BaseDocumentView::onDocumentChanged()
{

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
