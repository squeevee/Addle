#ifndef BRUSHINFO_HPP
#define BRUSHINFO_HPP

#include "compat.hpp"
#include <QSharedData>

class ADDLE_COMMON_EXPORT BrushInfoBuilder
{
public:
    inline BrushInfoBuilder& setSizeInvariant(bool value) { isSizeInvariant = value; return *this; }
    inline BrushInfoBuilder& setColorInvariant(bool value) { isColorInvariant = value; return *this; }
    inline BrushInfoBuilder& setPixelAliased(bool value) { isPixelAliased = value; return *this; }

    inline BrushInfoBuilder& setMinSize(double value) { minSize = value; return *this; }
    inline BrushInfoBuilder& setMaxSize(double value) { minSize = value; return *this; }

    inline BrushInfoBuilder& setPreferredSizes(QList<double> value, bool strict = false) { preferredSizes = value; strictSizing = strict; return *this; }
    
private:
    bool isSizeInvariant = false;
    bool isColorInvariant = false;
    bool isPixelAliased = false;

    double minSize = 0;
    double maxSize = Q_INFINITY;

    QList<double> preferredSizes;
    bool strictSizing = false;

    friend class BrushInfo;
};

class ADDLE_COMMON_EXPORT BrushInfo
{
    struct BrushInfoInner : QSharedData
    {
        BrushInfoInner(const BrushInfoBuilder& builder)
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
    BrushInfo() = default;
    BrushInfo(const BrushInfoBuilder& builder)
        : _data(new BrushInfoInner(builder))
    {
    }

    BrushInfo(const BrushInfo& other)
        : _data(other._data)
    {
    }

    BrushInfo(BrushInfo&& other)
    {
        _data.swap(other._data);
    }

    bool isNullInfo() const { return !_data; }

    bool isSizeInvariant() const { return _data ? _data->isSizeInvariant : false; }
    bool isColorInvariant() const { return _data ? _data->isColorInvariant : false; }
    bool isPixelAliased() const { return _data ? _data->isPixelAliased : false; }

    double getMinimumSize() const { return _data ? _data->minSize : 0; }
    double getMaximumSize() const { return _data ? _data->maxSize : 0; }

    QList<double> getPreferredSizes() const { return _data ? _data->preferredSizes : QList<double>(); }

private:
    QSharedDataPointer<BrushInfoInner> _data;
};

#endif // BRUSHINFO_HPP