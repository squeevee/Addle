#pragma once

#include <QImage>
#include <QAtomicInt>
#include <QSharedData>

#ifdef ADDLE_DEBUG
#include "utilities/debugging/qdebug_extensions.hpp"
#endif 

namespace Addle {

namespace aux_RasterTile {
    
constexpr std::size_t TILE_SIDE_LENGTH = 256;
constexpr QSize TILE_SIZE = QSize(TILE_SIDE_LENGTH, TILE_SIDE_LENGTH);

Q_DECL_PURE_FUNCTION
std::size_t calcBufferLength(QImage::Format format);

}

// Replacement for aux_tilesurface::Tile
//
// A chunk of readable and writable raster data comprising a fixed-size square 
// portion of IRasterSurface2, and managed as implicitly shared data. The
// contents of a tile can be accessed and modified as a QImage using `asImage()`
// (or the QImage conversion operator), or directly as bytes with `buffer()`.
//
// The location of the tile in its surface is given by `offset()` in canvas
// coordinates. 
//
// TODO: Currently a tile's raster data can only be stored as an in-memory 
// uncompressed image buffer. I anticipate in the future it will be desirable to 
// extend RasterTile to support optimizations for one or possibly more special 
// cases:
// - "Differencing" raster data in the undo/redo stack will probably be modeled 
// as RasterTile and would benefit from being compressed (and possibly even 
// offloaded to the disk) when not in active use.
// - Some image formats should eventually be supported such that when they are 
// loaded from a file (and not edited), Addle can memory-map the file and 
// decode it on demand. This would in particular allow large images to be 
// displayed without requiring the whole thing to be decoded/decompressed in 
// memory at full resolution all at once.
//   (This feature's relation to RasterTile is uncertain, since RasterTile 
//   does not and should not have the ability to signal higher quality 
//   interlaced data being available -- that's more of an IRenderer thing -- 
//   but some support corroboration by RasterTile could allow for such an 
//   image to be *edited* with relative efficiency, such that modified tiles 
//   are decoded in memory, but unmodified tiles still load from the file on 
//   demand.)
class RasterTile
{
public:
    static constexpr std::size_t SIDE_LENGTH = aux_RasterTile::TILE_SIDE_LENGTH;
    
    RasterTile() = default;
    RasterTile(const RasterTile&) = default;
    RasterTile(RasterTile&&) = default;
    
    RasterTile& operator=(const RasterTile&) = default;
    RasterTile& operator=(RasterTile&&) = default;
    
    RasterTile(QImage::Format format, QPoint offset, 
               QVector<QRgb> colorTable = QVector<QRgb>())
        : _data(new Data(format, (offset / (int)SIDE_LENGTH), colorTable))
    {
#ifdef ADDLE_DEBUG
        if (Q_UNLIKELY((offset.x() % SIDE_LENGTH) || (offset.y() % SIDE_LENGTH)))
        {
            qWarning("%s", qUtf8Printable(
                //% "Non-aligned tile offset (%1) will be truncated."
                qtTrId("debug-messages.raster-tile.offset-truncated")
                    .arg(aux_debug::debugString(offset))
            ));
        }
#endif
    }
    
    bool isNull() const { return !_data; }
    
    QPoint offset() const { return _data ? (_data->gridOffset * (int)SIDE_LENGTH): QPoint(); }
    int x() const { return _data ? _data->gridOffset.x() * SIDE_LENGTH : 0; }
    int y() const { return _data ? _data->gridOffset.y() * SIDE_LENGTH : 0; }
    QRect rect() const { return QRect(offset(), aux_RasterTile::TILE_SIZE); }
    
    void setOffset(QPoint offset)
    {
#ifdef ADDLE_DEBUG
        if (Q_UNLIKELY((offset.x() % SIDE_LENGTH) || (offset.y() % SIDE_LENGTH)))
        {
            qWarning("%s", qUtf8Printable(
                qtTrId("debug-messages.raster-tile.offset-truncated")
                    .arg(aux_debug::debugString(offset))
            ));
        }
#endif
        _data->gridOffset = (offset / (int)SIDE_LENGTH);
    }
    
    QImage::Format format() const { return _data ? _data->format : (QImage::Format)(NULL); }
    QVector<QRgb> colorTable() const { return _data ? _data->colorTable : QVector<QRgb>(); }
    
    // If the tile is not null, and the tile does not already have a buffer 
    // this will initialize an empty buffer.
    QImage asImage()
    { 
        if (!_data) return QImage();
        
        _data->detachBuffer(); 
        return asImage_p(); 
    }
    operator QImage() { return asImage(); }
    
    const QImage asImage() const { return _data ? asImage_p() : QImage(); }
    operator const QImage() const { return asImage(); }
    
    bool hasBuffer() const { return _data && _data->buffer; }
    
    // If the tile is not null, and the tile does not already have a buffer 
    // this will initialize an empty buffer.
    uchar* buffer()
    { 
        if (!_data) return nullptr;
        
        _data->detachBuffer(); 
        return _data->buffer->data; 
    }
    
    const uchar* buffer() const { return hasBuffer() ? _data->buffer->data : nullptr; }
    std::size_t bufferLength() const { return hasBuffer() ? aux_RasterTile::calcBufferLength(_data->format) : 0; }
    
    // By default, a tile's buffer is allocated with all bytes set to 0x00. 
    // This function causes a buffer to be allocated with undefined contents.
    // This will be faster if the user does not need the buffer to be initially 
    // zeroed (e.g., the bytes will be set to another value, or will be copied 
    // from elsewhere), but the default behavior is likely to be faster than 
    // calling this function and zeroing the bytes manually.
    //
    // The tile must not be null and must not have a buffer already.
    void initRawBuffer();
    
private:
    struct Buffer
    {
        QAtomicInt ref;
        uchar* data = nullptr;
    };
    
    struct Data : QSharedData
    {
        Data() = default;
        Data(const Data& other)
            : gridOffset(other.gridOffset), format(other.format), 
            colorTable(other.colorTable), buffer(other.buffer)
        {
            if (buffer)
                buffer->ref.ref();
        }
        
        Data(QImage::Format format_, QPoint gridOffset_, QVector<QRgb> colorTable_)
            : gridOffset(gridOffset_), format(format_), colorTable(colorTable_)
        {
        }
        
        ~Data() { disposeBuffer(); }
        
        void detachBuffer();
        void disposeBuffer();
        
        QPoint gridOffset;
        QImage::Format format;
        QVector<QRgb> colorTable;
        
        Buffer* buffer = nullptr;
        
        // QByteArray compressedData;
    };
    
    static void cleanupTileQImage(void*);
    QImage asImage_p() const;
    
    QSharedDataPointer<Data> _data;
};
    
}

Q_DECLARE_TYPEINFO(Addle::RasterTile, Q_MOVABLE_TYPE);
