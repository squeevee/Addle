#include <cstring>
#include <cstdlib>

#include <QPixelFormat>

#include "rastertile.hpp"

using namespace Addle;

std::size_t Addle::aux_RasterTile::calcBufferLength(QImage::Format format)
{
    static_assert(TILE_SIDE_LENGTH % 32 == 0);
    // Inconvenient values of TILE_SIDE_LENGTH may require some extra logic to 
    // meet padding and alignment requirements. This is unlikely to be needed so 
    // it is not currently implemented.
    
    return (TILE_SIDE_LENGTH * TILE_SIDE_LENGTH / 8) * QImage::toPixelFormat(format).bitsPerPixel();
}

void RasterTile::initRawBuffer()
{
    assert(_data && !_data->buffer);
    
    Q_UNIMPLEMENTED();
}

void RasterTile::Data::detachBuffer()
{
    if (buffer)
    {
        if (buffer->ref.loadRelaxed() != 1)
        {
            std::size_t length = aux_RasterTile::calcBufferLength(format);
            uchar* newData = (uchar*) std::malloc(length);
            std::memcpy(newData, buffer->data, length);
            
            buffer->ref.deref();
            buffer = new Buffer { 1, newData };
        }
    }
    else
    {
        std::size_t length = aux_RasterTile::calcBufferLength(format);
        uchar* data = (uchar*) std::calloc(length, 1);
        
        buffer = new Buffer { 1, data };
    }
}

void RasterTile::Data::disposeBuffer()
{
    assert(buffer);
    if (!buffer->ref.deref())
    {
        std::free(buffer->data);
        delete buffer;
        buffer = nullptr;
    }
}

void RasterTile::cleanupTileQImage(void* buffer_)
{
    assert(buffer_);
    auto buffer = static_cast<Buffer*>(buffer_);
    
    if (!buffer->ref.deref())
    {
        std::free(buffer->data);
        delete buffer;
    }
}
    
QImage RasterTile::asImage_p() const
{    
    assert(_data);
    
    _data->ref.ref(); // The image occupies one reference count on this data.
    
    QImage result(_data->buffer->data, SIDE_LENGTH, SIDE_LENGTH, _data->format, 
        &cleanupTileQImage, _data->buffer);
    result.setOffset(offset());
    result.setColorTable(_data->colorTable);
    
    return result;
}
