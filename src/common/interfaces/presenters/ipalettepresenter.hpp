#ifndef IPALETTEPRESENTER_HPP
#define IPALETTEPRESENTER_HPP

#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/initialize_trait.hpp"

#include "utilities/model/colorinfo.hpp"
#include "utilities/qmultiarray.hpp"

class IPalette;
class IPalettePresenter
{
public: 
    virtual ~IPalettePresenter() = default;

    virtual void initialize(IPalette& model) = 0;

    virtual IPalette& model() const = 0;
    virtual QMultiArray<ColorInfo, 2> colors() const = 0;
};

DECL_EXPECTS_INITIALIZE(IPalettePresenter);
DECL_IMPLEMENTED_AS_QOBJECT(IPalettePresenter);
DECL_MAKEABLE(IPalettePresenter);

#endif // IPALETTEPRESENTER_HPP