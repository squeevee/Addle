#ifndef IBRUSHMODEL_HPP
#define IBRUSHMODEL_HPP

#include "idtypes/brushid.hpp"

#include "utilities/canvas/brushpainterinfo.hpp"
#include "interfaces/traits/by_id_traits.hpp"

class IBrushModel
{
public:
    virtual ~IBrushModel() = default;

    virtual BrushId getId() const = 0;

    virtual BrushPainterInfo getPainterInfo() const = 0;
    
    struct CoreBrushes
    {
        static const BrushId BasicBrush;
    };
};

DECL_PERSISTENT_OBJECT_TYPE(IBrushModel, BrushId);

#endif // IBRUSHMODEL_HPP