#include "basedocumentview.hpp"
#include "ui_documentview.h"
#include "ui_zoomrotatewidget.h"

#include "common/servicelocator.hpp"

#include <QCoreApplication>

#include <QSignalBlocker>

#include <QAbstractButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

#include "common/interfaces/views/iviewport.hpp"
#include "common/interfaces/services/iapplicationsservice.hpp"
#include "common/utilities/qt_extensions/qobject.hpp"

BaseDocumentView::~BaseDocumentView()
{
    delete _ui;
    delete _zoomRotateWidgetUi;
}

void BaseDocumentView::initialize(IDocumentPresenter* presenter)
{
    _initHelper.initializeBegin();

    _presenter = presenter;
    _viewPortPresenter = _presenter->getViewPortPresenter();
    
    connect_interface(_presenter, SIGNAL(raiseError(QSharedPointer<IErrorPresenter>)), this, SLOT(onPresenterErrorRaised(QSharedPointer<IErrorPresenter>)));
    connect_interface(_presenter, SIGNAL(documentChanged(QSharedPointer<IDocument>)), this, SLOT(onPresenterDocumentLoaded()));

    _initHelper.initializeEnd();
}

void BaseDocumentView::setupUi()
{
    _ui = new Ui::DocumentView;
    _zoomRotateWidgetUi = new Ui::ZoomRotateWidget;

    _ui->setupUi(this);

    _zoomRotateWidget = new QWidget(this);
    _zoomRotateWidgetUi->setupUi(_zoomRotateWidget);
    _ui->statusBar->addPermanentWidget(_zoomRotateWidget);
    _zoomRotateWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    connect(_zoomRotateWidgetUi->spinBox_zoom_percent, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &BaseDocumentView::onZoomSpinBoxValueChanged);
    connect(_zoomRotateWidgetUi->slider_zoom, &QSlider::valueChanged, this, &BaseDocumentView::onZoomSliderValueChanged);

    connect(_ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenFileAction()));

    QWidget* viewPort = dynamic_cast<QWidget*>(_viewPortPresenter->getViewPort());
    _ui->viewPortLayout->addWidget(viewPort, 0, 0);
    viewPort->setFocus();

    connect_interface(_ui->action_turntableCcw, SIGNAL(triggered()), _viewPortPresenter, SLOT(turntableCcw()));
    connect_interface(_ui->action_turntableCw, SIGNAL(triggered()), _viewPortPresenter, SLOT(turntableCw()));
    connect_interface(_ui->action_zoomIn, SIGNAL(triggered()), _viewPortPresenter, SLOT(zoomIn()));
    connect_interface(_ui->action_zoomOut, SIGNAL(triggered()), _viewPortPresenter, SLOT(zoomOut()));

    connect_interface(_viewPortPresenter, SIGNAL(zoomChanged(double)), this, SLOT(onViewPortZoomChanged(double)));
    connect_interface(_viewPortPresenter, SIGNAL(rotationChanged(double)), this, SLOT(onViewPortRotationChanged(double)));
    connect_interface(_viewPortPresenter, SIGNAL(scrollStateChanged()), this, SLOT(onViewPortScrollStateChanged()));

    _zoomRotateWidgetUi->button_turntable_ccw->setDefaultAction(_ui->action_turntableCcw);
    _zoomRotateWidgetUi->button_turntable_cw->setDefaultAction(_ui->action_turntableCw);
    _zoomRotateWidgetUi->button_zoom_plus->setDefaultAction(_ui->action_zoomIn);
    _zoomRotateWidgetUi->button_zoom_minus->setDefaultAction(_ui->action_zoomOut);

    _zoomRotateWidgetUi->slider_zoom->setMinZoom(_viewPortPresenter->getMinZoomPresetValue());
    _zoomRotateWidgetUi->slider_zoom->setMaxZoom(_viewPortPresenter->getMaxZoomPresetValue());
    _zoomRotateWidgetUi->spinBox_zoom_percent->setMinimum(_viewPortPresenter->getMinZoomPresetValue() * 100);
    _zoomRotateWidgetUi->spinBox_zoom_percent->setMaximum(_viewPortPresenter->getMaxZoomPresetValue() * 100);

    connect(_ui->viewPortVerticalScrollBar, &QScrollBar::valueChanged, this, &BaseDocumentView::onVerticalViewPortScrollBarValueChanged);
    connect(_ui->viewPortHorizontalScrollBar, &QScrollBar::valueChanged, this, &BaseDocumentView::onHorizontalViewPortScrollBarValueChanged);

    _ui->viewPortVerticalScrollBar->setSingleStep(10);
    _ui->viewPortHorizontalScrollBar->setSingleStep(10);

    updateViewPortScrollBars();
    updateZoomWidget();
}

void BaseDocumentView::start()
{
    _initHelper.assertInitialized();
    setupUi();
    QWidget::show();
}

void BaseDocumentView::updateZoomWidget()
{
    _initHelper.assertInitialized();

    const QSignalBlocker block_slider_zoom(_zoomRotateWidgetUi->slider_zoom);
    const QSignalBlocker block_zoom_percent(_zoomRotateWidgetUi->spinBox_zoom_percent);

    if (_viewPortPresenter->canZoom())
    {
        double zoom = _viewPortPresenter->getZoom();

        _zoomRotateWidgetUi->slider_zoom->setEnabled(true);
        _zoomRotateWidgetUi->slider_zoom->setZoomLevel(zoom);
        _zoomRotateWidgetUi->spinBox_zoom_percent->setEnabled(true);
        _zoomRotateWidgetUi->spinBox_zoom_percent->setValue(zoom * 100);

        _zoomRotateWidgetUi->button_zoom_plus->setEnabled(_viewPortPresenter->canZoomIn());
        _zoomRotateWidgetUi->button_zoom_minus->setEnabled(_viewPortPresenter->canZoomOut());
    }
    else
    {
        _zoomRotateWidgetUi->slider_zoom->setEnabled(false);
        _zoomRotateWidgetUi->slider_zoom->setZoomLevel(1);
        _zoomRotateWidgetUi->spinBox_zoom_percent->setEnabled(false);
        _zoomRotateWidgetUi->spinBox_zoom_percent->setValue(100);
        _zoomRotateWidgetUi->button_zoom_plus->setEnabled(false);
        _zoomRotateWidgetUi->button_zoom_minus->setEnabled(false);
    }
}


void BaseDocumentView::updateViewPortScrollBars()
{
    _initHelper.assertInitialized();

    const QSignalBlocker block_veritcal_scroll(_ui->viewPortVerticalScrollBar);
    const QSignalBlocker block_horizontal_scroll(_ui->viewPortHorizontalScrollBar);

    if (_viewPortPresenter->canScroll())
    {
        QRect scrollRect = _viewPortPresenter->getScrollRect();

        int height = _viewPortPresenter->getSize().height();
        _ui->viewPortVerticalScrollBar->setEnabled(true);
        _ui->viewPortVerticalScrollBar->setMinimum(scrollRect.y());
        _ui->viewPortVerticalScrollBar->setMaximum(scrollRect.y() + scrollRect.height());
        _ui->viewPortVerticalScrollBar->setPageStep(height);
        _ui->viewPortVerticalScrollBar->setValue(0);

        int width = _viewPortPresenter->getSize().width();
        _ui->viewPortHorizontalScrollBar->setEnabled(true);
        _ui->viewPortHorizontalScrollBar->setMinimum(scrollRect.x());
        _ui->viewPortHorizontalScrollBar->setMaximum(scrollRect.x() + scrollRect.width());
        _ui->viewPortHorizontalScrollBar->setPageStep(width);
        _ui->viewPortHorizontalScrollBar->setValue(0);
    }
    else
    {
        _ui->viewPortVerticalScrollBar->setEnabled(false);
        _ui->viewPortVerticalScrollBar->setMinimum(0);
        _ui->viewPortVerticalScrollBar->setMaximum(0);
        _ui->viewPortVerticalScrollBar->setPageStep(0);

        _ui->viewPortHorizontalScrollBar->setEnabled(false);
        _ui->viewPortHorizontalScrollBar->setMinimum(0);
        _ui->viewPortHorizontalScrollBar->setMaximum(0);
        _ui->viewPortHorizontalScrollBar->setPageStep(0);
    }
}

void BaseDocumentView::onVerticalViewPortScrollBarValueChanged(int value)
{
    _viewPortPresenter->scrollY(value);
}

void BaseDocumentView::onHorizontalViewPortScrollBarValueChanged(int value)
{
    _viewPortPresenter->scrollX(value);
}

void BaseDocumentView::onZoomSpinBoxValueChanged(double value)
{
    _initHelper.assertInitialized();

    double zoom = value / 100;
    _viewPortPresenter->zoomTo(zoom, false);
}

void BaseDocumentView::onZoomSliderValueChanged(int value)
{
    _initHelper.assertInitialized();

    double zoom = _zoomRotateWidgetUi->slider_zoom->getZoomLevel();
    _viewPortPresenter->zoomTo(zoom);
}

void BaseDocumentView::onViewPortZoomChanged(double zoom)
{
    updateZoomWidget();
    QWidget::update();
}

void BaseDocumentView::onViewPortRotationChanged(double rotation)
{
}

void BaseDocumentView::onViewPortScrollStateChanged()
{
    updateViewPortScrollBars();
    QWidget::update();
}

void BaseDocumentView::onOpenFileAction()
{
    _initHelper.assertInitialized();

    QString filename = QFileDialog::getOpenFileName(
        this,
        "Open PNG image",
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
        "Supported image files (*.png *.jpg)"
        );

    _presenter->onActionLoadDocument(filename);
}

void BaseDocumentView::onPresenterDocumentLoaded()
{
    updateViewPortScrollBars();
    updateZoomWidget();
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