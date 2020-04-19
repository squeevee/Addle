#ifndef BRUSHPAINTERINFO_HPP
#define BRUSHPAINTERINFO_HPP

#include <QSharedData>

class BrushPainterInfo
{
    struct BrushPainterInfoInner : QSharedData
    {
        BrushPainterInfoInner() = default;
        BrushPainterInfoInner(
            bool isSizeInvariant,
            bool isColorInvariant,
            bool isPixelAliased,
            double minSize,
            double maxSize
        )
            : isSizeInvariant(isSizeInvariant),
            isColorInvariant(isColorInvariant),
            isPixelAliased(isPixelAliased),
            minSize(minSize),
            maxSize(maxSize)
        {
        }

        bool isSizeInvariant = false;
        bool isColorInvariant = false;
        bool isPixelAliased = false;

        double minSize = 0;
        double maxSize = Q_INFINITY;
    };
public:
    BrushPainterInfo()
        : _data(new BrushPainterInfoInner)
    {
    }

    BrushPainterInfo(
        bool isSizeInvariant,
        bool isColorInvariant,
        bool isPixelAliased,
        double minSize,
        double maxSize
    )
        : _data(new BrushPainterInfoInner(
            isSizeInvariant,
            isColorInvariant,
            isPixelAliased,
            minSize,
            maxSize
        ))
    {
    }

    BrushPainterInfo(const BrushPainterInfo& other)
        : _data(other._data)
    {
    }

    BrushPainterInfo(BrushPainterInfo&& other)
    {
        _data.swap(other._data);
    }

    bool isSizeInvariant() const { return _data->isSizeInvariant; }
    bool isColorInvariant() const { return _data->isColorInvariant; }
    bool isPixelAliased() const { return _data->isPixelAliased; }

    double getMinimumSize() const { return _data->minSize; }
    double getMaximumSize() const { return _data->maxSize; }

private:
    QSharedDataPointer<BrushPainterInfoInner> _data;
};

#endif // BRUSHPAINTERINFO_HPP