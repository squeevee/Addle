/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ICOLORPALETTE_HPP
#define ICOLORPALETTE_HPP

#include "utilities/multiarray.hpp"
#include <QColor>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

#include "utilities/model/colorinfo.hpp"
#include "idtypes/paletteid.hpp"
#include "utilities/model/palettebuilder.hpp"

namespace Addle {

class IPalette : public virtual IAmQObject
{
public:
    typedef MultiArray<ColorInfo, 2> ColorArray;

    virtual ~IPalette() = default;

    virtual PaletteId id() const = 0;

    virtual ColorArray colors() const = 0;

    virtual bool contains(QColor color) const = 0;
    virtual ColorInfo infoFor(QColor color) const = 0;

public slots:
    virtual void setColors(ColorArray colors) = 0;

signals: 
    virtual void colorsChanged(ColorArray colors) = 0;
};

namespace aux_IPalette {
    ADDLE_FACTORY_PARAMETER_ALIAS( config_detail::generic_id_parameter, id )
    ADDLE_FACTORY_PARAMETER_NAME( builder )
}

ADDLE_DECL_SINGLETON_REPO_MEMBER_BASIC(IPalette, PaletteId);
ADDLE_DECL_FACTORY_PARAMETERS(
    IPalette, 
    (required 
        (id,      (PaletteId)) 
        (builder, (const PaletteBuilder&)) 
    ) 
        // TODO: r-value
        // or better still, could the builder be folded into the parameter
        // definition?
)

// TODO: factory parameters

} // namespace Addle

Q_DECLARE_METATYPE(Addle::IPalette::ColorArray);

Q_DECLARE_INTERFACE(Addle::IPalette, "org.addle.IPalette")

#endif // ICOLORPALETTE_HPP
