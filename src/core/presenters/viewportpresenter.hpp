#ifndef VIEWPORTPRESENTER_HPP
#define VIEWPORTPRESENTER_HPP

#include <QObject>
#include <QRect>

#include "common/interfaces/presenters/icanvaspresenter.hpp"
#include "common/interfaces/presenters/ieditorpresenter.hpp"
#include "common/interfaces/presenters/iviewportpresenter.hpp"
#include "common/interfaces/views/iviewport.hpp"
#include "./helpers/zoomrotatehelper.hpp"

#include "common/utilities/initializehelper.hpp"

//The ViewPort is the main presenter for the Canvas, representing a rectangular
//region onscreen that transforms and displays the Canvas' contents. The
//ViewPort can be zoomed, moved, and rotated relative to the Canvas.
class ViewPortPresenter: public QObject, public virtual IViewPortPresenter
{
    Q_OBJECT
public:

    ViewPortPresenter() : _initHelper(this)
    {
    }
    virtual ~ViewPortPresenter() = default;

    void initialize(IDocumentPresenter* documentPresenter, ICanvasPresenter* canvasPresenter);

    IViewPort* getViewPort();
    IDocumentPresenter* getDocumentPresenter() { _initHelper.assertInitialized(); return _documentPresenter; }
    ICanvasPresenter* getCanvasPresenter() { _initHelper.assertInitialized(); return _canvasPresenter; }

    // # Scrolling / positioning
public:
    QPointF getPosition() { _initHelper.assertInitialized(); return _position; }
    void setPosition(QPointF center);
    
    bool canScroll();
    bool canScrollInfinitely() { return false; } //TODO
    QRect getScrollRect();
    void scrollX(int x) { setScrollPosition(QPoint(x, 0)); }
    void scrollY(int y) { setScrollPosition(QPoint(0, y)); }
    void setScrollPosition(QPoint position);

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
    double getMaxZoomPresetValue() { _initHelper.assertInitialized(); return ZoomRotateHelper::zoomValueOf(MAX_ZOOM_PRESET); }
    double getMinZoomPresetValue() { _initHelper.assertInitialized(); return ZoomRotateHelper::zoomValueOf(MIN_ZOOM_PRESET); }

    ZoomPreset zoomTo(double zoom, bool snapToPreset = true);

public slots:

    ZoomPreset zoomIn(bool* zoomed = nullptr);
    ZoomPreset zoomOut(bool* zoomed = nullptr);

signals:
    void zoomChanged(double zoom);

public:
    double getRotation() { _initHelper.assertInitialized(); return _rotation; }
    void setRotation(double rotation);

public slots:
    RotatePreset turntableCcw(bool* rotated = nullptr);
    RotatePreset turntableCw(bool* rotated = nullptr);

signals:
    void rotationChanged(double rotation);

public:

    QSize getSize() { _initHelper.assertInitialized(); return _size; }
    virtual QPointF getCenter() { _initHelper.assertInitialized(); return _center; }

    QTransform getOntoCanvasTransform() { _initHelper.assertInitialized(); return _ontoCanvas; }
    QTransform getFromCanvasTransform() { _initHelper.assertInitialized(); return _fromCanvas; }

    QRectF getSceneRect();

//    void gripMove(QPoint gripStart, QPoint gripEnd);
//    void nudgeMove(int dx, int dy);
//    void gripRotate(QPoint gripStart, QPoint gripEnd);
//    void twoGripTransform(QPoint grip1Start, QPoint grip1End, QPoint grip2Start, QPoint grip2End);

public slots:
    void resetView();
    void fitWidth();
    void fitCanvas();

    void setSize(QSize size);

signals:
    void transformsChanged();
    
private:
    IViewPort* _viewPort = nullptr;
    IDocumentPresenter* _documentPresenter;
    ICanvasPresenter* _canvasPresenter;

    //The actual size of the viewport on the screen.
    QSize _size;

    //The position of the center of the viewport.
    QPointF _center;

    //The zoom factor of the viewport
    double _zoom = 1.0;



    //The rotation of the viewport in degrees
    double _rotation = 0.0;

    //The center of the viewport in the canvas' coordinates.
    QPointF _position;

    //The transform that maps viewport coordinates onto the canvas' coordinates.
    QTransform _ontoCanvas;

    //The transform that maps canvas coordinates onto the viewport's coordinates.
    QTransform _fromCanvas;

    void calculateTransforms();
    ZoomPreset _zoomPreset;
    ZoomRotateHelper _zoomHelper;

    void centerView();
    const double ZOOM_SNAP_THRESHOLD = 0.10;
    const int VIEW_MARGIN = 10;

    InitializeHelper<ViewPortPresenter> _initHelper;
};

#endif // VIEWPORTPRESENTER_HPP