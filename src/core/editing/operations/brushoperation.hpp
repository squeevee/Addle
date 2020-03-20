#ifndef BRUSHOPERATION_HPP
#define BRUSHOPERATION_HPP

#include "interfaces/editing/operations/ibrushoperation.hpp"
#include "interfaces/editing/surfaces/irastersurface.hpp"

#include "utilities/initializehelper.hpp"
#include "utilities/image/imagecompressor.hpp"

#include <QPen>
#include <QBrush>
#include <QLineF>
#include <QPainterPath>

class BrushOperation : public QObject, public IBrushOperation
{
    Q_OBJECT
public:
    BrushOperation() : _initHelper(this) { }
    virtual ~BrushOperation() = default;

    void initialize(
        QWeakPointer<ILayer>& layer,
        QSharedPointer<IBrushPainter> brushPainter,
        Mode mode = Mode::paint
    );

    void addPainterData(BrushPainterData& painterData);

    void render(QPainter& painter, QRect region);

    QRect getBoundingRect() { return _areaOfEffect; }

    void doOperation();
    void undoOperation();

private:

    void freeze(QImage* forwardPtr = nullptr, QImage* reversePtr = nullptr, QImage* maskPtr = nullptr);

    QWeakPointer<ILayer> _layer;
    Mode _mode;
    QImage::Format _format;

    bool _frozen = false;

    QRect _areaOfEffect;
    QSharedPointer<IBrushPainter> _brushPainter;

    QSharedPointer<IRasterSurface> _workingSurface;

    ImageCompressor _forwardStored;
    ImageCompressor _reverseStored;
    ImageCompressor _maskStored;

    InitializeHelper<BrushOperation> _initHelper;
};

#endif // BRUSHOPERATION_HPP