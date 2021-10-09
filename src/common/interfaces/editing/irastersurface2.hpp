#pragma once

#include <boost/multi_array.hpp>

#include "utilities/image/rastertile.hpp"

#include "interfaces/iamqobject.hpp"
#include "interfaces/rendering/irenderable.hpp"

namespace Addle {
 
// Replacement for TileSurface, where TileSurface was already essentially a 
// replacement for IRasterSurface.
class IRasterSurface2 : public IRenderable, public virtual IAmQObject
{
protected:
    using grid_t = boost::multi_array_ref<RasterTile, 2>;
public:
    using tiles_view_t = grid_t::const_array_view<2>::type;
    
    virtual ~IRasterSurface2() = default;
    
};
    
}

Q_DECLARE_INTERFACE(Addle::IRasterSurface2, "org.addle.IRasterSurface2")
