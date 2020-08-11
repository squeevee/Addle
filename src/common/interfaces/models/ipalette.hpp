#ifndef ICOLORPALETTE_HPP
#define ICOLORPALETTE_HPP

#include "utilities/qmultiarray.hpp"
#include <QColor>

#include "interfaces/traits.hpp"




#include "utilities/model/colorinfo.hpp"
#include "idtypes/paletteid.hpp"
#include "utilities/model/palettebuilder.hpp"

class IPalette
{
public:
    virtual ~IPalette() = default;

    virtual PaletteId id() const = 0;

    virtual void initialize(const PaletteBuilder& builder) = 0;

    virtual QMultiArray<ColorInfo, 2> colors() const = 0;
    virtual void setColors(QMultiArray<ColorInfo, 2> colors) = 0;

    virtual bool contains(QColor color) const = 0;
    virtual ColorInfo infoFor(QColor color) const = 0;

signals: 
    virtual void colorsChanged() = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IPalette);
DECL_MAKEABLE(IPalette)
DECL_EXPECTS_INITIALIZE(IPalette);
DECL_PERSISTENT_OBJECT_TYPE(IPalette, PaletteId);

#endif // ICOLORPALETTE_HPP