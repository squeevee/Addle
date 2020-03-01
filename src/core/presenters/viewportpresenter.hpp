#ifndef VIEWPORTPRESENTER_HPP
#define VIEWPORTPRESENTER_HPP

#include <QObject>
#include <QRect>

#include "interfaces/presenters/ieditorpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/views/iviewport.hpp"
#include "utilities/presethelper.hpp"

#include "utilities/initializehelper.hpp"

//The ViewPort is the main presenter for the Canvas, representing a rectangular
//region onscreen that transforms and displays the Canvas' contents. The
//ViewPort can be zoomed, moved, and rotated relative to the Canvas.
class ViewPortPresenter: public QObject, public virtual IViewPortPresenter
{
    Q_OBJECT
public:

    ViewPortPresenter()
        : _initHelper(this)
    {
    }
    virtual ~ViewPortPresenter() = default;

    void initialize(IDocumentPresenter* documentPresenter);

    IViewPort* getViewPort();
    IDocumentPresenter* getDocumentPresenter() { _initHelper.assertInitialized(); return _documentPresenter; }

    // # Scrolling / positioning
public:
    QPointF getPosition() { _initHelper.assertInitialized(); return _position; }
    void setPosition(QPointF center);
    
    bool canScroll();
    QRect getScrollRect();
    void scrollX(int x) { setScrollPosition(QPoint(x, 0)); }
    void scrollY(int y) { setScrollPosition(QPoint(0, y)); }
    void setScrollPosition(QPoint position);

    void gripPan(QPointF start, QPointF end);

private:
    QPoint getScrollPosition(QPointF position);
    QPoint projectOntoScrollRect(QPoint scrollPosition);
signals:
    void positionChanged(QPointF position);
    void scrollStateChanged();

public:
    bool canZoom();
    bool canZoomIn() { _initHelper.assertInitialized(); return canZoom() && _zoomPreset != MAX_ZOOM_PRESET; }
    bool canZoomOut() { _initHelper.assertInitialized(); return canZoom() && _zoomPreset != MIN_ZOOM_PRESET; }

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

private:
    double constrainZoom(double zoom);

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

    QSize getViewPortSize() { _initHelper.assertInitialized(); return _size; }
    virtual QPointF getCenter() { _initHelper.assertInitialized(); return _center; }

    void gripPivot(QPointF gripStart, QPointF gripEnd);

    QTransform getOntoCanvasTransform() { _initHelper.assertInitialized(); return _ontoCanvas; }
    QTransform getFromCanvasTransform() { _initHelper.assertInitialized(); return _fromCanvas; }

//    void gripMove(QPoint gripStart, QPoint gripEnd);
//    void nudgeMove(int dx, int dy);
//    void gripRotate(QPoint gripStart, QPoint gripEnd);
//    void twoGripTransform(QPoint grip1Start, QPoint grip1End, QPoint grip2Start, QPoint grip2End);

    QPoint getGlobalOffset() { _initHelper.assertInitialized(); return _globalOffset; }

public slots:
    void resetView();
    void fitWidth();
    void fitCanvas();

    void setViewPortSize(QSize size);

    void setGlobalOffset(QPoint offset) { _initHelper.assertInitialized(); _globalOffset = offset; }

signals:
    void transformsChanged();
    
private:
    IViewPort* _viewPort = nullptr;
    IDocumentPresenter* _documentPresenter;

    //The actual size of the viewport on the screen.
    QSize _size;

    //The position of the center of the viewport.
    QPointF _center;

    //The zoom factor of the viewport
    double _zoom = 1.0;

    ZoomPreset _zoomPreset;

    //The rotation of the viewport in degrees
    double _rotation = 0.0;

    RotatePreset _rotatePreset;

    QPoint _globalOffset;

    //The center of the viewport in the canvas' coordinates.
    QPointF _position;

    //The transform that maps viewport coordinates onto the canvas' coordinates.
    QTransform _ontoCanvas;

    //The transform that maps canvas coordinates onto the viewport's coordinates.
    QTransform _fromCanvas;

    void calculateTransforms();

    void centerView();
    const double ZOOM_SNAP_THRESHOLD = 0.10;
    const int VIEW_MARGIN = 10;

    InitializeHelper<ViewPortPresenter> _initHelper;
    
    static const PresetHelper<RotatePreset, double> _rotatePresetHelper;
    static const PresetHelper<ZoomPreset, double> _zoomPresetHelper;
};

#endif // VIEWPORTPRESENTER_HPP