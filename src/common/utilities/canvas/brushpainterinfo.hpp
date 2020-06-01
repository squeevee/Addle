#ifndef BRUSHPAINTERINFO_HPP
#define BRUSHPAINTERINFO_HPP

#include "compat.hpp"
#include <QSharedData>

class ADDLE_COMMON_EXPORT BrushPainterInfoBuilder
{
public:
    inline BrushPainterInfoBuilder& setSizeInvariant(bool value) { isSizeInvariant = value; return *this; }
    inline BrushPainterInfoBuilder& setColorInvariant(bool value) { isColorInvariant = value; return *this; }
    inline BrushPainterInfoBuilder& setPixelAliased(bool value) { isPixelAliased = value; return *this; }

    inline BrushPainterInfoBuilder& setMinSize(double value) { minSize = value; return *this; }
    inline BrushPainterInfoBuilder& setMaxSize(double value) { minSize = value; return *this; }

    inline BrushPainterInfoBuilder& setPreferredSizes(QList<double> value, bool strict = false) { preferredSizes = value; strictSizing = strict; return *this; }
    
private:
    bool isSizeInvariant = false;
    bool isColorInvariant = false;
    bool isPixelAliased = false;

    double minSize = 0;
    double maxSize = Q_INFINITY;

    QList<double> preferredSizes;
    bool strictSizing = false;

    friend class BrushPainterInfo;
};

class ADDLE_COMMON_EXPORT BrushPainterInfo
{
    struct BrushPainterInfoInner : QSharedData
    {
        BrushPainterInfoInner(const BrushPainterInfoBuilder& builder)
            : isSizeInvariant(builder.isSizeInvariant),
            isColorInvariant(builder.isColorInvariant),
            isPixelAliased(builder.isPixelAliased),
            minSize(builder.minSize),
            maxSize(builder.maxSize),
            preferredSizes(builder.preferredSizes)
        {
        }

        bool isSizeInvariant = false;
        bool isColorInvariant = false;
        bool isPixelAliased = false;

        double minSize = 0;
        double maxSize = Q_INFINITY;

        QList<double> preferredSizes;
    };
public:
    BrushPainterInfo() = default;
    BrushPainterInfo(const BrushPainterInfoBuilder& builder)
        : _data(new BrushPainterInfoInner(builder))
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

    bool isSizeInvariant() const { return _data ? _data->isSizeInvariant : false; }
    bool isColorInvariant() const { return _data ? _data->isColorInvariant : false; }
    bool isPixelAliased() const { return _data ? _data->isPixelAliased : false; }

    double getMinimumSize() const { return _data ? _data->minSize : 0; }
    double getMaximumSize() const { return _data ? _data->maxSize : 0; }

    QList<double> getPreferredSizes() const { return _data ? _data->preferredSizes : QList<double>(); }

    static BrushPainterInfo defaultValues()
    {
        return BrushPainterInfo(BrushPainterInfoBuilder());
    }

private:
    QSharedDataPointer<BrushPainterInfoInner> _data;
};

#endif // BRUSHPAINTERINFO_HPP