#ifndef IBRUSH_HPP
#define IBRUSH_HPP

#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/initialize_traits.hpp"

#include "idtypes/brushid.hpp"
#include "interfaces/editing/brushrenderers/ibrushrenderer.hpp"

#include <QSharedPointer>
#include <QPainter>

class IBrush 
{
public:
    virtual ~IBrush() = default;
    
    virtual void initialize(BrushId id, QSharedPointer<IBrushRenderer> renderer) = 0;

    virtual BrushId getId() = 0;
    virtual QSharedPointer<IBrushRenderer> getRenderer() = 0;
};

DECL_MAKEABLE(IBrush)
DECL_EXPECTS_INITIALIZE(IBrush)

#endif // IBRUSH_HPP