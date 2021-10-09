/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IVIEWPORTPRESENTER_HPP
#define IVIEWPORTPRESENTER_HPP

#include <QPointF>
#include <QTransform>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"
#include "interfaces/metaobjectinfo.hpp"

namespace Addle {

class ICanvasPresenter;
class IScrollState; // TODO: member interface?

/**
 * @class IViewPortPresenter
 * The ViewPort adapts the canvas to the screen by transforming and truncating
 * the canvas according to the state (e.g., position, zoom, rotation, and size)
 * of the ViewPort. This presenter owns and manages the state of the ViewPort
 * and provides useful ways to modify it.
 * 
 * Note that the actual transformations are handled by GUI code.
 */
class IViewPortPresenter : public virtual IAmQObject
{
public:
    INTERFACE_META(IViewPortPresenter)

    enum ZoomPreset {
        // TODO: While "round numbers" look good when written out, at least a
        // few more steps should be chosen on a logarithmic scale (which 
        // "feel good" when zooming).

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

        // dynamic zoom presets
        //Fit = 1024,
        //FitToWidth = 1025
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

    class IScrollState
    {
    public:
        virtual ~IScrollState() = default;

        virtual int pageWidth() const = 0;
        virtual int pageHeight() const = 0;

        virtual int horizontalValue() const = 0;
        virtual int verticalValue() const = 0;

        virtual int horizontalMinimum() const = 0;
        virtual int verticalMinimum() const = 0;

        virtual int horizontalMaximum() const = 0;
        virtual int verticalMaximum() const = 0;
    };

    virtual ~IViewPortPresenter() = default;
    
    virtual QSharedPointer<ICanvasPresenter> canvas() const = 0;

public:
    virtual bool canNavigate() const = 0;

    virtual bool hasFocus() const = 0;
    virtual void setHasFocus(bool value) = 0;

signals:
    virtual void canNavigateChanged(bool) = 0;
    virtual void focusChanged(bool focus) = 0;

    // # Scrolling / positioning
public:
    virtual QPointF position() const = 0;
    virtual const IScrollState& scrollState() const = 0;

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
    virtual bool canZoomIn() const = 0;
    virtual bool canZoomOut() const = 0;

    virtual double zoom() const = 0;
    virtual void setZoom(double zoom) = 0;
    //virtual void gripZoom(QPoint gripStart, QPoint gripEnd) = 0;

    virtual ZoomPreset zoomPreset() const = 0;
    virtual void setZoomPreset(ZoomPreset preset) = 0;

    virtual double maxZoomPresetValue() const = 0;
    virtual double minZoomPresetValue() const = 0;
    
    virtual ZoomPreset zoomTo(double zoom, bool snapToPreset = true) = 0;

public slots:
    virtual ZoomPreset zoomIn(bool* zoomed = nullptr) = 0;
    virtual ZoomPreset zoomOut(bool* zoomed = nullptr) = 0;

signals:
    virtual void zoomChanged(double) = 0;
    virtual void canZoomInChanged(bool canZoomIn) = 0;
    virtual void canZoomOutChanged(bool canZoomOut) = 0;

    // # Rotation
public:
    virtual double rotation() const = 0;
    virtual void setRotation(double) = 0;
    
    virtual RotatePreset rotatePreset() const = 0;
    virtual void setRotatePreset(RotatePreset preset) = 0;

public slots:
    virtual RotatePreset turntableCcw(bool* rotated = nullptr) = 0;
    virtual RotatePreset turntableCw(bool* rotated = nullptr) = 0;

signals:
    virtual void rotationChanged(double) = 0;

    // # General transforming
public:
    virtual QSize size() const = 0;

    virtual void gripPivot(QPointF start, QPointF end) = 0;

    virtual QTransform ontoCanvasTransform() const = 0;
    virtual QTransform fromCanvasTransform() const = 0;

    virtual QPoint globalOffset() const = 0;
    virtual QPointF center() const = 0;

public slots:
    virtual void resetTransforms() = 0;
    virtual void fitWidth() = 0;
    virtual void fitCanvas() = 0;

    virtual void setSize(QSize size) = 0;
    virtual void setGlobalOffset(QPoint offset) = 0;

signals:
    virtual void transformsChanged() = 0;
    virtual void ontoCanvasTransformChanged(QTransform) = 0;
    virtual void fromCanvasTransformChanged(QTransform) = 0;
};

DECL_INTERFACE_META_PROPERTIES(
    IViewPortPresenter,
    DECL_INTERFACE_PROPERTY(canNavigate) // bool, read-only
    DECL_INTERFACE_PROPERTY(canZoomIn)   // bool, read-only
    DECL_INTERFACE_PROPERTY(canZoomOut)  // bool, read-only
    DECL_INTERFACE_PROPERTY(zoom)        // double
);

namespace aux_IViewPortPresenter {
    ADDLE_FACTORY_PARAMETER_NAME( canvas )
}

ADDLE_DECL_MAKEABLE(IViewPortPresenter)
ADDLE_DECL_FACTORY_PARAMETERS(
    IViewPortPresenter,
    (required (canvas, (QSharedPointer<ICanvasPresenter>)))
)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IViewPortPresenter, "org.addle.IViewportPresenter");

#endif // IVIEWPORTPRESENTER_HPP
