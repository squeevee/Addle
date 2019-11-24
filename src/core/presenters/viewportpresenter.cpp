#include "viewportpresenter.hpp"
#include "common/interfaces/presenters/icanvaspresenter.hpp"

#include "common/servicelocator.hpp"

#include <QPainter>

void ViewPortPresenter::initialize(IDocumentPresenter* documentPresenter, ICanvasPresenter* canvasPresenter)
{
    _initHelper.initializeBegin();
    _documentPresenter = documentPresenter;
    _canvasPresenter = canvasPresenter;

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
    return !_canvasPresenter->isEmpty();
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

    QRectF canvasBoundingRect = _fromCanvas.mapRect(QRectF(QPointF(), _canvasPresenter->getSize()));
    QRect basicRect = QRect(
        canvasBoundingRect.x() - x_margin,
        canvasBoundingRect.y() - y_margin,

        canvasBoundingRect.width() + x_margin + 1,
        canvasBoundingRect.height() + y_margin + 1
    );

    if (canScrollInfinitely())
    {
        // not implemented
        return QRect();
    }
    else
    {
        return basicRect;
    }

    return QRect();
}

void ViewPortPresenter::setScrollPosition(QPoint position)
{
    _initHelper.assertInitialized();

    if (!canScroll())
    {
        return;
    }

    if (!canScrollInfinitely())
    {
        //normalize
    }

    setPosition(_ontoCanvas.map(QPointF(position) + _center));
}

bool ViewPortPresenter::canZoom()
{
    return !_canvasPresenter->isEmpty();
}

void ViewPortPresenter::setZoom(double zoom)
{
    _initHelper.assertInitialized();
    _zoom = zoom;
    calculateTransforms();
    emit zoomChanged(_zoom);
    emit scrollStateChanged();
}

void ViewPortPresenter::setRotation(double rotation)
{
    _initHelper.assertInitialized();
    _rotation = rotation;
    calculateTransforms();
    emit rotationChanged(_rotation);
    emit scrollStateChanged();
}


void ViewPortPresenter::resetView()
{
    centerView();
    setZoomPreset(DEFAULT_ZOOM_PRESET);
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
    setZoom(ZoomRotateHelper::zoomValueOf(preset));
    //emit needsUpdate();
}

IViewPortPresenter::ZoomPreset ViewPortPresenter::zoomIn(bool* zoomed)
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

IViewPortPresenter::ZoomPreset ViewPortPresenter::zoomOut(bool* zoomed)
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

IViewPortPresenter::ZoomPreset ViewPortPresenter::zoomTo(double zoom, bool snapToPreset)
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

IViewPortPresenter::RotatePreset ViewPortPresenter::turntableCcw(bool* rotated)
{
    return IViewPortPresenter::RotatePreset::nullrotation;
}

IViewPortPresenter::RotatePreset ViewPortPresenter::turntableCw(bool* rotated)
{
    return IViewPortPresenter::RotatePreset::nullrotation;
}

QRectF ViewPortPresenter::getSceneRect()
{
    return _ontoCanvas.mapRect(QRectF(QPointF(), getSize()));
}

void ViewPortPresenter::setSize(QSize size)
{
    _initHelper.assertInitialized();
    _size = size;
    _center = QPointF(_size.width() / 2, _size.height() / 2);
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
    setPosition(QRectF(QPointF(), _canvasPresenter->getSize()).center());
    //emit needsUpdate();
}
