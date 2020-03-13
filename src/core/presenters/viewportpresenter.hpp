#ifndef VIEWPORTPRESENTER_HPP
#define VIEWPORTPRESENTER_HPP

#include <QObject>
#include <QRect>

#include "interfaces/presenters/ieditorpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"
#include "utilities/presethelper.hpp"

#include "utilities/initializehelper.hpp"

#include "utilities/presenter/propertycache.hpp"

//The ViewPort is the main presenter for the Canvas, representing a rectangular
//region onscreen that transforms and displays the Canvas' contents. The
//ViewPort can be zoomed, moved, and rotated relative to the Canvas.
class ViewPortPresenter: public QObject, public virtual IViewPortPresenter
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
        : _canNavigateCache(*this, &ViewPortPresenter::canNavigate_p, &ViewPortPresenter::canNavigateChanged),
        _canZoomInCache(*this, &ViewPortPresenter::canZoomIn_p, &ViewPortPresenter::canZoomInChanged),
        _canZoomOutCache(*this, &ViewPortPresenter::canZoomOut_p, &ViewPortPresenter::canZoomOutChanged),
        _transformCache(*this, &ViewPortPresenter::getTransforms_p),
        _scrollStateCache(*this, &ViewPortPresenter::getScrollState_p),
        _initHelper(this)
    {
        _canNavigateCache.onChange({
            [this](){ _canZoomInCache.recalculate(); }, //_BIND(_canZoomInCache, recalculate),
            [this](){ _canZoomOutCache.recalculate(); }//_BIND(_canZoomOutCache, recalculate)
        });
        _transformCache.onChange({
            [this](){ _scrollStateCache.recalculate(); }, //_BIND(_scrollStateCache, recalculate),
            [this](){ emit transformsChanged(); }
        });
        _scrollStateCache.onChange({
            [this](){ emit scrollStateChanged(); }
        });
    }
    virtual ~ViewPortPresenter() = default;

    void initialize(IDocumentPresenter* documentPresenter);

    IDocumentPresenter* getDocumentPresenter() { _initHelper.assertInitialized(); return _documentPresenter; }

public:
    bool canNavigate() { _initHelper.assertInitialized(); return _canNavigateCache.getValue(); }

signals:
    void canNavigateChanged(bool);

private:
    bool canNavigate_p();

    // # Scrolling / positioning
public:
    QPointF getPosition() { _initHelper.assertInitialized(); return _position; }
    const IScrollState& getScrollState() { _initHelper.assertInitialized(); return _scrollStateCache.getValue(); }

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
    bool canZoomIn() { _initHelper.assertInitialized(); return _canZoomInCache.getValue(); }
    bool canZoomOut() { _initHelper.assertInitialized(); return _canZoomOutCache.getValue(); }

    double getZoom() { _initHelper.assertInitialized(); return _zoom; }
    void setZoom(double zoom);
    //void gripZoom(QPoint gripStart, QPoint gripEnd);
    
    ZoomPreset getZoomPreset() { _initHelper.assertInitialized(); return _zoomPreset; }
    void setZoomPreset(ZoomPreset preset);
    double getMaxZoomPresetValue() { _initHelper.assertInitialized(); return _zoomPresetHelper.valueOf(MAX_ZOOM_PRESET); }
    double getMinZoomPresetValue() { _initHelper.assertInitialized(); return _zoomPresetHelper.valueOf(MIN_ZOOM_PRESET); }

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
    double getRotation() { _initHelper.assertInitialized(); return _rotation; }
    void setRotation(double rotation);
    RotatePreset getRotatePreset() { _initHelper.assertInitialized(); return _rotatePreset; }
    void setRotatePreset(RotatePreset preset);

public slots:
    RotatePreset turntableCcw(bool* rotated = nullptr);
    RotatePreset turntableCw(bool* rotated = nullptr);

signals:
    void rotationChanged(double rotation);

public:

    QSize getSize() { _initHelper.assertInitialized(); return _size; }
    virtual QPointF getCenter() { _initHelper.assertInitialized(); return _center; }

    void gripPivot(QPointF gripStart, QPointF gripEnd);

    QTransform getOntoCanvasTransform() { _initHelper.assertInitialized(); return _transformCache.getValue().ontoCanvas; }
    QTransform getFromCanvasTransform() { _initHelper.assertInitialized(); return _transformCache.getValue().fromCanvas; }

//    void gripMove(QPoint gripStart, QPoint gripEnd);
//    void nudgeMove(int dx, int dy);
//    void gripRotate(QPoint gripStart, QPoint gripEnd);
//    void twoGripTransform(QPoint grip1Start, QPoint grip1End, QPoint grip2Start, QPoint grip2End);

    QPoint getGlobalOffset() { _initHelper.assertInitialized(); return _globalOffset; }

public slots:
    void resetView();
    void fitWidth();
    void fitCanvas();

    void setSize(QSize size);

    void setGlobalOffset(QPoint offset) { _initHelper.assertInitialized(); _globalOffset = offset; }

signals:
    void transformsChanged();

private slots:
    void onDocumentChanged();
    
private:
    IDocumentPresenter* _documentPresenter;

    //The actual size of the viewport on the screen.
    QSize _size;

    //The position of the center of the viewport.
    QPointF _center;

    //The zoom factor of the viewport
    double _zoom = 1.0;

    ZoomPreset _zoomPreset;

    PropertyCache<ViewPortPresenter, bool> _canNavigateCache;
    PropertyCache<ViewPortPresenter, bool> _canZoomInCache;
    PropertyCache<ViewPortPresenter, bool> _canZoomOutCache;

    //The rotation of the viewport in degrees
    double _rotation = 0.0;

    RotatePreset _rotatePreset;

    QPoint _globalOffset;

    //The center of the viewport in the canvas' coordinates.
    QPointF _position;

    PropertyCache<ViewPortPresenter, TransformPair> _transformCache;

    PropertyCache<ViewPortPresenter, ScrollState> _scrollStateCache;

    TransformPair getTransforms_p();

    void centerView();
    const double ZOOM_SNAP_THRESHOLD = 0.10;
    const int VIEW_MARGIN = 10;

    InitializeHelper<ViewPortPresenter> _initHelper;
    
    static const PresetHelper<RotatePreset, double> _rotatePresetHelper;
    static const PresetHelper<ZoomPreset, double> _zoomPresetHelper;
};

#endif // VIEWPORTPRESENTER_HPP