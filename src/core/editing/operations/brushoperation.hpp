#ifndef BRUSHOPERATION_HPP
#define BRUSHOPERATION_HPP

#include "interfaces/editing/operations/ibrushoperation.hpp"

#include "utilities/initializehelper.hpp"
#include "utilities/image/expandingbuffer.hpp"
#include "utilities/image/imagecompressor.hpp"

#include <QPen>
#include <QBrush>
#include <QLineF>
#include <QPainterPath>

class BrushOperation : public QObject, public IBrushOperation
{
    Q_OBJECT
public:
    BrushOperation() : _initHelper(this), _workingBuffer(BUFFER_CHUNK_SIZE) { }
    virtual ~BrushOperation() = default;

    void initialize(
        QWeakPointer<ILayer>& layer,
        QSharedPointer<IBrushRenderer> brushRenderer,
        Mode mode = Mode::paint
    );

    void addPathSegment(const BrushPathSegment& pathSegment);

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

    InitializeHelper<BrushOperation> _initHelper;
};

#endif // BRUSHOPERATION_HPP