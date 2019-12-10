#ifndef IHAVEBUFFERPAINTER_HPP
#define IHAVEBUFFERPAINTER_HPP

#include "utilities/image/bufferpainter.hpp"

class IHaveBufferPainter 
{
public: 
    virtual BufferPainter getBufferPainter(QRect region) = 0;

protected:
    virtual void beforeBufferPainted(QRect region) { }
    virtual void afterBufferPainted(QRect region) { }

    friend class BufferPainter;
};

#endif // IHAVEBUFFERPAINTER_HPP