/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "viewportpresenter.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/idocumentpresenter.hpp"

#include "utilities/qobject.hpp"
#include "servicelocator.hpp"


#include <QLineF>
#include <QPainter>
using namespace Addle;

const PresetMap<ViewPortPresenter::RotatePreset, double> ViewPortPresenter::_rotatePresetMap
    = PresetMap<RotatePreset, double>(
            {
                { RotatePreset::_0deg,   0.0 },
                { RotatePreset::_45deg,  45.0 },
                { RotatePreset::_90deg,  90.0 },
                { RotatePreset::_135deg, 135.0 },
                { RotatePreset::_180deg, 180.0 },
                { RotatePreset::_225deg, 225.0 },
                { RotatePreset::_270deg, 270.0 },
                { RotatePreset::_315deg, 315.0 }
            },
            true
        );

const PresetMap<ViewPortPresenter::ZoomPreset, double> ViewPortPresenter::_zoomPresetMap
    = PresetMap<ZoomPreset, double>(
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
            },
            false
        );

void ViewPortPresenter::initialize(IMainEditorPresenter* mainEditorPresenter)
{
//     const Initializer init(_initHelper);
//     _mainEditorPresenter = mainEditorPresenter;
// 
//     connect_interface(
//         _mainEditorPresenter,
//         SIGNAL(isEmptyChanged(bool)),
//         this,
//         SLOT(onMainEditorPresenter_isEmptyChanged())
//     );
}

void ViewPortPresenter::setPosition(QPointF position)
{
    try
    {
        ASSERT_INIT();

        _position = position;
        _transformCache.recalculate();

        emit positionChanged(_position);
    }
    ADDLE_SLOT_CATCH
}

ViewPortPresenter::ScrollState ViewPortPresenter::scrollState_p()
{
//     ASSERT_INIT();
// 
//     if (!canNavigate())
//     {
//         return ScrollState();
//     }
// 
//     QTransform fromCanvas = fromCanvasTransform();
// 
//     QRect canvasRect = fromCanvas.mapRect(QRect(QPoint(), _mainEditorPresenter->documentPresenter()->size()));
//     
//     return ScrollState(canvasRect.size(), QRect(-canvasRect.topLeft(), _size));
}

void ViewPortPresenter::scrollX(int x)
{
    try 
    {
        QTransform ontoCanvas = ontoCanvasTransform();
        auto& scrollState = _scrollStateCache.value();

        setPosition(ontoCanvas.map(QPointF(x - scrollState._x, 0) + _center));
    }
    ADDLE_SLOT_CATCH
}

void ViewPortPresenter::scrollY(int y)
{
    try 
    {
        QTransform ontoCanvas = ontoCanvasTransform();
        auto& scrollState = _scrollStateCache.value();

        setPosition(ontoCanvas.map(QPointF(0, y - scrollState._y) + _center));
    }
    ADDLE_SLOT_CATCH
}

void ViewPortPresenter::gripPan(QPointF start, QPointF end)
{
    QPointF newPosition = _position + start - end;
    setPosition(newPosition);
}

bool ViewPortPresenter::canNavigate_p()
{
//     return !_mainEditorPresenter->isEmpty();
    return false;
}

void ViewPortPresenter::setHasFocus(bool value)
{
    ASSERT_INIT();
    if (_hasFocus != value)
    {
        _hasFocus = value;
        emit focusChanged(_hasFocus);
    }
}

void ViewPortPresenter::setZoom(double zoom)
{
    ASSERT_INIT();

    ZoomPreset nearestPreset = _zoomPresetMap.nearest(zoom);
    double nearest = _zoomPresetMap.valueOf(nearestPreset);

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
    ASSERT_INIT();

    _zoomPreset = preset;
    setZoom_p(_zoomPresetMap.valueOf(preset));
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
    if (zoom > _zoomPresetMap.valueOf(MAX_ZOOM_PRESET))
    {
        return _zoomPresetMap.valueOf(MAX_ZOOM_PRESET);
    }
    else if ( zoom < _zoomPresetMap.valueOf(MIN_ZOOM_PRESET))
    {
        return _zoomPresetMap.valueOf(MIN_ZOOM_PRESET);
    }
    else
    {
        return zoom;
    }
}

void ViewPortPresenter::setRotation(double rotation)
{
    ASSERT_INIT();
    _rotation = rotation;
    _rotatePreset = RotatePreset::nullrotation;

    _transformCache.recalculate();
    emit rotationChanged(_rotation);
}


void ViewPortPresenter::resetTransforms()
{
    try
    {
        centerView();
        setZoomPreset(DEFAULT_ZOOM_PRESET);
        setRotatePreset(DEFAULT_ROTATE_PRESET);
    }
    ADDLE_SLOT_CATCH
}

void ViewPortPresenter::fitWidth()
{

}

void ViewPortPresenter::fitCanvas()
{

}

IViewPortPresenter::ZoomPreset ViewPortPresenter::zoomIn(bool* zoomed)
{
    try 
    {
        ASSERT_INIT();
        bool p_zoomed = false;

        if (_zoomPreset != ZoomPreset::nullzoom) {
            ZoomPreset newPreset = _zoomPresetMap.nextUp(_zoomPreset);

            if (newPreset != ZoomPreset::nullzoom)
            {
                setZoomPreset(newPreset);
                p_zoomed = true;
            }
        }
        else
        {
            ZoomPreset newPreset = _zoomPresetMap.nextUp(zoom());

            if (newPreset != _zoomPreset)
            {
                setZoomPreset(newPreset);
                p_zoomed = true;
            }
        }

        if (zoomed)
            *zoomed = p_zoomed;
    }
    ADDLE_SLOT_CATCH

    return _zoomPreset;
}

IViewPortPresenter::ZoomPreset ViewPortPresenter::zoomOut(bool* zoomed)
{
    try
    {
        ASSERT_INIT();
        bool p_zoomed = false;

        if (_zoomPreset != ZoomPreset::nullzoom) {
            ZoomPreset newPreset = _zoomPresetMap.nextDown(_zoomPreset);

            if (newPreset != ZoomPreset::nullzoom)
            {
                setZoomPreset(newPreset);
                p_zoomed = true;
            }
        }
        else
        {
            ZoomPreset newPreset = _zoomPresetMap.nextDown(zoom());

            if (newPreset != _zoomPreset)
            {
                setZoomPreset(newPreset);
                p_zoomed = true;
            }
        }

        if (zoomed)
            *zoomed = p_zoomed;
    }
    ADDLE_SLOT_CATCH

    return _zoomPreset;
}

IViewPortPresenter::ZoomPreset ViewPortPresenter::zoomTo(double zoom, bool snapToPreset)
{
    ASSERT_INIT();
    ZoomPreset newPreset = ZoomPreset::nullzoom;
    
    if (snapToPreset)
        newPreset = _zoomPresetMap.nearest(zoom, ZOOM_SNAP_THRESHOLD);

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
    ASSERT_INIT();
    _rotatePreset = preset;
    setRotation(_rotatePresetMap.valueOf(preset));
}

IViewPortPresenter::RotatePreset ViewPortPresenter::turntableCcw(bool* rotated)
{
    try 
    {
        ASSERT_INIT();

        bool p_rotated = false;

        if (_rotatePreset != RotatePreset::nullrotation) {
            RotatePreset newPreset = _rotatePresetMap.nextDown(_rotatePreset);

            if (newPreset != RotatePreset::nullrotation)
            {
                setRotatePreset(newPreset);
                p_rotated = true;
            }
        }
        else
        {
            RotatePreset newPreset = _rotatePresetMap.nextDown(rotation());

            if (newPreset != _rotatePreset)
            {
                setRotatePreset(newPreset);
                p_rotated = true;
            }
        }

        if (rotated)
            *rotated = p_rotated;
    }
    ADDLE_SLOT_CATCH

    return _rotatePreset;
}

IViewPortPresenter::RotatePreset ViewPortPresenter::turntableCw(bool* rotated)
{
    try 
    {
        ASSERT_INIT();

        bool p_rotated = false;

        if (_rotatePreset != RotatePreset::nullrotation) {
            RotatePreset newPreset = _rotatePresetMap.nextUp(_rotatePreset);

            if (newPreset != RotatePreset::nullrotation)
            {
                setRotatePreset(newPreset);
                p_rotated = true;
            }
        }
        else
        {
            RotatePreset newPreset = _rotatePresetMap.nextUp(rotation());

            if (newPreset != _rotatePreset)
            {
                setRotatePreset(newPreset);
                p_rotated = true;
            }
        }

        if (rotated)
            *rotated = p_rotated;
    }
    ADDLE_SLOT_CATCH

    return _rotatePreset;
}

void ViewPortPresenter::gripPivot(QPointF gripStart, QPointF gripEnd)
{
    QLineF startLine(_position, gripStart);
    QLineF endLine(_position, gripEnd);

    QTransform fromCanvas = fromCanvasTransform();

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
    try 
    {
        ASSERT_INIT();
        _size = size;
        _center = QPointF((double)(_size.width()) / 2.0, (double)(_size.height()) / 2.0);
        _transformCache.recalculate();
    }
    ADDLE_SLOT_CATCH
}

void ViewPortPresenter::onMainEditorPresenter_isEmptyChanged()
{
    try
    {
        _canNavigateCache.recalculate();
        _scrollStateCache.recalculate();
        resetTransforms();
    }
    ADDLE_SLOT_CATCH
}

void ViewPortPresenter::propagateCanNavigate()
{
    _canZoomInCache.recalculate();
    _canZoomOutCache.recalculate();
}

ViewPortPresenter::TransformPair ViewPortPresenter::transforms_p()
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
//     if (_mainEditorPresenter->isEmpty())
//         setPosition(QPointF());
//     else
//         setPosition(QRectF(QPointF(), _mainEditorPresenter->documentPresenter()->size()).center());
}
