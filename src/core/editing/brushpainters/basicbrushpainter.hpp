#ifndef BASICBRUSHPAINTER_HPP
#define BASICBRUSHPAINTER_HPP

#include "interfaces/editing/brushpainters/ibasicbrushpainter.hpp"

class BasicBrushPainter : public IBasicBrushPainter
{
public:
    virtual ~BasicBrushPainter() = default;

    BrushId getId() const { return Id; }

    bool isSizeInvariant() const { return false; }
    bool isColorInvariant() const { return false; }
    bool isAliased() const { return false; }

    double getMinimumSize() const { return DEFAULT_MINIMUM_SIZE; }
    double getMaximumSize() const { return DEFAULT_MAXIMUM_SIZE; }

    QRect boundingRect(const BrushPainterData& segment) const;
    void paint(BrushPainterData& segment, QPainter& painter) const;
};

#endif // BASICBRUSHPAINTER_HPP