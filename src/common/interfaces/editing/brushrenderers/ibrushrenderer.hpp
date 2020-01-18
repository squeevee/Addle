#ifndef IBRUSHRENDERER_HPP
#define IBRUSHRENDERER_HPP

#include <QPainter>
#include <QImage>

#include "data/brushid.hpp"
#include "data/brushpathsegment.hpp"

class IBrushRenderer
{
public: 
    virtual ~IBrushRenderer() = default;

    virtual BrushId getId() const = 0;

    virtual bool isSizeInvariant() const = 0;
    virtual bool isColorInvariant() const = 0;
    virtual bool isAliased() const = 0;

    static constexpr double DEFAULT_MINIMUM_SIZE = 0.01;
    static constexpr double DEFAULT_MAXIMUM_SIZE = 1000.0;

    virtual double getMinimumSize() const = 0;
    virtual double getMaximumSize() const = 0;

    virtual void renderSegment(QImage& image, const BrushPathSegment& state) const = 0;
};

#endif // IBRUSHRENDERER_HPP