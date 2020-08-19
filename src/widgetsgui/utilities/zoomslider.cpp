/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "zoomslider.hpp"

#include <QStyle>
#include <QPainter>

#include <cmath>

using namespace Addle;

ZoomSlider::ZoomSlider(QWidget* parent)
    : QSlider(parent)
{
    setMinimum(0);
    setMaximum(ZOOM_SLIDER_VALUE_SPAN);

    connect(this, &QAbstractSlider::valueChanged, this, [this](int value) { emit zoomChanged(zoom()); });
}

ZoomSlider::~ZoomSlider()
{
}

void ZoomSlider::setMaxZoom(double max)
{
    _maxZoom = max;
    update();
}

void ZoomSlider::setMinZoom(double min)
{
    _minZoom = min;
    update();
}

int ZoomSlider::mapZoomToValue(double zoom)
{
    double lzoom = log10(zoom);
    double lmin = log10(_minZoom);
    double lmax = log10(_maxZoom);

    return (lzoom - lmin) * (ZOOM_SLIDER_VALUE_SPAN / (lmax - lmin));
}

double ZoomSlider::mapValueToZoom(int value)
{
    double lmin = log10(_minZoom);
    double lmax = log10(_maxZoom);
    double lzoom = (value * (lmax - lmin)) / ZOOM_SLIDER_VALUE_SPAN + lmin;

    return pow(10, lzoom);
}
