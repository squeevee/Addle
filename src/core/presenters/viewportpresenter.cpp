#include "viewportpresenter.hpp"

#include "servicelocator.hpp"

#include <QtDebug>

#include <QLineF>
#include <QPainter>

void ViewPortPresenter::initialize(IDocumentPresenter* documentPresenter)
{
    _initHelper.initializeBegin();
    _documentPresenter = documentPresenter;

    calculateTransforms();

    _initHelper.initializeEnd();
}

IViewPort* ViewPortPresenter::getViewPort()
{
    _initHelper.assertInitialized();

    if (!_viewPort)
    {
        _viewPort = ServiceLocator::make<IViewPort>(this);
    }

    return _viewPort;
}


void ViewPortPresenter::setPosition(QPointF position)
{
    _initHelper.assertInitialized();
    _position = position;
    calculateTransforms();
    emit positionChanged(_position);
    emit scrollStateChanged();
}

bool ViewPortPresenter::canScroll()
{
    _initHelper.assertInitialized();
    return !_documentPresenter->isEmpty();
}

QRect ViewPortPresenter::getScrollRect()
{
    _initHelper.assertInitialized();

    if (!canScroll())
    {
        return QRect();
    }

    int x_margin = _size.width();
    int y_margin = _size.height();

    QRectF canvasBoundingRect = _fromCanvas.mapRect(QRectF(QPointF(), _documentPresenter->getCanvasSize()));
    QRect basicRect = QRect(
        canvasBoundingRect.x() - x_margin,
        canvasBoundingRect.y() - y_margin,

        canvasBoundingRect.width() + x_margin + 1,
        canvasBoundingRect.height() + y_margin + 1
    );

    return basicRect.united(QRect(0,0,1,1));
}

void ViewPortPresenter::setScrollPosition(QPoint position)
{
    _initHelper.assertInitialized();

    if (!canScroll())
    {
        return;
    }

    position = projectOntoScrollRect(position);

    setPosition(_ontoCanvas.map(QPointF(position) + _center));
}

void ViewPortPresenter::gripPan(QPointF start, QPointF end)
{
    QPointF newPosition = _position + start - end;
    setPosition(newPosition);
    //setScrollPosition(getScrollPosition(newPosition));
}

QPoint ViewPortPresenter::getScrollPosition(QPointF position)
{
    return (_fromCanvas.map(position) - _center).toPoint();
}

QPoint ViewPortPresenter::projectOntoScrollRect(QPoint scrollPosition)
{
    QRect scrollRect = getScrollRect();
    QPoint result = scrollPosition;

    if (scrollRect.contains(scrollPosition))
    {
        return result;
    }

    if (scrollPosition.x() < scrollRect.left())
    {
        result.setX(scrollRect.left());
    }
    else if (scrollPosition.x() > scrollRect.right())
    {
        result.setX(scrollRect.right());
    }

    if (scrollPosition.y() < scrollRect.top())
    {
        result.setY(scrollRect.top());
    }
    else if (scrollPosition.y() > scrollRect.bottom())
    {
        result.setY(scrollRect.bottom());
    }

    return result;
}

bool ViewPortPresenter::canZoom()
{
    return !_documentPresenter->isEmpty();
}

void ViewPortPresenter::setZoom(double zoom)
{
    _initHelper.assertInitialized();
    _zoom = zoom;
    calculateTransforms();
    emit zoomChanged(_zoom);
    emit scrollStateChanged();
}

double ViewPortPresenter::constrainZoom(double zoom)
{
    if (zoom > ZoomPresetHelper::zoomValueOf(MAX_ZOOM_PRESET))
    {
        return ZoomPresetHelper::zoomValueOf(MAX_ZOOM_PRESET);
    }
    else if ( zoom < ZoomPresetHelper::zoomValueOf(MIN_ZOOM_PRESET))
    {
        return ZoomPresetHelper::zoomValueOf(MIN_ZOOM_PRESET);
    }
    else
    {
        return zoom;
    }
}

void ViewPortPresenter::setRotation(double rotation)
{
    _initHelper.assertInitialized();
    _rotation = rotation;
    _rotatePreset = RotatePreset::nullrotation;
    calculateTransforms();
    emit rotationChanged(_rotation);
    emit scrollStateChanged();
}


void ViewPortPresenter::resetView()
{
    centerView();
    setZoomPreset(DEFAULT_ZOOM_PRESET);
    setRotatePreset(DEFAULT_ROTATE_PRESET);
}

void ViewPortPresenter::fitWidth()
{

}

void ViewPortPresenter::fitCanvas()
{

}

void ViewPortPresenter::setZoomPreset(ZoomPreset preset)
{
    _initHelper.assertInitialized();
    _zoomPreset = preset;
    setZoom(ZoomPresetHelper::zoomValueOf(preset));
    //emit needsUpdate();
}

IViewPortPresenter::ZoomPreset ViewPortPresenter::zoomIn(bool* zoomed)
{
    _initHelper.assertInitialized();
    bool p_zoomed = false;

    if (_zoomPreset != ZoomPreset::nullzoom) {
        ZoomPreset newPreset = ZoomPresetHelper::nextUp(_zoomPreset);

        if (newPreset != ZoomPreset::nullzoom)
        {
            setZoomPreset(newPreset);
            p_zoomed = true;
        }
    }
    else
    {
        double zoom = getZoom();
        ZoomPreset newPreset = ZoomPresetHelper::nextUp(zoom);

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

IViewPortPresenter::ZoomPreset ViewPortPresenter::zoomOut(bool* zoomed)
{
    _initHelper.assertInitialized();
    bool p_zoomed = false;

    if (_zoomPreset != ZoomPreset::nullzoom) {
        ZoomPreset newPreset = ZoomPresetHelper::nextDown(_zoomPreset);

        if (newPreset != ZoomPreset::nullzoom)
        {
            setZoomPreset(newPreset);
            p_zoomed = true;
        }
    }
    else
    {
        double zoom = getZoom();
        ZoomPreset newPreset = ZoomPresetHelper::nextDown(zoom);

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

IViewPortPresenter::ZoomPreset ViewPortPresenter::zoomTo(double zoom, bool snapToPreset)
{
    _initHelper.assertInitialized();
    ZoomPreset newPreset = ZoomPreset::nullzoom;
    
    if (snapToPreset)
        newPreset = ZoomPresetHelper::nearest(zoom, ZOOM_SNAP_THRESHOLD);

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

void ViewPortPresenter::setRotatePreset(RotatePreset preset)
{
    _initHelper.assertInitialized();
    _rotatePreset = preset;
    setRotation(RotatePresetHelper::rotateValueOf(preset));
}

IViewPortPresenter::RotatePreset ViewPortPresenter::turntableCcw(bool* rotated)
{
    _initHelper.assertInitialized();

    bool p_rotated = false;

    if (_rotatePreset != RotatePreset::nullrotation) {
        RotatePreset newPreset = RotatePresetHelper::nextDown(_rotatePreset);

        if (newPreset != RotatePreset::nullrotation)
        {
            setRotatePreset(newPreset);
            p_rotated = true;
        }
    }
    else
    {
        double rotation = getRotation();
        RotatePreset newPreset = RotatePresetHelper::nextDown(rotation);

        if (newPreset != _rotatePreset)
        {
            setRotatePreset(newPreset);
            p_rotated = true;
        }
    }

    if (rotated)
        *rotated = p_rotated;

    return _rotatePreset;
}

IViewPortPresenter::RotatePreset ViewPortPresenter::turntableCw(bool* rotated)
{
    _initHelper.assertInitialized();

    bool p_rotated = false;

    if (_rotatePreset != RotatePreset::nullrotation) {
        RotatePreset newPreset = RotatePresetHelper::nextUp(_rotatePreset);

        if (newPreset != RotatePreset::nullrotation)
        {
            setRotatePreset(newPreset);
            p_rotated = true;
        }
    }
    else
    {
        double rotation = getRotation();
        RotatePreset newPreset = RotatePresetHelper::nextUp(rotation);

        if (newPreset != _rotatePreset)
        {
            setRotatePreset(newPreset);
            p_rotated = true;
        }
    }

    if (rotated)
        *rotated = p_rotated;

    return _rotatePreset;
}

void ViewPortPresenter::gripPivot(QPointF gripStart, QPointF gripEnd)
{
    QLineF startLine(_position, gripStart);
    QLineF endLine(_position, gripEnd);

    if (QLineF(_center, _fromCanvas.map(gripStart)).length() < 4)
        return;

    if (QLineF(_center, _fromCanvas.map(gripEnd)).length() < 4)
        return;

    double zoom = _zoom * (endLine.length() / startLine.length());
    double constrainedZoom = constrainZoom(zoom);

    setZoom(constrainedZoom);

    double rotation = _rotation - endLine.angle() + startLine.angle();
    setRotation(rotation);
}

void ViewPortPresenter::setViewPortSize(QSize size)
{
    _initHelper.assertInitialized();
    _size = size;
    _center = QPointF((double)(_size.width()) / 2.0, (double)(_size.height()) / 2.0);
    calculateTransforms();
    emit scrollStateChanged();
}

void ViewPortPresenter::calculateTransforms()
{
    _ontoCanvas = QTransform();

    _ontoCanvas.translate(_position.x(), _position.y());
    _ontoCanvas.rotate(-_rotation);
    _ontoCanvas.scale(1 / _zoom, 1 / _zoom);
    _ontoCanvas.translate(-_center.x(), -_center.y());

    _fromCanvas = _ontoCanvas.inverted();

    emit transformsChanged();
}

void ViewPortPresenter::centerView()
{
    setPosition(QRectF(QPointF(), _documentPresenter->getCanvasSize()).center());
    //emit needsUpdate();
}
