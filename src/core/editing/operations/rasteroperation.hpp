#ifndef RASTEROPERATION_HPP
#define RASTEROPERATION_HPP

#include "interfaces/editing/operations/irasteroperation.hpp"

class RasterOperation : public IRasterOperation
{
public: 
    virtual ~RasterOperation() = default;

    void initialize(QWeakPointer<IRasterSurface> surface, QImage data, QPoint offset, Mode mode);

    void do_();
    void undo();

private: 
    QWeakPointer<IRasterSurface> _surface;
    QPoint _offset;
    QImage _data;
    QRect _dataRect;

    QByteArray _xor;

    Mode _mode = (Mode)NULL;
};

#endif // RASTEROPERATION_HPP