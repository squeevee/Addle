/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IPALETTEPRESENTER_HPP
#define IPALETTEPRESENTER_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

// #include "idtypes/paletteid.hpp"

#include "utilities/model/colorinfo.hpp"
#include "utilities/multiarray.hpp"

#include <QMetaType>

namespace Addle {

class IPalette;
class IPalettePresenter : public virtual IAmQObject
{
public: 
    virtual ~IPalettePresenter() = default;
    
//     virtual PaletteId id() const = 0;

    virtual IPalette& model() const = 0;
    virtual MultiArray<ColorInfo, 2> colors() const = 0;
};

namespace aux_IPalettePresenter {
    ADDLE_FACTORY_PARAMETER_NAME( model )
}

// ADDLE_DECL_UNIQUE_REPO_MEMBER_BASIC(IPalettePresenter, PaletteId)
ADDLE_DECL_FACTORY_PARAMETERS(
    IPalettePresenter,
    (required  ( model, (IPalette&) ))
)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IPalettePresenter, "org.addle.IPalettePresenter")

#endif // IPALETTEPRESENTER_HPP
