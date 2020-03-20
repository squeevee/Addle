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
        QImage::Format format = QImage::Format_ARGB32_Premultiplied,
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

    RasterPaintHandle getPaintHandle(QRect area)
    {
        return RasterPaintHandle(*this, _buffer, _bufferOffset, _area);
    }

signals:
    void changed(QRect region);

protected:
    void allocate(QRect area);

private: 

    const int CHUNK_SIZE = 512;
    
    QImage _buffer;
    QPoint _bufferOffset;

    QRect _area;
};

#endif // RASTERSURFACE_HPP