#ifndef RASTERSURFACE_HPP
#define RASTERSURFACE_HPP

#include "interfaces/editing/surfaces/irastersurface.hpp"
#include <QObject>

#include "utilities/image/expandingbuffer.hpp"

class RasterSurface : public QObject, public IRasterSurface
{
    Q_OBJECT
public: 
    RasterSurface() : _buffer(512) { }
    virtual ~RasterSurface() = default; 

    void render(QPainter& painter, QRect region);

    BufferPainter makeBufferPainter(QRect region);

signals:
    void changed(QRect region);

private slots:
    void onBufferPainted(QRect region);

private: 
    ExpandingBuffer _buffer;
};

#endif // RASTERSURFACE_HPP