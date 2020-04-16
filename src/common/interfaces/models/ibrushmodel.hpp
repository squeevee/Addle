#ifndef IBRUSHMODEL_HPP
#define IBRUSHMODEL_HPP

#include "idtypes/brushid.hpp"

#include "utilities/canvas/brushpainterinfo.hpp"
#include "interfaces/traits/compat.hpp"
#include "interfaces/traits/by_id_traits.hpp"

class ADDLE_COMMON_EXPORT IBrushModel
{
public:
    virtual ~IBrushModel() = default;

    virtual BrushId getId() const = 0;

    virtual BrushPainterInfo getPainterInfo() const = 0;
};

DECL_PERSISTENT_OBJECT_TYPE(IBrushModel, BrushId);

#endif // IBRUSHMODEL_HPP