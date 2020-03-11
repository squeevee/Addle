#ifndef BUFFERPAINTER_HPP
#define BUFFERPAINTER_HPP

#include <QObject>
#include <QSharedData>
#include <QSharedDataPointer>

#include <QRect>
#include <QImage>
#include <QPainter>

class BufferPainterNotifier;
class ICanPaintBuffer;
class BufferPainter
{
public:
    BufferPainter(QImage& buffer, QRect paintArea = QRect(), QPoint bufferOffset = QPoint());
    BufferPainter(BufferPainter&& other);
    ~BufferPainter();

    QPainter& getPainter();

    BufferPainterNotifier* getNotifier() const { return _notifier; }

private:
    QPoint _bufferOffset;
    QRect _paintArea;
    QPainter* _painter = nullptr;

    BufferPainterNotifier* _notifier;

    QImage& _buffer;

    bool _final = true;
};

class BufferPainterNotifier : public QObject 
{
    Q_OBJECT
signals:
    void painting(QRect paintArea);
    void painted(QRect paintArea);
};

#endif // BUFFERPAINTER_HPP