#ifndef IBRUSH_HPP
#define IBRUSH_HPP

#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/initialize_trait.hpp"

#include "idtypes/brushid.hpp"
#include "interfaces/editing/brushpainters/ibrushpainter.hpp"

#include <QSharedPointer>
#include <QPainter>

class IBrush 
{
public:
    virtual ~IBrush() = default;
    
    virtual void initialize(BrushId id, QSharedPointer<IBrushPainter> renderer) = 0;

    virtual BrushId getId() = 0;
    virtual QSharedPointer<IBrushPainter> getPainter() = 0;
};

DECL_MAKEABLE(IBrush)
DECL_EXPECTS_INITIALIZE(IBrush)

#endif // IBRUSH_HPP