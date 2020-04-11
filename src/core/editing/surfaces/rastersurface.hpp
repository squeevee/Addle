#ifndef RASTERSURFACE_HPP
#define RASTERSURFACE_HPP

#include "interfaces/rendering/irenderstep.hpp"
#include "interfaces/editing/surfaces/irastersurface.hpp"
#include "utilities/initializehelper.hpp"
#include <QObject>
#include <QReadWriteLock>

class RasterSurfaceRenderStep;
class RasterSurface : public QObject, public IRasterSurface
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
    QSize getSize() const override
    { 
        _initHelper.check();
        const QReadLocker lock(&_lock);
        return _area.size();
    }

    QImage copy(QRect area, QPoint* offset) const override;

    void clear() override;

    QSharedPointer<IRenderStep> getRenderStep() override;

    RasterPaintHandle getPaintHandle(QRect handleArea) override
    {
        _lock.lockForWrite(); //unlock in onHandleDestroyed

        allocate(handleArea);
        return getPaintHandle_p(_buffer, _bufferOffset, handleArea);
    }
    
    void merge(IRasterSurface& other) override;

signals:
    void changed(QRect region);

protected:
    void onHandleDestroyed(const RasterPaintHandle& handle) override;

private: 
    void allocate(QRect allocArea);

    const int CHUNK_SIZE = 512;

    mutable QReadWriteLock _lock;
    
    QSharedPointer<IRasterSurface> _linked;
    QSharedPointer<IRenderStep> _renderStep;

    QImage::Format _format = QImage::Format_Invalid;
    QPainter::CompositionMode _compositionMode = (QPainter::CompositionMode)NULL;

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