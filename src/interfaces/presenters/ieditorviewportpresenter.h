#ifndef IEDITORVIEWPORTPRESENTER_H
#define IEDITORVIEWPORTPRESENTER_H

#include <QObject>
#include <QPointF>
#include <QTransform>

#include "ieditorpresenter.h"
#include "icanvaspresenter.h"

class IEditorViewPortPresenter
{
public:

    enum ZoomPreset {
        nullzoom,
        _5percent,
        _10percent,
        onesixth,
        _25percent,
        onethird,
        _50percent,
        _75percent,
        _100percent,
        _150percent,
        _200percent,
        _300percent,
        _400percent,
        _500percent,
        _800percent,
        _1000percent,
        _1600percent,
        _2000percent
    };

    enum RotatePreset {
        nullrotation,
        _0deg,
        _45deg,
        _90deg,
        _135deg,
        _180deg,
        _225deg,
        _270deg,
        _315deg
    };

    const ZoomPreset MIN_ZOOM_PRESET = ZoomPreset::_5percent;
    const ZoomPreset MAX_ZOOM_PRESET = ZoomPreset::_2000percent;
    const ZoomPreset DEFAULT_ZOOM_PRESET = ZoomPreset::_100percent;

    virtual ~IEditorViewPortPresenter() {}

    virtual void initialize(IEditorPresenter* editorPresenter) = 0;
    virtual IEditorPresenter* getEditorPresenter() = 0;
    virtual ICanvasPresenter* getCanvasPresenter() = 0;

    virtual double getZoom() = 0;
    virtual void setZoom(double zoom) = 0;

    virtual double getRotation() = 0;
    virtual void setRotation(double rotation) = 0;

    virtual QPointF getPosition() = 0;
    virtual void setPosition(QPointF center) = 0;

    virtual QTransform getOntoCanvasTransform() = 0;
    virtual QTransform getFromCanvasTransform() = 0;

    virtual QSize getSize() = 0;
    virtual void setSize(QSize size) = 0;
    virtual QPointF getCenter() = 0;

    virtual QRect getCanvasBoundingRect() = 0;
    virtual QRect getScrollRect() = 0;


    virtual ZoomPreset getZoomPreset() = 0;
    virtual void setZoomPreset(ZoomPreset preset) = 0;
    virtual double getMaxZoomPresetValue() = 0;
    virtual double getMinZoomPresetValue() = 0;
    virtual bool canZoomIn() = 0;
    virtual bool canZoomOut() = 0;
    virtual ZoomPreset zoomTo(double zoom, bool snapToPreset = true) = 0;

    virtual void gripMove(QPoint gripStart, QPoint gripEnd) = 0;
    virtual void nudgeMove(int dx, int dy) = 0;
    virtual void gripZoom(QPoint gripStart, QPoint gripEnd) = 0;
    virtual void gripRotate(QPoint gripStart, QPoint gripEnd) = 0;
    virtual void twoGripTransform(QPoint grip1Start, QPoint grip1End, QPoint grip2Start, QPoint grip2End) = 0;

signals:
    void positionChanged(QPointF position);
    void rotationChanged(double rotation);
    void zoomChanged(double zoom);

    void transformsChanged();

    void needsUpdate();

public slots:
    void resetView();
    void fitWidth();
    void fitCanvas();

    virtual ZoomPreset zoomIn(bool* zoomed = nullptr) = 0;
    virtual ZoomPreset zoomOut(bool* zoomed = nullptr) = 0;
    
    virtual RotatePreset turntableCcw(bool* rotated = nullptr) = 0;
    virtual RotatePreset turntableCw(bool* rotated = nullptr) = 0;

};

#endif //IEDITORVIEWPORTPRESENTER_H