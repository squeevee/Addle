#ifndef IRASTERDIFF_HPP
#define IRASTERDIFF_HPP

#include <QImage>
#include <QPainter>
#include <QBrush>
#include <QWeakPointer>

#include "interfaces/models/ilayer.hpp"
#include "interfaces/traits.hpp"



#include <QImage>
#include <QPoint>
namespace Addle {

class IRasterSurface;
class IRasterDiff
{
public:
    virtual ~IRasterDiff() = default;

    virtual void initialize(
        IRasterSurface& source,
        QWeakPointer<IRasterSurface> destination
    ) = 0;

    virtual void apply() = 0;

    virtual QRect area() const = 0;
    virtual QWeakPointer<IRasterSurface> desination() const = 0;

    virtual void compress() = 0;
    virtual void uncompress() = 0;

    virtual void blockingCompress() = 0;
    virtual void blockingUncompress() = 0;
};

DECL_MAKEABLE(IRasterDiff)
DECL_EXPECTS_INITIALIZE(IRasterDiff)

} // namespace Addle
#endif // IRASTERDIFF_HPP