#ifndef BUFFERPAINTER_HPP
#define BUFFERPAINTER_HPP

#include <QSharedData>
#include <QSharedDataPointer>

#include <QRect>
#include <QImage>
#include <QPainter>

class IHaveBufferPainter;
class BufferPainter
{
public:
    BufferPainter(QImage& buffer, IHaveBufferPainter* owner = nullptr, QRect paintArea = QRect(), QPoint bufferOffset = QPoint());
    BufferPainter(BufferPainter&& other);
    ~BufferPainter();

    QPainter& getPainter();

private:
    QPoint _bufferOffset;
    QRect _paintArea;
    QPainter* _painter = nullptr;

    IHaveBufferPainter* _owner;
    QImage& _buffer;

    bool _final = true;
};

#endif // BUFFERPAINTER_HPP