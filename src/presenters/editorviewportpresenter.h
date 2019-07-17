#ifndef EDITORVIEWPORTPRESENTER_H
#define EDITORVIEWPORTPRESENTER_H

#include <QObject>
#include <QRect>

#include "interfaces/presenters/icanvaspresenter.h"
#include "interfaces/presenters/ieditorpresenter.h"
#include "interfaces/presenters/ieditorviewportpresenter.h"
#include "./helpers/zoomrotatehelper.h"

#include "utilities/initializehelper.h"

//The ViewPort is the main presenter for the Canvas, representing a rectangular
//region onscreen that transforms and displays the Canvas' contents. The
//ViewPort can be zoomed, moved, and rotated relative to the Canvas.
class EditorViewPortPresenter: public QObject, public IEditorViewPortPresenter
{
    Q_OBJECT
public:

    EditorViewPortPresenter() : _initHelper(this) {}
    virtual ~EditorViewPortPresenter() = default;

    void initialize(IEditorPresenter* editorPresenter);
    IEditorPresenter* getEditorPresenter() { _initHelper.assertInitialized(); return _editorPresenter; }
    ICanvasPresenter* getCanvasPresenter() { _initHelper.assertInitialized(); return _canvasPresenter; }

    //The "zoom level" of the ViewPort, i.e., the factor by which the canvas
    //is scaled onto the view.
    double getZoom() { _initHelper.assertInitialized(); return _zoom; }
    void setZoom(double zoom);


    //The rotation of the ViewPort in degrees relative to the Canvas. Zero is
    //upright and positive degrees correspond to counter-clockwise rotation.
    double getRotation() { _initHelper.assertInitialized(); return _rotation; }
    void setRotation(double rotation);

    //The position of the center of the ViewPort relative to the top-left corner
    //of the Canvas boundary, in the Canvas' coordinates.
    QPointF getPosition() { return _position; }
    void setPosition(QPointF center);

    QTransform getOntoCanvasTransform() { _initHelper.assertInitialized(); return _ontoCanvas; }
    QTransform getFromCanvasTransform() { _initHelper.assertInitialized(); return _fromCanvas; }

    QSize getSize() { _initHelper.assertInitialized(); return _size; }
    void setSize(QSize size);
    virtual QPointF getCenter() { _initHelper.assertInitialized(); return _center; }

    QRect getCanvasBoundingRect();
    QRect getScrollRect();

    ZoomPreset getZoomPreset() { _initHelper.assertInitialized(); return _zoomPreset; }
    void setZoomPreset(ZoomPreset preset);
    bool canZoomIn() { _initHelper.assertInitialized(); return _zoomPreset != MAX_ZOOM_PRESET; }
    bool canZoomOut() { _initHelper.assertInitialized(); return _zoomPreset != MIN_ZOOM_PRESET; }
    double getMaxZoomPresetValue() { _initHelper.assertInitialized(); return ZoomRotateHelper::zoomValueOf(MAX_ZOOM_PRESET); }
    double getMinZoomPresetValue() { _initHelper.assertInitialized(); return ZoomRotateHelper::zoomValueOf(MIN_ZOOM_PRESET); }
    ZoomPreset zoomTo(double zoom, bool snapToPreset = true);

    void gripMove(QPoint gripStart, QPoint gripEnd);
    void nudgeMove(int dx, int dy);
    void gripZoom(QPoint gripStart, QPoint gripEnd);
    void gripRotate(QPoint gripStart, QPoint gripEnd);
    void twoGripTransform(QPoint grip1Start, QPoint grip1End, QPoint grip2Start, QPoint grip2End);

signals:

    void needsUpdate();

    void positionChanged(QPointF position);
    void rotationChanged(double rotation);
    void zoomChanged(double zoom);
    void transformsChanged();

public slots:

    void canvasBoundsChanged(QRect change);

    void resetView();
    void fitWidth();
    void fitCanvas();

    ZoomPreset zoomIn(bool* zoomed = nullptr);
    ZoomPreset zoomOut(bool* zoomed = nullptr);

    RotatePreset turntableCcw(bool* rotated = nullptr);
    RotatePreset turntableCw(bool* rotated = nullptr);
private:

    IEditorPresenter* _editorPresenter;
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

protected:
    InitializeHelper _initHelper;
};

#endif //EDITORVIEWPORTPRESENTER_H