#include "brushiconhelper.hpp"
#include "interfaces/editing/ibrushengine.hpp"

#include <QtDebug>

#include <QLineF>
#include <cmath> // for sqrt

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
    _brushStroke.setPreview(true);
}

void BrushIconHelper::BrushIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode);
    Q_UNUSED(state);

    if (!_helper) return;

    const double size = _brushStroke.getSize();

    QPointF center;

    // This position algorithm assumes that the brush will look good if treated
    // as a circle with r = 1/2 size. Naturally, as brush appearances become
    // more complex, this behavior will become more fine-tuned and customizable
    // with brush parameters.
    //
    // This algorithm gives best results if rect is a square, but (I think)
    // won't break with an eccentric rectangle. I don't anticipate this being a
    // problem.

    if (size <= qMin(rect.width(), rect.height()))
    {
        // The brush is small enough to fit entirely within the icon, so it
        // will be centered in the icon.

        center = QRectF(rect).center();
    }
    else if (size / 2 <= qMin(rect.width(), rect.height()))
    {
        // A sector of the brush can fit within the icon, so the bounding
        // square of the brush circle will be aligned to the top-left corner
        // of the icon.

        center = QRectF(rect.topLeft(), QSize(size, size)).center();
    }
    else 
    {
        // The brush is very large. It will be positioned such that the top right
        // and bottom left corners of the icon are on the edge of the circle of
        // the brush, showing the maximum curvature, and hopefully giving the
        // user a reasonable estimate of the brush's true size

        // Naming the bottom-left corner of the icon A, and the top-right B,
        // this calculates the position of a third point C such that the circle
        // of the brush centered on C will pass through A and B.
        
        // Triangle ABC is isoceles with the unequal side being AB. Thus, if 
        // the midpoint of AB is D, then triangle ADC is a right triangle with
        // two known side lengths. We calculate side DC with Pythagoras, then 
        // use QLineF to extend a line from D perpendicular to AB, and take its 
        // endpoint.

        QLineF ab(rect.topRight() + QPoint(0, 1), rect.bottomLeft() + QPoint(1, 0));
        qreal ad_l = ab.length() / 2;
        qreal r = size / 2;
        qreal dc_l = sqrt(r * r - ad_l * ad_l);

        QLineF dc = ab.normalVector();
        qreal dc_a = dc.angle();
        dc.setP1(ab.center());
        dc.setAngle(dc_a);
        dc.setLength(dc_l);

        center = dc.p2();
    }

    _helper->_surface->clear();

    _brushStroke.clear();
    _brushStroke.moveTo(center);
    _brushStroke.paint();

    // TODO: adjust background color for contrast if needed

    painter->fillRect(rect, _helper->_background);
    render(_helper->_surface->getRenderStep(), rect, painter);
}