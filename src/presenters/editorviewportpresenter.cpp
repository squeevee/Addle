#include "editorviewportpresenter.h"
#include "interfaces/presenters/icanvaspresenter.h"

#include "servicelocator.h"

#include <QPainter>

void EditorViewPortPresenter::initialize(IEditorPresenter* editorPresenter)
{
    _initHelper.initializeBegin();
    _editorPresenter = editorPresenter;
    _canvasPresenter = _editorPresenter->getCanvasPresenter();
    calculateTransforms();
    
    _initHelper.initializeEnd();
}

void EditorViewPortPresenter::setZoom(double zoom)
{
    _initHelper.assertInitialized();
    _zoom = zoom;
    emit zoomChanged(_zoom);
    calculateTransforms();
}

void EditorViewPortPresenter::setRotation(double rotation)
{
    _initHelper.assertInitialized();
    _rotation = rotation;
    emit rotationChanged(_rotation);
    calculateTransforms();
}


void EditorViewPortPresenter::setPosition(QPointF position)
{
    _initHelper.assertInitialized();
    _position = position;
    emit positionChanged(_position);
    calculateTransforms();
}

void EditorViewPortPresenter::setSize(QSize size)
{
    _initHelper.assertInitialized();
    _size = size;
    _center = QPointF(_size.width() / 2, _size.height() / 2);
    calculateTransforms();
}

void EditorViewPortPresenter::calculateTransforms()
{
    _ontoCanvas = QTransform();

    _ontoCanvas.translate(_position.x(), _position.y());
    _ontoCanvas.rotate(-_rotation);
    _ontoCanvas.scale(1 / _zoom, 1 / _zoom);
    _ontoCanvas.translate(- _center.x(), - _center.y());

    _fromCanvas = _ontoCanvas.inverted();

    emit transformsChanged();
}

QRect EditorViewPortPresenter::getCanvasBoundingRect()
{
    return _fromCanvas.mapRect(QRect(QPoint(), _canvasPresenter->getSize()));
}

QRect EditorViewPortPresenter::getScrollRect()
{
    _initHelper.assertInitialized();
    QRect canvasBoundingRect = getCanvasBoundingRect();
    int x_margin = _size.width();
    int y_margin = _size.height();

    QRect marginRect = QRect(
        canvasBoundingRect.x() - x_margin,
        canvasBoundingRect.y() - y_margin,
        canvasBoundingRect.width() + 2 * x_margin,
        canvasBoundingRect.height() + 2 * y_margin);

    QRect viewPortRect = QRect(QPoint(), _size);

    return marginRect.united(viewPortRect);
}


void EditorViewPortPresenter::resetView()
{
    centerView();
    setZoomPreset(DEFAULT_ZOOM_PRESET);
    emit needsUpdate();
}

void EditorViewPortPresenter::fitWidth()
{

}

void EditorViewPortPresenter::fitCanvas()
{

}

void EditorViewPortPresenter::centerView()
{
    QSize size = _canvasPresenter->getSize();
    setPosition(QPointF(size.width()/2.0, size.height()/2.0));
    emit needsUpdate();
}

void EditorViewPortPresenter::setZoomPreset(ZoomPreset preset)
{
    _initHelper.assertInitialized();
    _zoomPreset = preset;
    setZoom(ZoomRotateHelper::zoomValueOf(preset));
    emit needsUpdate();
}

IEditorViewPortPresenter::ZoomPreset EditorViewPortPresenter::zoomIn(bool* zoomed)
{
    _initHelper.assertInitialized();
    bool p_zoomed = false;

    if (_zoomPreset != ZoomPreset::nullzoom) {
        ZoomPreset newPreset = _zoomHelper.nextUp(_zoomPreset);

        if (newPreset != ZoomPreset::nullzoom)
        {
            setZoomPreset(newPreset);
            p_zoomed = true;
        }
    }
    else
    {
        double zoom = getZoom();
        ZoomPreset newPreset = _zoomHelper.nextUp(zoom);

        if (newPreset != _zoomPreset)
        {
            setZoomPreset(newPreset);
            p_zoomed = true;
        }
    }

    if (zoomed)
        *zoomed = p_zoomed;

    return _zoomPreset;
}

IEditorViewPortPresenter::ZoomPreset EditorViewPortPresenter::zoomOut(bool* zoomed)
{
    _initHelper.assertInitialized();
    bool p_zoomed = false;

    if (_zoomPreset != ZoomPreset::nullzoom) {
        ZoomPreset newPreset = _zoomHelper.nextDown(_zoomPreset);

        if (newPreset != ZoomPreset::nullzoom)
        {
            setZoomPreset(newPreset);
            p_zoomed = true;
        }
    }
    else
    {
        double zoom = getZoom();
        ZoomPreset newPreset = _zoomHelper.nextDown(zoom);

        if (newPreset != _zoomPreset)
        {
            setZoomPreset(newPreset);
            p_zoomed = true;
        }
    }

    if (zoomed)
        *zoomed = p_zoomed;

    return _zoomPreset;
}

IEditorViewPortPresenter::ZoomPreset EditorViewPortPresenter::zoomTo(double zoom, bool snapToPreset)
{
    _initHelper.assertInitialized();
    ZoomPreset newPreset = ZoomPreset::nullzoom;
    
    if (snapToPreset)
        newPreset = _zoomHelper.nearest(zoom, ZOOM_SNAP_THRESHOLD);

    if (newPreset != ZoomPreset::nullzoom)
    {
        if (newPreset != _zoomPreset)
        {
            setZoomPreset(newPreset);
        }
    }
    else
    {
        _zoomPreset = ZoomPreset::nullzoom;
        setZoom(zoom);
    }
    
    return _zoomPreset;
}

void EditorViewPortPresenter::gripMove(QPoint gripStart, QPoint gripEnd)
{
    _initHelper.assertInitialized();
    QTransform ontoCanvas = getOntoCanvasTransform();
    QPointF center = getCenter();
    QPointF newCenter = ontoCanvas.map(center - (gripEnd - gripStart));
    setPosition(newCenter);
}

void EditorViewPortPresenter::nudgeMove(int dx, int dy)
{
    _initHelper.assertInitialized();
    QTransform ontoCanvas = getOntoCanvasTransform();
    QPointF center = getCenter();
    QPointF newCenter = ontoCanvas.map(center + QPointF(dx, dy));
    setPosition(newCenter);
}

void EditorViewPortPresenter::gripZoom(QPoint gripStart, QPoint gripEnd)
{

}

void EditorViewPortPresenter::gripRotate(QPoint gripStart, QPoint gripEnd)
{

}

void EditorViewPortPresenter::twoGripTransform(QPoint grip1Start, QPoint grip1End, QPoint grip2Start, QPoint grip2End)
{

}

IEditorViewPortPresenter::RotatePreset EditorViewPortPresenter::turntableCcw(bool* rotated)
{
    return IEditorViewPortPresenter::RotatePreset::nullrotation;
}

IEditorViewPortPresenter::RotatePreset EditorViewPortPresenter::turntableCw(bool* rotated)
{
    return IEditorViewPortPresenter::RotatePreset::nullrotation;
}

void EditorViewPortPresenter::canvasBoundsChanged(QRect change)
{
    
}