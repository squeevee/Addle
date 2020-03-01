#ifndef BASICBRUSHRENDERER_HPP
#define BASICBRUSHRENDERER_HPP

#include "interfaces/editing/brushrenderers/ibasicbrushrenderer.hpp"

class BasicBrushRenderer : public IBasicBrushRenderer
{
public:
    virtual ~BasicBrushRenderer() = default;

    BrushId getId() const { return Id; }

    bool isSizeInvariant() const { return false; }
    bool isColorInvariant() const { return false; }
    bool isAliased() const { return false; }

    double getMinimumSize() const { return DEFAULT_MINIMUM_SIZE; }
    double getMaximumSize() const { return DEFAULT_MAXIMUM_SIZE; }

    QRect getSegmentBoundingRect(const BrushPathSegment& segment) const;
    void renderSegment(QImage& image, BrushPathSegment& state) const;
};

#endif // BASICBRUSHRENDERER_HPP