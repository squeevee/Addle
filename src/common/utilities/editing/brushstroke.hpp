#ifndef BRUSHSTROKE_HPP
#define BRUSHSTROKE_HPP

#include "compat.hpp"

#include "interfaces/models/ibrush.hpp"

#include <QObject>

#include <QColor>
#include <QSharedPointer>
#include <QPainterPath>

#include <QStack>
namespace Addle {

class IRasterSurface;
class ADDLE_COMMON_EXPORT BrushStroke : public QObject
{
    Q_OBJECT
public:    
    BrushStroke(
        BrushId id,
        QColor color = QColor(),
        double size = 0,
        QSharedPointer<IRasterSurface> buffer = QSharedPointer<IRasterSurface>()
    );
    
    inline BrushId id() const { return _id; }
    inline IBrush& brush() const { return _brush; }
    inline QSharedPointer<IRasterSurface> buffer() const { return _buffer; }

    void conform();

    void save();
    void restore();

    QColor color() const;
    void setColor(QColor color);

    double size() const;
    void setSize(double size);
    
    QRect bound() const;
    void setBound(QRect bound);

    QList<QPointF> positions() const;
    void clear();
    void moveTo(QPointF pos);

    double length() const;

    QPointF lastPositionPainted() const;

    double lastLengthPainted() const;
    inline double lengthSincePaint() const { return length() - lastLengthPainted(); }

    QRect lastPaintedBound() const;
    
    bool isMarkedPainted() const;
    void markPainted();

    bool isPreview() const;
    void setPreview(bool isPreview);

    QVariantHash engineState() const;
    QVariantHash& engineState();

    void paint();

// signals:
//     // ?
//     virtual void colorChanged(QColor color);
//     virtual void sizeChanged(QColor color);

private:
    BrushId _id;
    IBrush& _brush;
    QSharedPointer<IRasterSurface> _buffer;

    struct PainterState
    {
        PainterState() = default;
        PainterState(QColor color_, double size_)
            : color(color_), size(size_)
        {
        }

        QColor color;
        double size = 0;

        QRect bound;

        QList<QPointF> positions;

        double length = 0;

        bool markedPainted = false;
        QPointF lastPositionPainted;
        double lastLengthPainted = 0;

        QRect lastPaintedBound;

        bool isPreview = false;
        QVariantHash engineState;
    };

    QStack<PainterState> _painterStates;
};

} // namespace Addle
#endif // BRUSHSTROKE_HPP