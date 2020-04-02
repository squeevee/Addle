#ifndef RASTERSURFACE_HPP
#define RASTERSURFACE_HPP

#include "interfaces/editing/surfaces/irastersurface.hpp"
#include <QObject>

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

    void render(QPainter& painter, QRect region) const;

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
};

#endif // RASTERSURFACE_HPP