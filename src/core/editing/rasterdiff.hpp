#ifndef RASTERDIFF_HPP
#define RASTERDIFF_HPP

#include "core/compat.hpp"
#include "interfaces/editing/irasterdiff.hpp"

class ADDLE_CORE_EXPORT RasterDiff : public IRasterDiff
{
public: 
    virtual ~RasterDiff() = default;

    void initialize(
        /*const*/ IRasterSurface& source,
        QWeakPointer<IRasterSurface> destination
    );

    void apply();

    QRect getArea() const { return _area; }
    QWeakPointer<IRasterSurface> getDesination() const { return _destination; }

    void compress() { }
    void uncompress() { }

    void blockingCompress() { }
    void blockingUncompress() { }

private: 
    static const int PIXEL_WIDTH = 4;

    QWeakPointer<IRasterSurface> _destination;

    QRect _area;

    bool _isCompressed = false;
    QByteArray _uncompressed;
    QByteArray _compressed;
};

#endif // RASTERDIFF_HPP