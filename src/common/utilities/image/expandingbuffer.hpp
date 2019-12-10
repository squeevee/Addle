#ifndef EXPANDINGIMAGEBUFFER_HPP
#define EXPANDINGIMAGEBUFFER_HPP

#include <QImage>
#include <QRect>
#include <QTransform>

#include "interfaces/editing/ihavebufferpainter.hpp"
#include "bufferpainter.hpp"

class ExpandingBuffer
{
public:

    ExpandingBuffer(int chunkSize) : _chunkSize(chunkSize) { }

    void initialize(QImage::Format format, QRect start = QRect());

    void grab(QRect area);
    void render(QPainter& painter, QRect paintRegion) const;

    inline bool isNull() const { return _region.isNull(); }
    inline QRect getRegion() const { return _region; }

    inline QSize getSize() const { return _region.size(); }
    inline QTransform getOntoBufferTransform() { return _ontoBufferTransform; }

    void clear();

    BufferPainter createBufferPainter(QRect paintArea, IHaveBufferPainter* owner = nullptr);

    QImage image;

private:
    const int _chunkSize;

    QImage::Format _format;
    QRect _region;
    QTransform _ontoBufferTransform;
};

#endif // EXPANDINGIMAGEBUFFER_HPP