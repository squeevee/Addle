/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ZOOMSLIDER_HPP
#define ZOOMSLIDER_HPP

#include <QSlider>

namespace Addle {

class ZoomSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(
        double zoom
        READ zoom
        WRITE setZoom
        NOTIFY zoomChanged
    )

public:
    const int ZOOM_SLIDER_VALUE_SPAN = 100;

    static constexpr const char* ZOOM_PROPERTY_NAME = "zoom";

    ZoomSlider(QWidget* parent);
    virtual ~ZoomSlider();

    double zoom() { return mapValueToZoom(value()); }
    void setZoom(double zoom) { setValue(mapZoomToValue(zoom)); emit zoomChanged(zoom); }

    double maxZoom() { return _maxZoom; }
    void setMaxZoom(double max);

    double minZoom() { return _minZoom; }
    void setMinZoom(double min);
    
signals:
    void zoomChanged(double zoom);

private:
    int mapZoomToValue(double zoom);
    double mapValueToZoom(int value);

    //TODO: These should come from the presenter
    double _maxZoom = 10.0;
    double _minZoom = 0.10;
};

} // namespace Addle

#endif // ZOOMSLIDER_HPP
