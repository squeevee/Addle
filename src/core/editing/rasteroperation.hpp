#ifndef RASTEROPERATION_HPP
#define RASTEROPERATION_HPP

#include "interfaces/editing/irasteroperation.hpp"

#include "utilities/initializehelper.hpp"
#include "utilities/image/expandingbuffer.hpp"
#include "utilities/image/imagecompressor.hpp"

#include <QPen>
#include <QBrush>
#include <QLineF>
#include <QPainterPath>

class RasterOperation : public QObject, public IRasterOperation
{
    Q_OBJECT
public:
    RasterOperation() : _initHelper(this), _workingBuffer(BUFFER_CHUNK_SIZE) { }
    virtual ~RasterOperation() = default;

    void initialize(
        const QWeakPointer<ILayer>& layer,
        Mode mode = Mode::paint
    );

    BufferPainter getBufferPainter(QRect paintArea);
    void render(QPainter& painter, QRect region);

    QRect getBoundingRect() { return _areaOfEffect; }

    void doOperation();
    void undoOperation();

protected:
    void beforeBufferPainted(QRect region);

private:
    static const int BUFFER_CHUNK_SIZE = 512;

    void freeze(QImage* forwardPtr = nullptr, QImage* reversePtr = nullptr, QImage* maskPtr = nullptr);

    QWeakPointer<ILayer> _layer;
    Mode _mode;
    QImage::Format _format;

    bool _frozen = false;

    QRect _areaOfEffect;

    ExpandingBuffer _workingBuffer;

    ImageCompressor _forwardStored;
    ImageCompressor _reverseStored;
    ImageCompressor _maskStored;

    InitializeHelper<RasterOperation> _initHelper;
};

#endif // RASTEROPERATION_HPP