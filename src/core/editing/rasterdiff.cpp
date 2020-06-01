#include "rasterdiff.hpp"
#include <cstring>

#include "interfaces/editing/irastersurface.hpp"
#include "interfaces/rendering/irenderstack.hpp"

#include "utilities/render/renderutils.hpp"

#include "servicelocator.hpp"

void RasterDiff::initialize(
        /*const*/ IRasterSurface& source,
        QWeakPointer<IRasterSurface> destination
    )
{
    _area = source.getArea();
    _destination = destination;

    auto s_destination = _destination.toStrongRef();

    int bufferLength = _area.width() * _area.height() * PIXEL_DEPTH;
    
    _uncompressed = QByteArray(bufferLength, 0x00);
    {
        QImage mergedBuffer(
            reinterpret_cast<uchar*>(_uncompressed.data()), 
            _area.width(),
            _area.height(),
            QImage::Format_ARGB32_Premultiplied
        );
        QPainter painter(&mergedBuffer);
        painter.translate(-_area.topLeft());
        render({ s_destination->getRenderStep(), source.getRenderStep() }, _area, &painter);
    }

    {
        RasterBitReader surfaceReader = s_destination->getBitReader(_area);

        if (surfaceReader.area().isNull()) return;
        
        //assert surfaceReader.pixelWidth() == PIXEL_DEPTH
        const int padTop = surfaceReader.area().top() - _area.top();
        const int height = surfaceReader.area().height();

        const int padLeft = (surfaceReader.area().left() - _area.left()) * PIXEL_DEPTH;
        const int readerWidth = surfaceReader.area().width() * PIXEL_DEPTH;
        const int fullWidth = _area.width() * PIXEL_DEPTH;

        int line = 0;

        while (line < height)
        {
            int column = 0;

            uchar* merged = reinterpret_cast<uchar*>(_uncompressed.data()) + (fullWidth * (line + padTop)) + padLeft;
            const uchar* surface = surfaceReader.scanLine(line);

            while (column < readerWidth)
            {
                *merged++ ^= *surface++;
                column++;
            }

            line++;
        }
    }
}

void RasterDiff::apply()
{
    if (!_destination) return;
    auto s_destination = _destination.toStrongRef();

    RasterBitWriter writer = s_destination->getBitWriter(_area);
    //assert surfaceReader.pixelWidth() == PIXEL_DEPTH
    
    {
        const int width = _area.width() * PIXEL_DEPTH;
        const int height = _area.height();

        int line = 0;
        
        while (line < height)
        {
            int column = 0;

            uchar* surface = writer.scanLine(line);
            const uchar* diff = reinterpret_cast<const uchar*>(_uncompressed.data()) + (width * line);

            while (column < width)
            {
                *surface++ ^= *diff++;
                column++;
            }

            line++;
        }
    }
}
