#include "viewportpresenter.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "utilities/qtextensions/qobject.hpp"
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

void ViewPortPresenter::initialize(IMainEditorPresenter* mainEditorPresenter)
{
    _initHelper.initializeBegin();
    _mainEditorPresenter = mainEditorPresenter;

    connect_interface(
        _mainEditorPresenter,
        SIGNAL(isEmptyChanged(bool)),
        this,
        SLOT(onMainEditorPresenter_isEmptyChanged())
    );

    _transformCache.recalculate();

    _initHelper.initializeEnd();
}

void ViewPortPresenter::setPosition(QPointF position)
{
    _initHelper.check();

    _position = position;
    _transformCache.recalculate();

    emit positionChanged(_position);
}

ViewPortPresenter::ScrollState ViewPortPresenter::getScrollState_p()
{
    _initHelper.check();

    if (!canNavigate())
    {
        return ScrollState();
    }

    QTransform fromCanvas = getFromCanvasTransform();

    QRect canvasRect;// = fromCanvas.mapRect(QRect(QPoint(), _mainEditorPresenter->getCanvasSize()));
    
    return ScrollState(canvasRect.size(), QRect(-canvasRect.topLeft(), _size));
}

void ViewPortPresenter::scrollX(int x)
{
    QTransform ontoCanvas = getOntoCanvasTransform();
    auto& scrollState = _scrollStateCache.getValue();

    setPosition(ontoCanvas.map(QPointF(x - scrollState._x, 0) + _center));
}

void ViewPortPresenter::scrollY(int y)
{
    QTransform ontoCanvas = getOntoCanvasTransform();
    auto& scrollState = _scrollStateCache.getValue();

    setPosition(ontoCanvas.map(QPointF(0, y - scrollState._y) + _center));
}

void ViewPortPresenter::gripPan(QPointF start, QPointF end)
{
    QPointF newPosition = _position + start - end;
    setPosition(newPosition);
}

bool ViewPortPresenter::canNavigate_p()
{
    return !_mainEditorPresenter->isEmpty();
}

void ViewPortPresenter::setZoom(double zoom)
{
    _initHelper.check();

    ZoomPreset nearestPreset = _zoomPresetHelper.nearest(zoom);
    double nearest = _zoomPresetHelper.valueOf(nearestPreset);

    if (qAbs(nearest - zoom) < 0.01)
    {
        zoom = nearest;
        _zoomPreset = nearestPreset;
    }
    else 
    {
        _zoomPreset = ZoomPreset::nullzoom;
    }
    setZoom_p(zoom);
}

void ViewPortPresenter::setZoomPreset(ZoomPreset preset)
{
    _initHelper.check();

    _zoomPreset = preset;
    setZoom_p(_zoomPresetHelper.valueOf(preset));
}

void ViewPortPresenter::setZoom_p(double zoom)
{
    if (zoom != _zoom)
    {
        _zoom = zoom;
        _canZoomInCache.recalculate();
        _canZoomOutCache.recalculate();
        _transformCache.recalculate();
        emit zoomChanged(_zoom);
    }
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
    _initHelper.check();
    _rotation = rotation;
    _rotatePreset = RotatePreset::nullrotation;

    _transformCache.recalculate();
    emit rotationChanged(_rotation);
}


void ViewPortPresenter::resetTransforms()
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

IViewPortPresenter::ZoomPreset ViewPortPresenter::zoomIn(bool* zoomed)
{
    _initHelper.check();
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
    _initHelper.check();
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
    _initHelper.check();
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
    _initHelper.check();
    _rotatePreset = preset;
    setRotation(_rotatePresetHelper.valueOf(preset));
}

IViewPortPresenter::RotatePreset ViewPortPresenter::turntableCcw(bool* rotated)
{
    _initHelper.check();

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
    _initHelper.check();

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

    QTransform fromCanvas = getFromCanvasTransform();

    if (QLineF(_center, fromCanvas.map(gripStart)).length() < 4)
        return;

    if (QLineF(_center, fromCanvas.map(gripEnd)).length() < 4)
        return;

    double zoom = _zoom * (endLine.length() / startLine.length());
    double constrainedZoom = constrainZoom(zoom);

    setZoom(constrainedZoom);

    double rotation = _rotation - endLine.angle() + startLine.angle();
    setRotation(rotation);
}

void ViewPortPresenter::setSize(QSize size)
{
    _initHelper.check();
    _size = size;
    _center = QPointF((double)(_size.width()) / 2.0, (double)(_size.height()) / 2.0);
    _transformCache.recalculate();
}

void ViewPortPresenter::onMainEditorPresenter_isEmptyChanged()
{
    _canNavigateCache.recalculate();
}

void ViewPortPresenter::propagateCanNavigate()
{
    _canZoomInCache.recalculate();
    _canZoomOutCache.recalculate();
}

ViewPortPresenter::TransformPair ViewPortPresenter::getTransforms_p()
{
    QTransform ontoCanvas;

    ontoCanvas.translate(_position.x(), _position.y());
    ontoCanvas.rotate(-_rotation);
    ontoCanvas.scale(1 / _zoom, 1 / _zoom);
    ontoCanvas.translate(-_center.x(), -_center.y());

    QTransform fromCanvas = ontoCanvas.inverted();

    return { ontoCanvas, fromCanvas };
}

void ViewPortPresenter::centerView()
{
    //setPosition(QRectF(QPointF(), _mainEditorPresenter->getCanvasSize()).center());
    //emit needsUpdate();
}
