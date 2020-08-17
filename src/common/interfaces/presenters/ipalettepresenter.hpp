#ifndef IPALETTEPRESENTER_HPP
#define IPALETTEPRESENTER_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

#include "idtypes/paletteid.hpp"

#include "utilities/model/colorinfo.hpp"
#include "utilities/qmultiarray.hpp"

#include <QMetaType>

namespace Addle {

class IPalette;
class IPalettePresenter : public virtual IAmQObject
{
public: 
    virtual ~IPalettePresenter() = default;

    virtual void initialize(IPalette& model) = 0;
    virtual void initialize(PaletteId id) = 0;

    virtual IPalette& model() const = 0;
    virtual QMultiArray<ColorInfo, 2> colors() const = 0;
};

DECL_MAKEABLE(IPalettePresenter);
DECL_EXPECTS_INITIALIZE(IPalettePresenter);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IPalettePresenter, "org.addle.IPalettePresenter")

#endif // IPALETTEPRESENTER_HPP