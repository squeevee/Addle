#ifndef ZOOMSLIDER_HPP
#define ZOOMSLIDER_HPP

#include <QSlider>

class ZoomSlider : public QSlider
{
    Q_OBJECT
public:
    const int ZOOM_SLIDER_VALUE_SPAN = 100;

    ZoomSlider(QWidget* parent);
    virtual ~ZoomSlider();

    double getZoomLevel() { return mapValueToZoom(value()); }
    void setZoomLevel(double zoom) { setValue(mapZoomToValue(zoom)); }

    double getMaxZoom() { return _maxZoom; }
    void setMaxZoom(double max);

    double getMinZoom() { return _minZoom; }
    void setMinZoom(double min);

private:
    int mapZoomToValue(double zoom);
    double mapValueToZoom(int value);

    //TODO: These should come from the presenter
    double _maxZoom = 10.0;
    double _minZoom = 0.10;
};

#endif // ZOOMSLIDER_HPP