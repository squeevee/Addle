#ifndef COMMONBRUSHPAINTERHELPER_HPP
#define COMMONBRUSHPAINTERHELPER_HPP

#include "interfaces/editing/ibrushpainter.hpp"

class CommonBrushPainterHelper
{
public:
    CommonBrushPainterHelper(IBrushPainter& owner)
        : _owner(owner)
    {
    }

    void initialize(QSharedPointer<IRasterSurface> buffer_arg, double size, QColor color)
    {
        buffer = buffer_arg;
        _size = size;
        _color = color;
    }

    QSharedPointer<IRasterSurface> buffer;
    bool isPreview = false;

    inline double getSize() const { return _size; }
    inline void setSize(double size)
    {
        if (_size != size)
        {
            _size = size;
            emit _owner.sizeChanged(_size);
        }
    }

    inline QColor getColor() const { return _color; }
    inline void setColor(QColor color)
    {
        if (_color != color)
        {
            _color = color;
            emit _owner.colorChanged(_color);
        }
    }

private:
    double _size = 0;
    QColor _color;

    IBrushPainter& _owner;
};

#endif // COMMONBRUSHPAINTERHELPER_HPP