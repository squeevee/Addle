/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef RASTERSURFACE_HPP
#define RASTERSURFACE_HPP

#include "compat.hpp"
#include "interfaces/rendering/irenderstep.hpp"
#include "interfaces/editing/irastersurface.hpp"
#include "utilities/initializehelper.hpp"
#include <QObject>
#include <QReadWriteLock>
namespace Addle {

class RasterSurfaceRenderStep;
class ADDLE_CORE_EXPORT RasterSurface : public QObject, public IRasterSurface
{
    Q_OBJECT
    IAMQOBJECT_IMPL
public:
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
        ASSERT_INIT();
        const QWriteLocker lock(&_lock);
        _compositionMode = mode;
    }

    virtual QPainter::CompositionMode compositionMode() const override
    {
        ASSERT_INIT();
        const QReadLocker lock(&_lock);
        return _compositionMode;
    }

    bool replaceMode() const { ASSERT_INIT(); return _replaceMode; }
    void setReplaceMode(bool replace) { ASSERT_INIT(); _replaceMode = replace; }
    
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

    QRect area() const override
    { 
        ASSERT_INIT();
        const QReadLocker lock(&_lock);
        return _area;
    }

    void clear() override;

    int alpha() const { ASSERT_INIT(); return _alpha; }
    void setAlpha(int alpha) { ASSERT_INIT(); _alpha = alpha; emit changed(_area); }

    QSharedPointer<IRenderStep> renderStep() override;

    RasterPaintHandle paintHandle(QRect handleArea) override
    {
        _lock.lockForWrite(); //unlock in onPaintHandleDestroyed

        allocate(handleArea);
        return paintHandle_p(_buffer, _bufferOffset, handleArea);
    }

    RasterBitReader bitReader(QRect area) const override
    {
        _lock.lockForRead(); //unlock in onBitReaderDestroyed

        QRect readerArea = QRect(_bufferOffset, _buffer.size()).intersected(area);
        return bitReader_p(_buffer, _bufferOffset, readerArea);
    }

    RasterBitWriter bitWriter(QRect area)
    {
        _lock.lockForWrite(); //unlock in onBitReaderDestroyed

        allocate(area);
        return bitWriter_p(_buffer, _bufferOffset, area);
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

    InitializeHelper _initHelper;

    friend class RasterSurfaceRenderStep;
};

class RasterSurfaceRenderStep : public QObject, public IRenderStep
{
    Q_OBJECT
    IAMQOBJECT_IMPL
public: 
    RasterSurfaceRenderStep(RasterSurface& owner) : _owner(owner) { }
    virtual ~RasterSurfaceRenderStep() = default;

    virtual void onPush(RenderData& data) override;
    virtual void onPop(RenderData& data) override;

    virtual QRect areaHint() override { return _owner._area; }

signals: 
    void changed(QRect area);

private:
    RasterSurface& _owner;
};

} // namespace Addle

#endif // RASTERSURFACE_HPP