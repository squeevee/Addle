#include "brushiconhelper.hpp"
#include "interfaces/editing/ibrushengine.hpp"

#include <QtDebug>

#include <QLineF>
#include <cmath> // for sqrt

#include "servicelocator.hpp"

#include "interfaces/models/ibrushmodel.hpp"
#include "interfaces/services/iappearanceservice.hpp"

#include "utilities/editing/brushstroke.hpp"
#include "utilities/render/renderutils.hpp"

#include "utils.hpp"
using namespace Addle;

QImage BrushIconHelper::_pattern8 = QImage();
QImage BrushIconHelper::_pattern64 = QImage();

BrushIconHelper::BrushIconHelper(QObject* parent)
    : QObject(parent)
{
    _underSurface = ServiceLocator::makeShared<IRasterSurface>();
    _brushSurface = ServiceLocator::makeShared<IRasterSurface>();
    _renderStack = ServiceLocator::makeShared<IRenderStack>(
        QList<QWeakPointer<IRenderStep>>({ 
            _underSurface->renderStep().toWeakRef(),
            _brushSurface->renderStep().toWeakRef()
        })
    );

    if (_pattern8.isNull())
        _pattern8.load(ServiceLocator::get<IAppearanceService>().selector().select(":/misc/pattern8.png"));

    if (_pattern64.isNull())
        _pattern64.load(ServiceLocator::get<IAppearanceService>().selector().select(":/misc/pattern64.png"));
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
    _brushStroke(new BrushStroke(brush, _helper->color(), size, helper->_brushSurface))
{
    _brushStroke->setPreview(true);
}

BrushIconHelper::BrushIconEngine::BrushIconEngine(
    QPointer<const BrushIconHelper> helper,
    BrushId brush
)
    : _helper(helper),
    _brushStroke(new BrushStroke(brush, _helper->color(), 0, helper->_brushSurface)),
    _autoSize(true)
{
    _brushStroke->setPreview(true);
}

QIconEngine* BrushIconHelper::BrushIconEngine::clone() const
{ 
    return new BrushIconEngine(*this);
}

void BrushIconHelper::BrushIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
    if(!_helper) return;

    if (_cache.width() != rect.width() 
        || _cache.height() != rect.height()
        || _cachedColor != _helper->color()
        || _cachedBackground != _helper->background()
        || (!_autoSize && _cachedScale != _helper->scale()))
    {
        paint_p(rect.size());
        
        _cachedColor = _helper->color();
        _cachedScale = _helper->scale();
        _cachedBackground = _helper->background();
    }

    painter->drawPixmap(rect, _cache);
}

void BrushIconHelper::BrushIconEngine::paint_p(QSize iconSize)
{
    if (!_helper) return; // assert

    double size;
    double scale;
    QPointF center;

    IBrushModel::PreviewHints hints = ServiceLocator::get<IBrushModel>(_helper->_brush).previewHints();
    bool copyMode = ServiceLocator::get<IBrushModel>(_helper->_brush).copyMode();

    const QRect iconRect(QPoint(), iconSize);

    double shortside = qMin(iconSize.width(), iconSize.height());

    bool smallIcon = shortside < 24;
    shortside = smallIcon ? shortside - 2 : shortside - 4;

    if (_autoSize)
    {
        scale = 1;
        size = shortside;
        _brushStroke->setSize(size);
    }
    else
    {
        scale = _helper->scale();
        size = _brushStroke->size();
    }

    QRectF frameRect(
        QPointF(
            (iconSize.width() - shortside) / 2,
            (iconSize.height() - shortside) / 2
        ),
        QSizeF(shortside, shortside)
    );

    QRectF canonicalRect = QRectF(
        frameRect.topLeft() / scale,
        QSizeF(shortside, shortside) / scale
    );

    _brushStroke->setColor(_helper->color());

    // This position algorithm assumes that the brush will look good if treated
    // as a circle with r = 1/2 size. Naturally, as brush appearances become
    // more complex, this behavior will become more fine-tuned and customizable
    // with brush parameters.

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

    _cache = QPixmap(iconSize);
    QPainter painter(&_cache);

    _helper->_underSurface->clear();
    if (hints & IBrushModel::Subtractive)
    {
        auto surfaceHandle = _helper->_underSurface->paintHandle(coarseBoundRect(canonicalRect));
        
        surfaceHandle.painter().setPen(Qt::NoPen);

        if (smallIcon)
            surfaceHandle.painter().setBrush(QBrush(_pattern8));
        else 
            surfaceHandle.painter().setBrush(QBrush(_pattern64));
            
        surfaceHandle.painter().setBrushOrigin(canonicalRect.center());
        surfaceHandle.painter().drawRect(coarseBoundRect(canonicalRect));
    }

    if (copyMode) _helper->_brushSurface->link(_helper->_underSurface);
    
    _helper->_brushSurface->clear();
    _brushStroke->clear();
    _brushStroke->moveTo(center);
    _brushStroke->paint();

    _helper->_brushSurface->unlink();

    // TODO: adjust background color for contrast if needed

    painter.setRenderHint(QPainter::Antialiasing, true);

    // if (_helper->background().alpha() < 0xFF)
    // {
    //     if (smallIcon)
    //         _helper->_checkerBoard.setWidth(4);
    //     else
    //         _helper->_checkerBoard.setWidth(12);

    //     _helper->_checkerBoard.setOrigin(iconRect.center());
    //     _helper->_checkerBoard.paint(painter, iconRect);
    // }

    painter.fillRect(iconRect, _helper->background());
    painter.setClipRect(frameRect);
    //painter.translate(frameRect.topLeft());

    if (!_autoSize)
    {
        painter.scale(scale, scale);
    }

    _helper->_renderStack->render(RenderData(iconRect, &painter));
    //render(_helper->_brushSurface->renderStep(), coarseBoundRect(canonicalRect), &painter);
}

QIcon BrushIconHelper::SizeIconProvider::icon(double size) const
{
    return _helper ? _helper->varySize(size) : QIcon();
}
