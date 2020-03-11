#ifndef ZOOMSLIDER_HPP
#define ZOOMSLIDER_HPP

#include <QSlider>

class ZoomSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(
        double zoom
        READ getZoom
        WRITE setZoom
        NOTIFY zoomChanged
    )

public:
    const int ZOOM_SLIDER_VALUE_SPAN = 100;

    static constexpr const char* ZOOM_PROPERTY_NAME = "zoom";

    ZoomSlider(QWidget* parent);
    virtual ~ZoomSlider();

    double getZoom() { return mapValueToZoom(value()); }
    void setZoom(double zoom) { setValue(mapZoomToValue(zoom)); emit zoomChanged(zoom); }

    double getMaxZoom() { return _maxZoom; }
    void setMaxZoom(double max);

    double getMinZoom() { return _minZoom; }
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

#endif // ZOOMSLIDER_HPP