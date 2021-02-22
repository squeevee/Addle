/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef VIEWPORTPRESENTER_HPP
#define VIEWPORTPRESENTER_HPP

#include "compat.hpp"
#include <QObject>
#include <QRect>

#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"

#include "utils.hpp"
#include "utilities/presetmap.hpp"
#include "utilities/lazyvalue.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT ViewPortPresenter : public QObject, public IViewPortPresenter
{
    Q_OBJECT
    Q_INTERFACES(Addle::IViewPortPresenter)
    Q_PROPERTY(
        bool canNavigate
        READ canNavigate
        NOTIFY canNavigateChanged
    )
    Q_PROPERTY(
        bool canZoomIn
        READ canZoomIn
        NOTIFY canZoomInChanged
    )
    Q_PROPERTY(
        bool canZoomOut
        READ canZoomOut
        NOTIFY canZoomOutChanged
    )
    Q_PROPERTY(
        double zoom
        READ zoom 
        WRITE setZoom
        NOTIFY zoomChanged
    )
    IAMQOBJECT_IMPL

    struct TransformPair
    {
        inline bool operator!=(const TransformPair& other)
        {
            return ontoCanvas != other.ontoCanvas
                && fromCanvas != other.fromCanvas;
        }
        QTransform ontoCanvas;
        QTransform fromCanvas;
    };

    class ScrollState : public IViewPortPresenter::IScrollState
    {
        int _rangeWidth = -1;
        int _rangeHeight = -1;

        int _width = -1;
        int _height = -1;

        int _x = 0;
        int _y = 0;

    public:
        ScrollState() = default;

        ScrollState(QSize canvasRect, QRect current)
            : ScrollState()
        {
            _rangeWidth = canvasRect.width();
            _rangeHeight = canvasRect.height();

            _width = current.width();
            _height = current.height();

            _x = current.x();
            _y = current.y();
        }

        virtual ~ScrollState() = default;

        int pageWidth() const { return _width; }
        int pageHeight() const { return _height; }

        int horizontalValue() const { return _x; }
        int verticalValue() const { return _y; }

        int horizontalMinimum() const { return qMin(_x, -_width); }
        int verticalMinimum() const { return qMin(_y, -_height); }

        int horizontalMaximum() const { return qMax(_rangeWidth + 1, _x); }
        int verticalMaximum() const { return qMax(_rangeHeight + 1, _y); }

        inline bool operator!=(const ScrollState& other) const
        {
            return _rangeWidth != other._rangeWidth
                || _rangeHeight != other._rangeHeight
                || _width != other._width
                || _height != other._height
                || _x != other._x
                || _y != other._y;
        }

        friend class ViewPortPresenter;
    };

public:
    ViewPortPresenter(QSharedPointer<ICanvasPresenter> canvas);
    virtual ~ViewPortPresenter() = default;

    QSharedPointer<ICanvasPresenter> canvas() const override { return _canvas; }

    bool canNavigate() const override { return _canNavigateCache.value(); }

    bool hasFocus() const override { return _hasFocus; }
    void setHasFocus(bool value) override;

signals:
    void canNavigateChanged(bool) override;
    void focusChanged(bool focus) override;

private:
    bool canNavigate_p();

    // # Scrolling / positioning
public:
    QPointF position() const override { return _position; }
    const IScrollState& scrollState() const override { return _scrollStateCache.value(); }

public slots:
    void setPosition(QPointF center) override;
    void scrollX(int x) override;
    void scrollY(int y) override;

    void gripPan(QPointF start, QPointF end) override;

signals:
    void positionChanged(QPointF position) override;
    void scrollStateChanged() override;

private:
    ScrollState scrollState_p();

public:
    bool canZoomIn() const override { return _canZoomInCache.value(); }
    bool canZoomOut() const override { return _canZoomOutCache.value(); }

    double zoom() const override { return _zoom; }
    void setZoom(double zoom) override;
    //void gripZoom(QPoint gripStart, QPoint gripEnd);
    
    ZoomPreset zoomPreset() const override { return _zoomPreset; }
    void setZoomPreset(ZoomPreset preset) override;
    double maxZoomPresetValue() const override { return _zoomPresetMap.valueOf(MAX_ZOOM_PRESET); }
    double minZoomPresetValue() const override { return _zoomPresetMap.valueOf(MIN_ZOOM_PRESET); }

    ZoomPreset zoomTo(double zoom, bool snapToPreset = true) override;

public slots:
    ZoomPreset zoomIn(bool* zoomed = nullptr) override;
    ZoomPreset zoomOut(bool* zoomed = nullptr) override;

signals:
    void zoomChanged(double zoom) override;
    void canZoomInChanged(bool canZoomIn) override;
    void canZoomOutChanged(bool canZoomOut) override;

private:
    double constrainZoom(double zoom);
    void setZoom_p(double zoom);

    bool canZoomIn_p() { return canNavigate() && _zoom <= maxZoomPresetValue() && _zoomPreset != MAX_ZOOM_PRESET; }
    bool canZoomOut_p() { return canNavigate() && _zoom >= minZoomPresetValue() && _zoomPreset != MIN_ZOOM_PRESET; }

    void propagateCanNavigate();

public:
    double rotation() const override { return _rotation; }
    void setRotation(double rotation) override;
    RotatePreset rotatePreset() const override { return _rotatePreset; }
    void setRotatePreset(RotatePreset preset) override;

public slots:
    RotatePreset turntableCcw(bool* rotated = nullptr) override;
    RotatePreset turntableCw(bool* rotated = nullptr) override;

signals:
    void rotationChanged(double rotation) override;

public:

    QSize size() const override { return _size; }

    void gripPivot(QPointF gripStart, QPointF gripEnd) override;

    QTransform ontoCanvasTransform() const override { return _transformCache.value().ontoCanvas; }
    QTransform fromCanvasTransform() const override { return _transformCache.value().fromCanvas; }

    QPoint globalOffset() const override { return _globalOffset; }
    virtual QPointF center() const override { return _center; }

public slots:
    void resetTransforms() override;
    void fitWidth() override;
    void fitCanvas() override;

    void setSize(QSize size) override;

    void setGlobalOffset(QPoint offset) override { try { _globalOffset = offset; } ADDLE_SLOT_CATCH }

signals:
    void transformsChanged() override;
    void ontoCanvasTransformChanged(QTransform) override;
    void fromCanvasTransformChanged(QTransform) override;

private slots:
    void onMainEditorPresenter_isEmptyChanged();
    
private:
    QSharedPointer<ICanvasPresenter> _canvas;

    //The actual size of the viewport on the screen.
    QSize _size;

    //The position of the center of the viewport.
    QPointF _center;

    //The zoom factor of the viewport
    double _zoom = 1.0;

    ZoomPreset _zoomPreset;

    LazyProperty<bool> _canNavigateCache;
    LazyProperty<bool> _canZoomInCache;
    LazyProperty<bool> _canZoomOutCache;

    bool _hasFocus = false;

    //The rotation of the viewport in degrees
    double _rotation = 0.0;

    RotatePreset _rotatePreset;

    QPoint _globalOffset;

    //The center of the viewport in the canvas' coordinates.
    QPointF _position;

    LazyProperty<TransformPair> _transformCache;

    LazyProperty<ScrollState> _scrollStateCache;

    TransformPair transforms_p();

    void centerView();
    const double ZOOM_SNAP_THRESHOLD = 0.10;
    const int VIEW_MARGIN = 10;
    
    static const PresetMap<RotatePreset, double> _rotatePresetMap;
    static const PresetMap<ZoomPreset, double> _zoomPresetMap;
};

} // namespace Addle
#endif // VIEWPORTPRESENTER_HPP
