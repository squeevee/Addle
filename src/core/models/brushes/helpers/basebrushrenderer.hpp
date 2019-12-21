#ifndef BASEBRUSHRENDERER_HPP
#define BASEBRUSHRENDERER_HPP

#include "interfaces/models/brushes/ibrush.hpp"

class BaseBrushRenderer : public IBrush::IBrushRenderer
{
public:
    BaseBrushRenderer(IBrush& owner)
        : _owner(owner)
    {
    }

    BaseBrushRenderer(IBrush& owner, double size, QPointF position, QColor color)
        : _owner(owner), _size(size), _position(position), _color(color)
    {
    }

    IBrush& getOwner() { return _owner; }

    double getSize() { return _size; }
    void setSize(double size) { _size = size; }

    QPointF getPosition() { return _position; }
    void setPosition(QPointF pos) { _position = pos; }

    QColor getColor() { return _color; }
    void setColor(QColor color) { _color = color; }

private: 
    double _size = 0.0;
    QPointF _position;
    QColor _color;

    IBrush& _owner;
};

#endif // BASEBRUSHRENDERER_HPP