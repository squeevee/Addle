#include "brushiconhelper.hpp"
#include "interfaces/editing/ibrushengine.hpp"

#include <QtDebug>

#include <QLineF>
#include <cmath> // for sqrt

#include "servicelocator.hpp"

#include "utilities/editing/brushstroke.hpp"
#include "utilities/render/renderutils.hpp"

#include "utilities/mathutils.hpp"

BrushIconHelper::BrushIconHelper(QObject* parent)
    : QObject(parent)
{
    _surface = ServiceLocator::makeShared<IRasterSurface>();
}

QIcon BrushIconHelper::icon() const
{
    return QIcon(new BrushIconEngine(QPointer<const BrushIconHelper>(this), _brush));
}

QIcon BrushIconHelper::varySize(double size) const
{
    return QIcon(new BrushIconEngine(QPointer<const BrushIconHelper>(this), _brush, size));
}

// QIcon BrushIconHelper::varyColor(QColor color) const
// {
//     return QIcon(new BrushIconEngine(QPointer<const BrushIconHelper>(this), _brush, color));
// }

QIcon BrushIconHelper::varyBrush(BrushId brush) const
{
    return QIcon(new BrushIconEngine(QPointer<const BrushIconHelper>(this), brush));
}

QSharedPointer<ISizeSelectionPresenter::IconProvider> BrushIconHelper::sizeIconProvider()
{
    if (!_sizeIconProvider)
        _sizeIconProvider = QSharedPointer<ISizeSelectionPresenter::IconProvider>(
            new SizeIconProvider(this)
        );

    return _sizeIconProvider;
}

BrushIconHelper::BrushIconEngine::BrushIconEngine(
    QPointer<const BrushIconHelper> helper,
    BrushId brush,
    double size
)
    : _helper(helper),
    _brushStroke(brush, _helper->color(), size, helper->_surface)
{
    _brushStroke.setPreview(true);
}

BrushIconHelper::BrushIconEngine::BrushIconEngine(
    QPointer<const BrushIconHelper> helper,
    BrushId brush
)
    : _helper(helper),
    _brushStroke(brush, _helper->color(), 0, helper->_surface),
    _autoSize(true)
{
    _brushStroke.setPreview(true);
}

QIconEngine* BrushIconHelper::BrushIconEngine::clone() const
{ 
    if (_autoSize)
        return new BrushIconEngine(_helper, _brushStroke.id());
    else
        return new BrushIconEngine(_helper, _brushStroke.id(), _brushStroke.getSize());
}

void BrushIconHelper::BrushIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode);
    Q_UNUSED(state);

    if (!_helper) return;

    QPointF center;
    QRectF canonicalRect;
    double size;
    double scale = _helper->scale();

    if (_autoSize)
    {
        canonicalRect = QRectF(QPointF(), QSizeF(rect.width(), rect.height()));
        size = qMin(canonicalRect.width(), canonicalRect.height());
        _brushStroke.setSize(size);
    }
    else 
    {
        size = _brushStroke.getSize();
        canonicalRect = QRectF(
            QPointF(),
            QSizeF(rect.width() / scale, rect.height() / scale)
        );
    }

    _brushStroke.setColor(_helper->color());

    // This position algorithm assumes that the brush will look good if treated
    // as a circle with r = 1/2 size. Naturally, as brush appearances become
    // more complex, this behavior will become more fine-tuned and customizable
    // with brush parameters.
    //
    // This algorithm gives best results if rect is a square, but (I think)
    // won't break with an eccentric rectangle. I don't anticipate this being a
    // problem.

    // TODO: optimize so that when scaled and centered, the brush is drawn on
    // the center of a surface pixel, and will (hopefully) appear more
    // symmetrical.

    if (_autoSize || size <= qMin(canonicalRect.width(), canonicalRect.height()))
    {
        // The brush is small enough to fit entirely within the icon, so it
        // will be centered in the icon.

        center = canonicalRect.center();
    }
    else if (size / 2 <= qMin(canonicalRect.width(), canonicalRect.height()))
    {
        // A sector of the brush can fit within the icon, so the bounding
        // square of the brush circle will be aligned to the top-left corner
        // of the icon.

        center = QRectF(canonicalRect.topLeft(), QSize(size, size)).center();
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

        QLineF ab(canonicalRect.topRight(), canonicalRect.bottomLeft());
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

    painter->fillRect(rect, _helper->background());

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(rect.topLeft());
    painter->scale(scale, scale);
    render(_helper->_surface->getRenderStep(), coarseBoundRect(canonicalRect), painter);

    painter->restore();
}

QIcon BrushIconHelper::SizeIconProvider::icon(double size) const
{
    return _helper ? _helper->varySize(size) : QIcon();
}