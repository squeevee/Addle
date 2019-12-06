#ifndef RASTEROPERATION_HPP
#define RASTEROPERATION_HPP

#include "interfaces/editing/irasteroperation.hpp"

#include "utilities/initializehelper.hpp"

#include <QPen>
#include <QBrush>
#include <QLineF>
#include <QPainterPath>

class RasterOperation : public QObject, public IRasterOperation
{
    Q_OBJECT
public:
    RasterOperation() : _initHelper(this) { }
    virtual ~RasterOperation() = default;

    void initialize(
        const QWeakPointer<ILayer>& layer,
        Mode mode = Mode::paint
    );

    std::unique_ptr<QPainter> getBufferPainter(QRect region);
    void render(QPainter& painter, QRect region);

    QRect getBoundingRect() { return _areaOfEffect; }

    void doOperation();
    void undoOperation();

private:
    static const int BUFFER_CHUNK_SIZE = 10;

    QWeakPointer<ILayer> _layer;
    Mode _mode;
    QImage::Format _format;

    bool _frozen = false;

    QRect _bufferRegion;
    QRect _areaOfEffect;

    QImage _buffer;

    void autoSizeBuffer(QRect criticalArea);

    QByteArray _forwardData;
    QByteArray _backwardData;

    InitializeHelper<RasterOperation> _initHelper;

};

#endif // RASTEROPERATION_HPP