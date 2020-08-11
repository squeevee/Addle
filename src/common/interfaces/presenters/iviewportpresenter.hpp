#ifndef IVIEWPORTPRESENTER_HPP
#define IVIEWPORTPRESENTER_HPP

#include <QObject>
#include <QPointF>
#include <QTransform>

#include "interfaces/traits.hpp"



#include "interfaces/metaobjectinfo.hpp"

#include "iscrollstate.hpp"

class IMainEditorPresenter;
class IViewPortPresenter
{
public:
    INTERFACE_META(IViewPortPresenter)

    enum ZoomPreset {
        nullzoom = -1,
        _5percent,
        _10percent,
        onesixth,
        _25percent,
        onethird,
        _50percent,
        twothirds,
        _100percent,
        _150percent,
        _200percent,
        _300percent,
        _400percent,
        _600percent,
        _800percent,
        _1600percent
    };

    enum RotatePreset {
        nullrotation = -1,
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
    const ZoomPreset MAX_ZOOM_PRESET = ZoomPreset::_1600percent;
    const ZoomPreset DEFAULT_ZOOM_PRESET = ZoomPreset::_100percent;

    const RotatePreset DEFAULT_ROTATE_PRESET = RotatePreset::_0deg;

    virtual ~IViewPortPresenter() = default;

    virtual void initialize(IMainEditorPresenter* mainEditorPresenter) = 0;
    
    virtual IMainEditorPresenter* getMainEditorPresenter() = 0;

public:
    virtual bool canNavigate() = 0;

    virtual bool hasFocus() const = 0;
    virtual void setHasFocus(bool value) = 0;

signals:
    virtual void canNavigateChanged(bool) = 0;

    virtual void focusChanged(bool focus) = 0;

    // # Scrolling / positioning
public:
    virtual QPointF getPosition() = 0;
    virtual const IScrollState& getScrollState() = 0;

public slots:
    virtual void setPosition(QPointF center) = 0;
    virtual void scrollX(int x) = 0;
    virtual void scrollY(int y) = 0;

    virtual void gripPan(QPointF start, QPointF end) = 0;

signals:
    virtual void positionChanged(QPointF position) = 0;
    virtual void scrollStateChanged() = 0;

    // # Zooming
public:
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
    virtual void zoomChanged(double) = 0;

    // # Rotation
public:
    virtual double getRotation() = 0;
    virtual void setRotation(double) = 0;


public slots:
    virtual RotatePreset turntableCcw(bool* rotated = nullptr) = 0;
    virtual RotatePreset turntableCw(bool* rotated = nullptr) = 0;

signals:
    virtual void rotationChanged(double) = 0;

    // # General transforming
public:
    virtual QSize getSize() = 0;
    virtual QPoint getGlobalOffset() = 0;
    virtual QPointF getCenter() = 0;

    virtual void gripPivot(QPointF start, QPointF end) = 0;

    virtual QTransform getOntoCanvasTransform() = 0;
    virtual QTransform getFromCanvasTransform() = 0;

public slots:
    virtual void resetTransforms() = 0;
    virtual void fitWidth() = 0;
    virtual void fitCanvas() = 0;

    virtual void setSize(QSize size) = 0;
    virtual void setGlobalOffset(QPoint offset) = 0;

signals:
    virtual void transformsChanged() = 0;
};

DECL_INTERFACE_META_PROPERTIES(
    IViewPortPresenter,
    DECL_INTERFACE_PROPERTY(canNavigate)
    DECL_INTERFACE_PROPERTY(canZoomIn)
    DECL_INTERFACE_PROPERTY(canZoomOut)
    DECL_INTERFACE_PROPERTY(zoom)
);

DECL_MAKEABLE(IViewPortPresenter)
DECL_EXPECTS_INITIALIZE(IViewPortPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IViewPortPresenter)

#endif // IVIEWPORTPRESENTER_HPP
