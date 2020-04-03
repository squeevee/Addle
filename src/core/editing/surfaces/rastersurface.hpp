#ifndef RASTERSURFACE_HPP
#define RASTERSURFACE_HPP

#include "interfaces/rendering/irenderstep.hpp"
#include "interfaces/editing/surfaces/irastersurface.hpp"
#include <QObject>

class RasterSurface;
class RasterSurfaceRenderStep : public QObject, public IRenderStep
{
    Q_OBJECT 
    Q_INTERFACES(IRenderStep)
public: 
    RasterSurfaceRenderStep(RasterSurface& owner) : _owner(owner) { }
    virtual ~RasterSurfaceRenderStep() = default;

    virtual void before(RenderData& data) { }
    virtual void after(RenderData& data);

signals: 
    void changed(QRect area);

private:
    RasterSurface& _owner;
};

class RasterSurface : public QObject, public IRasterSurface
{
    Q_OBJECT
public: 
    virtual ~RasterSurface() = default; 

    void initialize(
        QRect area = QRect(),
        InitFlags flags = None
    );
    void initialize(
        QImage image,
        InitFlags flags = None
    );

    QRect getArea() const { return _area; }
    QSize getSize() const { return _area.size(); }

    QImage copy(QRect area, QPoint* offset = nullptr) const;

    IRenderStep* makeRenderStep() { return new RasterSurfaceRenderStep(*this); }

    RasterPaintHandle getPaintHandle(QRect handleArea)
    {
        allocate(handleArea);
        return getPaintHandle_p(_buffer, _bufferOffset, handleArea);
    }
    
    void merge(IRasterSurface& other);

signals:
    void changed(QRect region);

protected:
    void onHandleDestroyed(const RasterPaintHandle& handle) { emit changed(handle.getArea()); }

private: 
    void allocate(QRect allocArea);

    const int CHUNK_SIZE = 512;
    
    QImage::Format _format = QImage::Format_Invalid;

    QImage _buffer;
    QPoint _bufferOffset;

    QRect _area;

    friend class RasterSurfaceRenderStep;
};

#endif // RASTERSURFACE_HPP