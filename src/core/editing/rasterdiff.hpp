#ifndef RASTERDIFF_HPP
#define RASTERDIFF_HPP

#include "compat.hpp"
#include "interfaces/editing/irasterdiff.hpp"
namespace Addle {

class ADDLE_CORE_EXPORT RasterDiff : public IRasterDiff
{
public: 
    virtual ~RasterDiff() = default;

    void initialize(
        /*const*/ IRasterSurface& source,
        QWeakPointer<IRasterSurface> destination
    );

    void apply();

    QRect area() const { return _area; }
    QWeakPointer<IRasterSurface> desination() const { return _destination; }

    void compress() { }
    void uncompress() { }

    void blockingCompress() { }
    void blockingUncompress() { }

private: 
    static const int PIXEL_DEPTH = 4;

    QWeakPointer<IRasterSurface> _destination;

    QRect _area;

    bool _isCompressed = false;
    QByteArray _uncompressed;
    QByteArray _compressed;
};

} // namespace Addle

#endif // RASTERDIFF_HPP