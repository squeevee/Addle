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

#include "utils.hpp"
#include "utilities/initializehelper.hpp"
#include "utilities/presetmap.hpp"
#include "utilities/presenter/propertycache.hpp"

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
    ViewPortPresenter()
    {
        _canNavigateCache.calculateBy(&ViewPortPresenter::canNavigate_p, this);
        _canNavigateCache.onChange.bind(&ViewPortPresenter::canNavigateChanged, this);
        _canNavigateCache.onChange_Recalculate(_canZoomInCache);
        _canNavigateCache.onChange_Recalculate(_canZoomOutCache);

        _canZoomInCache.calculateBy(&ViewPortPresenter::canZoomIn_p, this);
        _canZoomInCache.onChange.bind(&ViewPortPresenter::canZoomInChanged, this);

        _canZoomOutCache.calculateBy(&ViewPortPresenter::canZoomOut_p, this);
        _canZoomOutCache.onChange.bind(&ViewPortPresenter::canZoomOutChanged, this);

        _transformCache.calculateBy(&ViewPortPresenter::transforms_p, this);
        _transformCache.onChange.bind(&ViewPortPresenter::transformsChanged, this);
        _transformCache.onChange_Recalculate(_scrollStateCache);
        
        _scrollStateCache.calculateBy(&ViewPortPresenter::scrollState_p, this);
        _scrollStateCache.onChange.bind(&ViewPortPresenter::scrollStateChanged, this);
    }
    virtual ~ViewPortPresenter() = default;

    void initialize(IMainEditorPresenter* mainEditorPresenter);

    IMainEditorPresenter* mainEditorPresenter() { ASSERT_INIT(); return _mainEditorPresenter; }

    bool canNavigate() const { ASSERT_INIT(); return _canNavigateCache.value(); }

    bool hasFocus() const { ASSERT_INIT(); return _hasFocus; }
    void setHasFocus(bool value);

signals:
    void canNavigateChanged(bool);
    void focusChanged(bool focus);

private:
    bool canNavigate_p();

    // # Scrolling / positioning
public:
    QPointF position() const { ASSERT_INIT(); return _position; }
    const IScrollState& scrollState() const { ASSERT_INIT(); return _scrollStateCache.value(); }

public slots:
    void setPosition(QPointF center);
    void scrollX(int x);
    void scrollY(int y);

    void gripPan(QPointF start, QPointF end);

signals:
    void positionChanged(QPointF position);
    void scrollStateChanged();

private:
    ScrollState scrollState_p();

public:
    bool canZoomIn() const { ASSERT_INIT(); return _canZoomInCache.value(); }
    bool canZoomOut() const { ASSERT_INIT(); return _canZoomOutCache.value(); }

    double zoom() const { ASSERT_INIT(); return _zoom; }
    void setZoom(double zoom);
    //void gripZoom(QPoint gripStart, QPoint gripEnd);
    
    ZoomPreset zoomPreset() const { ASSERT_INIT(); return _zoomPreset; }
    void setZoomPreset(ZoomPreset preset);
    double maxZoomPresetValue() const { ASSERT_INIT(); return _zoomPresetMap.valueOf(MAX_ZOOM_PRESET); }
    double minZoomPresetValue() const { ASSERT_INIT(); return _zoomPresetMap.valueOf(MIN_ZOOM_PRESET); }

    ZoomPreset zoomTo(double zoom, bool snapToPreset = true);

public slots:
    ZoomPreset zoomIn(bool* zoomed = nullptr);
    ZoomPreset zoomOut(bool* zoomed = nullptr);

signals:
    void zoomChanged(double zoom);
    void canZoomInChanged(bool canZoomIn);
    void canZoomOutChanged(bool canZoomOut);

private:
    double constrainZoom(double zoom);
    void setZoom_p(double zoom);

    bool canZoomIn_p() { return canNavigate() && _zoom <= maxZoomPresetValue() && _zoomPreset != MAX_ZOOM_PRESET; }
    bool canZoomOut_p() { return canNavigate() && _zoom >= minZoomPresetValue() && _zoomPreset != MIN_ZOOM_PRESET; }

    void propagateCanNavigate();

public:
    double rotation() const { ASSERT_INIT(); return _rotation; }
    void setRotation(double rotation);
    RotatePreset rotatePreset() const { ASSERT_INIT(); return _rotatePreset; }
    void setRotatePreset(RotatePreset preset);

public slots:
    RotatePreset turntableCcw(bool* rotated = nullptr);
    RotatePreset turntableCw(bool* rotated = nullptr);

signals:
    void rotationChanged(double rotation);

public:

    QSize size() const { ASSERT_INIT(); return _size; }

    void gripPivot(QPointF gripStart, QPointF gripEnd);

    QTransform ontoCanvasTransform() const { ASSERT_INIT(); return _transformCache.value().ontoCanvas; }
    QTransform fromCanvasTransform() const { ASSERT_INIT(); return _transformCache.value().fromCanvas; }

    QPoint globalOffset() const { ASSERT_INIT(); return _globalOffset; }
    virtual QPointF center() const { ASSERT_INIT(); return _center; }

public slots:
    void resetTransforms();
    void fitWidth();
    void fitCanvas();

    void setSize(QSize size);

    void setGlobalOffset(QPoint offset) { try { ASSERT_INIT(); _globalOffset = offset; } ADDLE_SLOT_CATCH }

signals:
    void transformsChanged();
    void ontoCanvasTransformChanged(QTransform);
    void fromCanvasTransformChanged(QTransform);

private slots:
    void onMainEditorPresenter_isEmptyChanged();
    
private:
    IMainEditorPresenter* _mainEditorPresenter;

    //The actual size of the viewport on the screen.
    QSize _size;

    //The position of the center of the viewport.
    QPointF _center;

    //The zoom factor of the viewport
    double _zoom = 1.0;

    ZoomPreset _zoomPreset;

    PropertyCache<bool> _canNavigateCache;
    PropertyCache<bool> _canZoomInCache;
    PropertyCache<bool> _canZoomOutCache;

    bool _hasFocus = false;

    //The rotation of the viewport in degrees
    double _rotation = 0.0;

    RotatePreset _rotatePreset;

    QPoint _globalOffset;

    //The center of the viewport in the canvas' coordinates.
    QPointF _position;

    PropertyCache<TransformPair> _transformCache;

    PropertyCache<ScrollState> _scrollStateCache;

    TransformPair transforms_p();

    void centerView();
    const double ZOOM_SNAP_THRESHOLD = 0.10;
    const int VIEW_MARGIN = 10;

    InitializeHelper _initHelper;
    
    static const PresetMap<RotatePreset, double> _rotatePresetMap;
    static const PresetMap<ZoomPreset, double> _zoomPresetMap;
};

} // namespace Addle
#endif // VIEWPORTPRESENTER_HPP