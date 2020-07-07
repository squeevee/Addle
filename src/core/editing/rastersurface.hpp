#ifndef RASTERSURFACE_HPP
#define RASTERSURFACE_HPP

#include "compat.hpp"
#include "interfaces/rendering/irenderstep.hpp"
#include "interfaces/editing/irastersurface.hpp"
#include "utilities/initializehelper.hpp"
#include <QObject>
#include <QReadWriteLock>

class RasterSurfaceRenderStep;
class ADDLE_CORE_EXPORT RasterSurface : public QObject, public IRasterSurface
{
    Q_OBJECT
public:
    RasterSurface() : _initHelper(this) {}
    virtual ~RasterSurface() = default; 

    void initialize(
        QRect area = QRect(),
        QPainter::CompositionMode compositionMode = QPainter::CompositionMode_SourceOver,
        InitFlags flags = None
    ) override;
    void initialize(
        QImage image,
        QPoint offset = QPoint(),
        QPainter::CompositionMode compositionMode = QPainter::CompositionMode_SourceOver,
        InitFlags flags = None
    ) override;

    virtual void setCompositionMode(QPainter::CompositionMode mode) override
    {
        _initHelper.check();
        const QWriteLocker lock(&_lock);
        _compositionMode = mode;
    }

    virtual QPainter::CompositionMode getCompositionMode() const override
    {
        _initHelper.check();
        const QReadLocker lock(&_lock);
        return _compositionMode;
    }

    bool replaceMode() const { _initHelper.check(); return _replaceMode; }
    void setReplaceMode(bool replace) { _initHelper.check(); _replaceMode = replace; }
    
    void link(QSharedPointer<IRasterSurface> other) override
    { 
        const QWriteLocker lock(&_lock);
        _linked = other; 
    }
    void unlink() override
    { 
        const QWriteLocker lock(&_lock);
        _linked.clear(); 
    }

    QRect getArea() const override
    { 
        _initHelper.check();
        const QReadLocker lock(&_lock);
        return _area;
    }

    void clear() override;

    int alpha() const { _initHelper.check(); return _alpha; }
    void setAlpha(int alpha) { _initHelper.check(); _alpha = alpha; emit changed(_area); }

    QSharedPointer<IRenderStep> getRenderStep() override;

    RasterPaintHandle getPaintHandle(QRect handleArea) override
    {
        _lock.lockForWrite(); //unlock in onPaintHandleDestroyed

        allocate(handleArea);
        return getPaintHandle_p(_buffer, _bufferOffset, handleArea);
    }

    RasterBitReader getBitReader(QRect area) const override
    {
        _lock.lockForRead(); //unlock in onBitReaderDestroyed

        QRect readerArea = QRect(_bufferOffset, _buffer.size()).intersected(area);
        return getBitReader_p(_buffer, _bufferOffset, readerArea);
    }

    RasterBitWriter getBitWriter(QRect area)
    {
        _lock.lockForWrite(); //unlock in onBitReaderDestroyed

        allocate(area);
        return getBitWriter_p(_buffer, _bufferOffset, area);
    }

signals:
    void changed(QRect region);

protected:
    void onPaintHandleDestroyed(const RasterPaintHandle& handle) override;
    void onBitReaderDestroyed(const RasterBitReader& handle) const override;
    void onBitWriterDestroyed(const RasterBitWriter& handle) override;

private: 
    void allocate(QRect allocArea);
    void copyLinked();

    const int CHUNK_SIZE = 64;

    mutable QReadWriteLock _lock;
    
    QSharedPointer<IRasterSurface> _linked;
    QSharedPointer<IRenderStep> _renderStep;

    QPainter::CompositionMode _compositionMode = (QPainter::CompositionMode)NULL;
    int _alpha = 0xFF;
    bool _replaceMode = false;

    QImage _buffer;
    QPoint _bufferOffset;

    QRect _area;

    InitializeHelper<RasterSurface> _initHelper;

    friend class RasterSurfaceRenderStep;
};

class RasterSurfaceRenderStep : public QObject, public IRenderStep
{
    Q_OBJECT 
    Q_INTERFACES(IRenderStep)
public: 
    RasterSurfaceRenderStep(RasterSurface& owner) : _owner(owner) { }
    virtual ~RasterSurfaceRenderStep() = default;

    virtual void onPush(RenderData& data) override;
    virtual void onPop(RenderData& data) override;

    virtual QRect getAreaHint() override { return _owner._area; }

signals: 
    void changed(QRect area);

private:
    RasterSurface& _owner;
};

#endif // RASTERSURFACE_HPP