#ifndef BASICBRUSHPAINTER_HPP
#define BASICBRUSHPAINTER_HPP

#include "interfaces/editing/ibrushpainter.hpp"

class BasicBrushPainter : public IBrushPainter
{
public:
    virtual ~BasicBrushPainter() = default;

    BrushId getId() const { return CoreBrushes::BasicBrush; }

    bool isSizeInvariant() const { return false; }
    bool isColorInvariant() const { return false; }
    bool isAliased() const { return false; }

    double getMinimumSize() const { return DEFAULT_MINIMUM_SIZE; }
    double getMaximumSize() const { return DEFAULT_MAXIMUM_SIZE; }

    QRect boundingRect(const BrushPainterData& data) const;
    void paint(BrushPainterData& data, QImage& buffer) const;
};

#endif // BASICBRUSHPAINTER_HPP