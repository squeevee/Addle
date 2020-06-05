#ifndef BRUSHSTROKE_HPP
#define BRUSHSTROKE_HPP

#include "compat.hpp"

#include "interfaces/models/ibrushmodel.hpp"
#include "utilities/canvas/brushinfo.hpp"

#include <QObject>

#include <QColor>
#include <QSharedPointer>
#include <QPainterPath>

#include <QStack>

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
    
    inline IBrushModel& brush() const { return _brush; }
    inline QSharedPointer<IRasterSurface> getBuffer() const { return _buffer; }

    void conform();

    void save();
    void restore();

    QColor color() const;
    void setColor(QColor color);

    double getSize() const;
    void setSize(double size);
    
    QList<QPointF> positions() const;
    void clear();
    void moveTo(QPointF pos);

    bool isPreview() const;
    void setPreview(bool isPreview);

    QVariantHash engineState() const;
    QVariantHash& engineState();

// signals:
//     // ?
//     virtual void colorChanged(QColor color);
//     virtual void sizeChanged(QColor color);

private:
    IBrushModel& _brush;
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

        QList<QPointF> positions;

        bool isPreview = false;
        QVariantHash engineState;
    };

    QStack<PainterState> _painterStates;
};

#endif // BRUSHSTROKE_HPP