#include "zoomrotatewidget.hpp"

#include <QHBoxLayout>

#include <cmath>

#include "utilities/qt_extensions/qobject.hpp"

ZoomRotateWidget::ZoomRotateWidget(IViewPortPresenter& presenter, QWidget* parent)
    : QWidget(parent),
    _presenter(presenter)
{
    _button_zoomIn = new QToolButton(this);
    _action_zoomIn = new QAction(this);
    _action_zoomIn->setIcon(QIcon(":/icons/zoom_in.png"));
    _button_zoomIn->setDefaultAction(_action_zoomIn);
    connect_interface(_action_zoomIn, SIGNAL(triggered()), &_presenter, SLOT(zoomIn()));

    _button_zoomOut = new QToolButton(this);
    _action_zoomOut = new QAction(this);
    _action_zoomOut->setIcon(QIcon(":/icons/zoom_out.png"));
    _button_zoomOut->setDefaultAction(_action_zoomOut);
    connect_interface(_action_zoomOut, SIGNAL(triggered()), &_presenter, SLOT(zoomOut()));

    _button_turntableCW = new QToolButton(this);
    _action_turntableCW = new QAction(this);
    _action_turntableCW->setIcon(QIcon(":/icons/turntable_cw.png"));
    _button_turntableCW->setDefaultAction(_action_turntableCW);
    connect_interface(_action_turntableCW, SIGNAL(triggered()), &_presenter, SLOT(turntableCw()));

    _button_turntableCCW = new QToolButton(this);
    _action_turntableCCW = new QAction(this);
    _action_turntableCCW->setIcon(QIcon(":/icons/turntable_ccw.png"));
    _button_turntableCCW->setDefaultAction(_action_turntableCCW);
    connect_interface(_action_turntableCCW, SIGNAL(triggered()), &_presenter, SLOT(turntableCcw()));

    _button_reset = new QToolButton(this);
    _action_reset = new QAction(this);
    _action_reset->setIcon(QIcon(":/icons/reset_view.png"));
    _button_reset->setDefaultAction(_action_reset);
    connect_interface(_action_reset, SIGNAL(triggered()), &_presenter, SLOT(resetView()));

    _slider_zoom = new ZoomSlider(this);
    _slider_zoom->setMinZoom(_presenter.getMinZoomPresetValue());
    _slider_zoom->setMaxZoom(_presenter.getMaxZoomPresetValue());
    _slider_zoom->setOrientation(Qt::Orientation::Horizontal);
    _slider_zoom->setMinimumSize(QSize(150, 0));
    _slider_zoom->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _spinBox_zoomPercent = new QDoubleSpinBox(this);
    _spinBox_zoomPercent->setMinimum(_presenter.getMinZoomPresetValue() * 100);
    _spinBox_zoomPercent->setMaximum(_presenter.getMaxZoomPresetValue() * 100);

    QHBoxLayout* layout = new QHBoxLayout(this);
    QWidget::setLayout(layout);

    layout->addWidget(_button_turntableCCW);
    layout->addWidget(_button_zoomOut);
    layout->addWidget(_slider_zoom);
    layout->addWidget(_spinBox_zoomPercent);
    layout->addWidget(_button_zoomIn);
    layout->addWidget(_button_turntableCW);
    layout->addWidget(_button_reset);

    connect_interface(&_presenter, SIGNAL(zoomChanged(double)), this, SLOT(onPresenterZoomChanged()));
    connect_interface(&_presenter, SIGNAL(rotationChanged(double)), this, SLOT(onPresenterRotationChanged()));
    connect(
        _spinBox_zoomPercent,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        [&](double value) {
            double zoom = value / 100;
            _presenter.zoomTo(zoom, false);
        }
    );
    connect(
        _slider_zoom,
        &QSlider::valueChanged,
        [&](int value)
        {
            double zoom = _slider_zoom->getZoomLevel();
            _presenter.zoomTo(zoom);
        }
    );

    updateChildren();
}

void ZoomRotateWidget::updateZoom()
{    
    const QSignalBlocker block_slider_zoom(_slider_zoom);
    const QSignalBlocker block_zoom_percent(_spinBox_zoomPercent);

    if (_presenter.canZoom())
    {
        double zoom = _presenter.getZoom();

        _slider_zoom->setEnabled(true);
        _slider_zoom->setZoomLevel(zoom);
        _spinBox_zoomPercent->setEnabled(true);
        _spinBox_zoomPercent->setValue(zoom * 100);

        _button_zoomIn->setEnabled(_presenter.canZoomIn());
        _button_zoomOut->setEnabled(_presenter.canZoomOut());
    }
    else
    {
        _slider_zoom->setEnabled(false);
        _slider_zoom->setZoomLevel(1);
        _spinBox_zoomPercent->setEnabled(false);
        _spinBox_zoomPercent->setValue(100);
        _button_zoomIn->setEnabled(false);
        _button_zoomOut->setEnabled(false);
    }
}

void ZoomRotateWidget::updateRotate()
{

}

void ZoomRotateWidget::onPresenterZoomChanged()
{
    updateZoom();
}

void ZoomRotateWidget::onPresenterRotationChanged()
{
    updateRotate();
}