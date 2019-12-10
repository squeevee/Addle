#ifndef COMMONBRUSHRENDERERS_HPP
#define COMMONBRUSHRENDERERS_HPP

#include <QtDebug>
#include "interfaces/presenters/tools/ibrushliketoolpresenter.hpp"
#include "basebrushrenderer.hpp"

class PixelBrushTipRenderer : public BasePixelBrushRenderer
{
    virtual ~PixelBrushTipRenderer() = default;
    BrushTipId getId() { return IBrushLikeToolPresenter::CommonBrushTips::Pixel; }
    bool snapToPixels() { return true; }
    bool sizeInvariant()  { return true; }
    QRect getBoundingRect(double, QPointF center)
    {
        return QRect(pointFloor(center), QSize(1, 1));
    }
    void render(double, QPointF center, QPainter& painter)
    {
        painter.drawPoint(pointFloor(center));
    }
};

class TwoSquareBrushTipRenderer : public BasePixelBrushRenderer
{
    virtual ~TwoSquareBrushTipRenderer() = default;
    BrushTipId getId() { return IBrushLikeToolPresenter::CommonBrushTips::TwoSquare; }
    bool sizeInvariant()  { return true; }
    QRect getBoundingRect(double brushSize, QPointF center)
    { 
        return QRect(pointFloor(center), QSize(2,2));
    }
    void render(double, QPointF center, QPainter& painter)
    {
        QRect rect(pointFloor(center), QSize(2,2));

        painter.drawRect(rect);
    }
};

class SquareBrushTipRenderer : public BasePixelBrushRenderer
{
    virtual ~SquareBrushTipRenderer() = default;
    BrushTipId getId() { return IBrushLikeToolPresenter::CommonBrushTips::Square; }
    void render(double brushSize, QPointF center, QPainter& painter)
    {
        int properSize = qRound(brushSize);
        painter.drawRect( center.x() - properSize / 2, center.y() - properSize / 2, properSize, properSize );
    }
};

class HardCircleBrushTipRenderer : public BasePixelBrushRenderer
{
    virtual ~HardCircleBrushTipRenderer() = default;
    BrushTipId getId() { return IBrushLikeToolPresenter::CommonBrushTips::HardCircle; }
    void render(double brushSize, QPointF center, QPainter& painter)
    {
        
    }
};

class SoftCircleBrushTipRenderer : public BaseSmoothBrushRenderer
{
    virtual ~SoftCircleBrushTipRenderer() = default;
    BrushTipId getId() { return IBrushLikeToolPresenter::CommonBrushTips::SoftCircle; }
    void render(double brushSize, QPointF center, QPainter& painter)
    {

    }
};

#endif // COMMONBRUSHRENDERERS_HPP