#ifndef IBRUSHPAINTER_HPP
#define IBRUSHPAINTER_HPP

#include <QImage>

#include "idtypes/brushid.hpp"
#include "utilities/canvas/brushpainterdata.hpp"

#include "interfaces/traits/by_id_traits.hpp"

class IBrushPainter
{
public: 
    virtual ~IBrushPainter() = default;

    virtual BrushId getId() const = 0;

    virtual bool isSizeInvariant() const = 0;
    virtual bool isColorInvariant() const = 0;
    virtual bool isAliased() const = 0;

    static constexpr double DEFAULT_MINIMUM_SIZE = 0.01;
    static constexpr double DEFAULT_MAXIMUM_SIZE = 1000.0;

    virtual double getMinimumSize() const = 0;
    virtual double getMaximumSize() const = 0;

    virtual QRect boundingRect(const BrushPainterData& data) const = 0;
    virtual void paint(BrushPainterData& data, QImage& buffer) const = 0;

    struct CoreBrushes
    {
        static const BrushId BasicBrush;
    };
};

DECL_MAKEABLE_BY_ID(IBrushPainter, BrushId);

#endif // IBRUSHPAINTER_HPP