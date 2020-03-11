#ifndef ICANPAINTBUFFER_HPP
#define ICANPAINTBUFFER_HPP

#include "utilities/image/bufferpainter.hpp"

class ICanPaintBuffer
{
public: 
    virtual BufferPainter makeBufferPainter(QRect region) = 0;
};

#endif // ICANPAINTBUFFER_HPP