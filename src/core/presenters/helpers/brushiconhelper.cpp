#include "brushiconhelper.hpp"
#include "interfaces/editing/ibrushengine.hpp"

#include <QtDebug>

#include <QLineF>
#include <cmath>

#include "servicelocator.hpp"

#include "utilities/editing/brushstroke.hpp"
#include "utilities/render/renderutils.hpp"

BrushIconHelper::BrushIconHelper(QObject* parent)
    : QObject(parent)
{
    _surface = ServiceLocator::makeShared<IRasterSurface>();
}

QIcon BrushIconHelper::icon(BrushId brush, QColor color, double size) const
{
    return QIcon(new BrushIconEngine(QPointer<const BrushIconHelper>(this), brush, color, size));
}

BrushIconHelper::BrushIconEngine::BrushIconEngine(QPointer<const BrushIconHelper> helper, BrushId brush, QColor color, double size)
    : _helper(helper), _brushStroke(brush, color, size, helper->_surface)
{
}

void BrushIconHelper::BrushIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
    if (!_helper) return;

    _helper->_surface->clear();

    QPointF center;

    const double size = _brushStroke.getSize();

    // This position algorithm assumes that the brush will look okay if treated
    // as a circle with r = 1/2 size.
    if (size <= qMin(rect.width(), rect.height()))
    {
        // The brush is small enough to fit entirely within the frame, so it
        // will be centered.
        center = rect.center();
    }
    else if (size / 2 <= qMin(rect.width(), rect.height()))
    {
        // A sector of the brush can fit within the frame, so it will be aligned
        // with the top left corner.
        center = QRect(rect.topLeft(), QSize(size, size)).center();
    }
    else 
    {
        // The brush is very large. It will be positioned such that the top right
        // and bottom left corners of the icon are on the edge of the brush, 
        // showing the maximum curvature, and hopefully giving the user a
        // reasonable estimate of the brush's size

        QRectF rectf(rect);
        QLineF ab(rectf.topRight(), rectf.bottomLeft());
        qreal halfab = ab.length() / 2;
        qreal r = size / 2;
        qreal ext = sqrt(r * r - halfab * halfab);

        QLineF normal = ab.normalVector();
        qreal angle = normal.angle();
        normal.setP1(ab.center());
        normal.setAngle(angle);
        normal.setLength(ext);

        center = normal.p2();
    }

    _brushStroke.clear();
    _brushStroke.moveTo(center);
    _brushStroke.paint();

    painter->fillRect(rect, _helper->_background);
    render(_helper->_surface->getRenderStep(), rect, painter);
}