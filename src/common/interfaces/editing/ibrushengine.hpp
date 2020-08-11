#ifndef IBRUSHENGINE_HPP
#define IBRUSHENGINE_HPP

#include <QList>
#include <QPainterPath>
#include <QRect>

#include "idtypes/brushengineid.hpp"

#include "utilities/editing/brushstroke.hpp"

#include "interfaces/traits.hpp"

class IBrushModel;
class IBrushEngine
{
public:
    virtual ~IBrushEngine() = default;

    virtual BrushEngineId id() const = 0;

    virtual QPainterPath indicatorShape(const BrushStroke& painter) const = 0;
    virtual void paint(BrushStroke& painter) const = 0;
};

DECL_PERSISTENT_OBJECT_TYPE(IBrushEngine, BrushEngineId);

#endif // IBRUSHENGINE_HPP
