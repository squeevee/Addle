#ifndef BRUSHPAINTERCOMMON_HPP
#define BRUSHPAINTERCOMMON_HPP

#include "interfaces/editing/ibrushpainter.hpp"
#include "interfaces/editing/irastersurface.hpp"
#include "servicelocator.hpp"

class BrushPainterCommon
{
public:
    BrushPainterCommon(IBrushPainter& owner)
        : _owner(owner)
    {
    }

    void initialize(double size, QColor color, QSharedPointer<IRasterSurface> buffer_arg)
    {
        _size = size;
        _color = color;

        if (buffer_arg)
            buffer = buffer_arg;
        else 
            buffer = ServiceLocator::makeShared<IRasterSurface>();
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

#endif // BRUSHPAINTERCOMMON_HPP