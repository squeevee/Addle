#include "viewportpresenter.hpp"

#include "servicelocator.hpp"

#include <QtDebug>

#include <QLineF>
#include <QPainter>

const PresetHelper<ViewPortPresenter::RotatePreset, double> ViewPortPresenter::_rotatePresetHelper
    = PresetHelper<RotatePreset, double>(
            true,
            RotatePreset::nullrotation,
            {
                { RotatePreset::_0deg,   0.0 },
                { RotatePreset::_45deg,  45.0 },
                { RotatePreset::_90deg,  90.0 },
                { RotatePreset::_135deg, 135.0 },
                { RotatePreset::_180deg, 180.0 },
                { RotatePreset::_225deg, 225.0 },
                { RotatePreset::_270deg, 270.0 },
                { RotatePreset::_315deg, 315.0 }
            }
        );

const PresetHelper<ViewPortPresenter::ZoomPreset, double> ViewPortPresenter::_zoomPresetHelper
    = PresetHelper<ZoomPreset, double>(
            false,
            ZoomPreset::nullzoom,
            {
                { ZoomPreset::_5percent,      0.05 },
                { ZoomPreset::_10percent,     0.10 },
                { ZoomPreset::onesixth,  (1.0 / 6) },
                { ZoomPreset::_25percent,     0.25 },
                { ZoomPreset::onethird,  (1.0 / 3) },
                { ZoomPreset::_50percent,     0.50 },
                { ZoomPreset::twothirds, (2.0 / 3) },
                { ZoomPreset::_100percent,    1.00 },
                { ZoomPreset::_150percent,    1.50 },
                { ZoomPreset::_200percent,    2.00 },
                { ZoomPreset::_300percent,    3.00 },
                { ZoomPreset::_400percent,    4.00 },
                { ZoomPreset::_600percent,    6.00 },
                { ZoomPreset::_800percent,    8.00 },
                { ZoomPreset::_1600percent,  16.00 }
            }
        );

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
    _zoomPreset = ZoomPreset::nullzoom;
    calculateTransforms();
    emit zoomChanged(_zoom);
    emit scrollStateChanged();
}

double ViewPortPresenter::constrainZoom(double zoom)
{
    if (zoom > _zoomPresetHelper.valueOf(MAX_ZOOM_PRESET))
    {
        return _zoomPresetHelper.valueOf(MAX_ZOOM_PRESET);
    }
    else if ( zoom < _zoomPresetHelper.valueOf(MIN_ZOOM_PRESET))
    {
        return _zoomPresetHelper.valueOf(MIN_ZOOM_PRESET);
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
    setZoom(_zoomPresetHelper.valueOf(preset));
    //emit needsUpdate();
}

IViewPortPresenter::ZoomPreset ViewPortPresenter::zoomIn(bool* zoomed)
{
    _initHelper.assertInitialized();
    bool p_zoomed = false;

    if (_zoomPreset != ZoomPreset::nullzoom) {
        ZoomPreset newPreset = _zoomPresetHelper.nextUp(_zoomPreset);

        if (newPreset != ZoomPreset::nullzoom)
        {
            setZoomPreset(newPreset);
            p_zoomed = true;
        }
    }
    else
    {
        double zoom = getZoom();
        ZoomPreset newPreset = _zoomPresetHelper.nextUp(zoom);

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
        ZoomPreset newPreset = _zoomPresetHelper.nextDown(_zoomPreset);

        if (newPreset != ZoomPreset::nullzoom)
        {
            setZoomPreset(newPreset);
            p_zoomed = true;
        }
    }
    else
    {
        double zoom = getZoom();
        ZoomPreset newPreset = _zoomPresetHelper.nextDown(zoom);

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
        newPreset = _zoomPresetHelper.nearest(zoom, ZOOM_SNAP_THRESHOLD);

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
    setRotation(_rotatePresetHelper.valueOf(preset));
}

IViewPortPresenter::RotatePreset ViewPortPresenter::turntableCcw(bool* rotated)
{
    _initHelper.assertInitialized();

    bool p_rotated = false;

    if (_rotatePreset != RotatePreset::nullrotation) {
        RotatePreset newPreset = _rotatePresetHelper.nextDown(_rotatePreset);

        if (newPreset != RotatePreset::nullrotation)
        {
            setRotatePreset(newPreset);
            p_rotated = true;
        }
    }
    else
    {
        double rotation = getRotation();
        RotatePreset newPreset = _rotatePresetHelper.nextDown(rotation);

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
        RotatePreset newPreset = _rotatePresetHelper.nextUp(_rotatePreset);

        if (newPreset != RotatePreset::nullrotation)
        {
            setRotatePreset(newPreset);
            p_rotated = true;
        }
    }
    else
    {
        double rotation = getRotation();
        RotatePreset newPreset = _rotatePresetHelper.nextUp(rotation);

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
