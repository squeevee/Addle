#ifndef VIEWPORTPRESENTER_HPP
#define VIEWPORTPRESENTER_HPP

#include <QObject>
#include <QRect>

#include "interfaces/presenters/iviewportpresenter.hpp"
#include "utilities/presethelper.hpp"

#include "utilities/initializehelper.hpp"

#include "utilities/presenter/propertycache.hpp"
#include "core/compat.hpp"

//The ViewPort is the main presenter for the Canvas, representing a rectangular
//region onscreen that transforms and displays the Canvas' contents. The
//ViewPort can be zoomed, moved, and rotated relative to the Canvas.
class ADDLE_CORE_EXPORT ViewPortPresenter: public QObject, public virtual IViewPortPresenter
{
    Q_OBJECT
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
        READ getZoom 
        WRITE setZoom
        NOTIFY zoomChanged
    )

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

    class ScrollState : public IScrollState
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

        int getPageWidth() const { return _width; }
        int getPageHeight() const { return _height; }

        int getHorizontalValue() const { return _x; }
        int getVerticalValue() const { return _y; }

        int getHorizontalMinimum() const { return qMin(_x, -_width); }
        int getVerticalMinimum() const { return qMin(_y, -_height); }

        int getHorizontalMaximum() const { return qMax(_rangeWidth + 1, _x); }
        int getVerticalMaximum() const { return qMax(_rangeHeight + 1, _y); }

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
        : _initHelper(this)
    {
        _canNavigateCache.calculateBy(&ViewPortPresenter::canNavigate_p, this);
        _canNavigateCache.onChange(&ViewPortPresenter::canNavigateChanged, this);
        _canNavigateCache.onChange_Recalculate(_canZoomInCache);
        _canNavigateCache.onChange_Recalculate(_canZoomOutCache);

        _canZoomInCache.calculateBy(&ViewPortPresenter::canZoomIn_p, this);
        _canZoomInCache.onChange(&ViewPortPresenter::canZoomInChanged, this);

        _canZoomOutCache.calculateBy(&ViewPortPresenter::canZoomOut_p, this);
        _canZoomOutCache.onChange(&ViewPortPresenter::canZoomOutChanged, this);

        _transformCache.calculateBy(&ViewPortPresenter::getTransforms_p, this);
        _transformCache.onChange(&ViewPortPresenter::transformsChanged, this);
        _transformCache.onChange_Recalculate(_scrollStateCache);
        
        _scrollStateCache.calculateBy(&ViewPortPresenter::getScrollState_p, this);
        _scrollStateCache.onChange(&ViewPortPresenter::scrollStateChanged, this);
    }
    virtual ~ViewPortPresenter() = default;

    void initialize(IMainEditorPresenter* mainEditorPresenter);

    IMainEditorPresenter* getMainEditorPresenter() { _initHelper.check(); return _mainEditorPresenter; }

public:
    bool canNavigate() { _initHelper.check(); return _canNavigateCache.getValue(); }

signals:
    void canNavigateChanged(bool);

private:
    bool canNavigate_p();

    // # Scrolling / positioning
public:
    QPointF getPosition() { _initHelper.check(); return _position; }
    const IScrollState& getScrollState() { _initHelper.check(); return _scrollStateCache.getValue(); }

public slots:
    void setPosition(QPointF center);
    void scrollX(int x);
    void scrollY(int y);

    void gripPan(QPointF start, QPointF end);

signals:
    void positionChanged(QPointF position);
    void scrollStateChanged();

private:
    ScrollState getScrollState_p();

public:
    bool canZoomIn() { _initHelper.check(); return _canZoomInCache.getValue(); }
    bool canZoomOut() { _initHelper.check(); return _canZoomOutCache.getValue(); }

    double getZoom() { _initHelper.check(); return _zoom; }
    void setZoom(double zoom);
    //void gripZoom(QPoint gripStart, QPoint gripEnd);
    
    ZoomPreset getZoomPreset() { _initHelper.check(); return _zoomPreset; }
    void setZoomPreset(ZoomPreset preset);
    double getMaxZoomPresetValue() { _initHelper.check(); return _zoomPresetHelper.valueOf(MAX_ZOOM_PRESET); }
    double getMinZoomPresetValue() { _initHelper.check(); return _zoomPresetHelper.valueOf(MIN_ZOOM_PRESET); }

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

    bool canZoomIn_p() { return canNavigate() && _zoom <= getMaxZoomPresetValue() && _zoomPreset != MAX_ZOOM_PRESET; }
    bool canZoomOut_p() { return canNavigate() && _zoom >= getMinZoomPresetValue() && _zoomPreset != MIN_ZOOM_PRESET; }

    void propagateCanNavigate();

public:
    double getRotation() { _initHelper.check(); return _rotation; }
    void setRotation(double rotation);
    RotatePreset getRotatePreset() { _initHelper.check(); return _rotatePreset; }
    void setRotatePreset(RotatePreset preset);

public slots:
    RotatePreset turntableCcw(bool* rotated = nullptr);
    RotatePreset turntableCw(bool* rotated = nullptr);

signals:
    void rotationChanged(double rotation);

public:

    QSize getSize() { _initHelper.check(); return _size; }
    virtual QPointF getCenter() { _initHelper.check(); return _center; }

    void gripPivot(QPointF gripStart, QPointF gripEnd);

    QTransform getOntoCanvasTransform() { _initHelper.check(); return _transformCache.getValue().ontoCanvas; }
    QTransform getFromCanvasTransform() { _initHelper.check(); return _transformCache.getValue().fromCanvas; }

//    void gripMove(QPoint gripStart, QPoint gripEnd);
//    void nudgeMove(int dx, int dy);
//    void gripRotate(QPoint gripStart, QPoint gripEnd);
//    void twoGripTransform(QPoint grip1Start, QPoint grip1End, QPoint grip2Start, QPoint grip2End);

    QPoint getGlobalOffset() { _initHelper.check(); return _globalOffset; }

public slots:
    void resetTransforms();
    void fitWidth();
    void fitCanvas();

    void setSize(QSize size);

    void setGlobalOffset(QPoint offset) { _initHelper.check(); _globalOffset = offset; }

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

    //The rotation of the viewport in degrees
    double _rotation = 0.0;

    RotatePreset _rotatePreset;

    QPoint _globalOffset;

    //The center of the viewport in the canvas' coordinates.
    QPointF _position;

    PropertyCache<TransformPair> _transformCache;

    PropertyCache<ScrollState> _scrollStateCache;

    TransformPair getTransforms_p();

    void centerView();
    const double ZOOM_SNAP_THRESHOLD = 0.10;
    const int VIEW_MARGIN = 10;

    InitializeHelper<ViewPortPresenter> _initHelper;
    
    static const PresetHelper<RotatePreset, double> _rotatePresetHelper;
    static const PresetHelper<ZoomPreset, double> _zoomPresetHelper;
};

#endif // VIEWPORTPRESENTER_HPP