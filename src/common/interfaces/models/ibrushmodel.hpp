#ifndef IBRUSHMODEL_HPP
#define IBRUSHMODEL_HPP

#include "idtypes/brushid.hpp"
#include "idtypes/brushengineid.hpp"

#include "utilities/editing/brushinfo.hpp"
#include "interfaces/traits/by_id_traits.hpp"

class IBrushModel
{
public:
    virtual ~IBrushModel() = default;

    virtual BrushId id() const = 0;
    virtual BrushEngineId engineId() const = 0;

    virtual BrushInfo info() const = 0;

    // Common engine parameters, like pressure and velocity dynamics, will be
    // given properties here.

    // virtual QVariantHash& customEngineParameters() = 0;
    virtual QVariantHash customEngineParameters() const = 0;
};

DECL_PERSISTENT_OBJECT_TYPE(IBrushModel, BrushId);

#endif // IBRUSHMODEL_HPP