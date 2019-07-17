#include "views/mainwindow.h"
#include "forms/headers/ui_mainwindow.h"
#include "forms/headers/ui_mainwindow_zoomwidget.h"

#include "servicelocator.h"

#include <QAbstractButton>
#include "editorviewport.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _zoomWidgetUi(new Ui::MainWindow_ZoomWidget)
{
    _ui->setupUi(this);

    _presenter = ServiceLocator::make<IEditorPresenter>();
    _editorViewPortPresenter = _presenter->getEditorViewPortPresenter();

    _ui->editorViewPort->initialize(_editorViewPortPresenter);

    connect(_ui->action_turntableCcw, SIGNAL(triggered()), dynamic_cast<QObject*>(_editorViewPortPresenter), SLOT(turntableCcw()));
    connect(_ui->action_turntableCw, SIGNAL(triggered()), dynamic_cast<QObject*>(_editorViewPortPresenter), SLOT(turntableCw()));
    connect(_ui->action_zoomIn, SIGNAL(triggered()), dynamic_cast<QObject*>(_editorViewPortPresenter), SLOT(zoomIn()));
    connect(_ui->action_zoomOut, SIGNAL(triggered()), dynamic_cast<QObject*>(_editorViewPortPresenter), SLOT(zoomOut()));

    _zoomWidget = new QWidget(this);
    _zoomWidgetUi->setupUi(_zoomWidget);
    _ui->statusBar->addPermanentWidget(_zoomWidget);
    _zoomWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    _zoomWidgetUi->button_turntable_ccw->setDefaultAction(_ui->action_turntableCcw);
    _zoomWidgetUi->button_turntable_cw->setDefaultAction(_ui->action_turntableCw);
    _zoomWidgetUi->button_zoom_plus->setDefaultAction(_ui->action_zoomIn);
    _zoomWidgetUi->button_zoom_minus->setDefaultAction(_ui->action_zoomOut);

    _zoomWidgetUi->slider_zoom->setMinZoom(_editorViewPortPresenter->getMinZoomPresetValue());
    _zoomWidgetUi->slider_zoom->setMaxZoom(_editorViewPortPresenter->getMaxZoomPresetValue());
    _zoomWidgetUi->spinBox_zoom_percent->setMinimum(_editorViewPortPresenter->getMinZoomPresetValue() * 100);
    _zoomWidgetUi->spinBox_zoom_percent->setMaximum(_editorViewPortPresenter->getMaxZoomPresetValue() * 100);
    connect(_zoomWidgetUi->spinBox_zoom_percent, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::zoomWidgetSpinBoxValueChanged);
    connect(_zoomWidgetUi->slider_zoom, &QSlider::valueChanged, this, &MainWindow::zoomWidgetSliderValueChanged);
    
    updateZoomWidget();
}

MainWindow::~MainWindow()
{
    delete _ui;
    delete _zoomWidgetUi;
}

void MainWindow::updateZoomWidget()
{
    double zoom = _editorViewPortPresenter->getZoom();

    _zoomWidgetUi->slider_zoom->setZoomLevel(zoom);
    _zoomWidgetUi->spinBox_zoom_percent->setValue(zoom * 100);

    _zoomWidgetUi->button_zoom_plus->setEnabled(_editorViewPortPresenter->canZoomIn());
    _zoomWidgetUi->button_zoom_minus->setEnabled(_editorViewPortPresenter->canZoomOut());
}


void MainWindow::zoomWidgetSpinBoxValueChanged(double value)
{
    //Weird bug
    /*double zoom = value / 100;
    _editor->zoomTo(zoom, false);

    updateZoomWidget();
    update();*/
}

void MainWindow::zoomWidgetSliderValueChanged(int value)
{
    double zoom = _zoomWidgetUi->slider_zoom->getZoomLevel();
    _editorViewPortPresenter->zoomTo(zoom);

    updateZoomWidget();
    update();
}