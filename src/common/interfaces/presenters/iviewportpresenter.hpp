#ifndef IVIEWPORTPRESENTER_HPP
#define IVIEWPORTPRESENTER_HPP

#include <QObject>
#include <QPointF>
#include <QTransform>

#include "idocumentpresenter.hpp"

#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"

class IViewPort;
class IViewPortPresenter
{
public:

    enum ZoomPreset {
        nullzoom,
        _5percent,
        _10percent,
        onesixth,
        _25percent,
        onethird,
        _50percent,
        _75percent,
        _100percent,
        _150percent,
        _200percent,
        _300percent,
        _400percent,
        _500percent,
        _800percent,
        _1000percent,
        _1600percent,
        _2000percent
    };

    enum RotatePreset {
        nullrotation,
        _0deg,
        _45deg,
        _90deg,
        _135deg,
        _180deg,
        _225deg,
        _270deg,
        _315deg
    };

    const ZoomPreset MIN_ZOOM_PRESET = ZoomPreset::_5percent;
    const ZoomPreset MAX_ZOOM_PRESET = ZoomPreset::_2000percent;
    const ZoomPreset DEFAULT_ZOOM_PRESET = ZoomPreset::_100percent;

    const RotatePreset DEFAULT_ROTATE_PRESET = RotatePreset::_0deg;

    virtual ~IViewPortPresenter() = default;

    virtual void initialize(IDocumentPresenter* documentPresenter) = 0;

    virtual IViewPort* getViewPort() = 0;
    virtual IDocumentPresenter* getDocumentPresenter() = 0;

    // # Scrolling / positioning
public:
    virtual QPointF getPosition() = 0;
    virtual void setPosition(QPointF center) = 0;

    virtual bool canScroll() = 0;
    virtual QRect getScrollRect() = 0;
    virtual void scrollX(int x) = 0;
    virtual void scrollY(int y) = 0;
    virtual void setScrollPosition(QPoint position) = 0;

    virtual void gripPan(QPointF start, QPointF end) = 0;

signals:
    virtual void positionChanged(QPointF position) = 0;
    virtual void scrollStateChanged() = 0;

    // # Zooming
public:
    virtual bool canZoom() = 0;
    virtual bool canZoomIn() = 0;
    virtual bool canZoomOut() = 0;

    virtual double getZoom() = 0;
    virtual void setZoom(double zoom) = 0;
    //virtual void gripZoom(QPoint gripStart, QPoint gripEnd) = 0;

    virtual ZoomPreset getZoomPreset() = 0;
    virtual void setZoomPreset(ZoomPreset preset) = 0;
    virtual double getMaxZoomPresetValue() = 0;
    virtual double getMinZoomPresetValue() = 0;
    
    virtual ZoomPreset zoomTo(double zoom, bool snapToPreset = true) = 0;

public slots:
    virtual ZoomPreset zoomIn(bool* zoomed = nullptr) = 0;
    virtual ZoomPreset zoomOut(bool* zoomed = nullptr) = 0;

signals:
    virtual void zoomChanged(double zoom) = 0;

    // # Rotation
public:
    virtual double getRotation() = 0;
    virtual void setRotation(double rotation) = 0;


public slots:
    virtual RotatePreset turntableCcw(bool* rotated = nullptr) = 0;
    virtual RotatePreset turntableCw(bool* rotated = nullptr) = 0;

signals:
    virtual void rotationChanged(double rotation) = 0;

    // # General transforming
public:
    virtual QSize getViewPortSize() = 0;
    virtual QPoint getGlobalOffset() = 0;
    virtual QPointF getCenter() = 0;

    virtual void gripPivot(QPointF start, QPointF end) = 0;

    virtual QTransform getOntoCanvasTransform() = 0;
    virtual QTransform getFromCanvasTransform() = 0;

public slots:
    virtual void resetView() = 0;
    virtual void fitWidth() = 0;
    virtual void fitCanvas() = 0;

    virtual void setViewPortSize(QSize size) = 0;
    virtual void setGlobalOffset(QPoint offset) = 0;

signals:
    virtual void transformsChanged() = 0;
};

DECL_MAKEABLE(IViewPortPresenter)
DECL_EXPECTS_INITIALIZE(IViewPortPresenter)
DECL_INIT_DEPENDENCY(IViewPortPresenter, IDocumentPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IViewPortPresenter)

#endif // IVIEWPORTPRESENTER_HPP
