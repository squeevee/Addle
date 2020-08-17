#ifndef ICOLORPALETTE_HPP
#define ICOLORPALETTE_HPP

#include "utilities/qmultiarray.hpp"
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
    typedef QMultiArray<ColorInfo, 2> ColorArray;

    virtual ~IPalette() = default;

    virtual PaletteId id() const = 0;

    virtual void initialize(const PaletteBuilder& builder) = 0;

    virtual ColorArray colors() const = 0;

    virtual bool contains(QColor color) const = 0;
    virtual ColorInfo infoFor(QColor color) const = 0;

public slots:
    virtual void setColors(ColorArray colors) = 0;

signals: 
    virtual void colorsChanged(ColorArray colors) = 0;
};

DECL_MAKEABLE(IPalette)
DECL_EXPECTS_INITIALIZE(IPalette);
DECL_PERSISTENT_OBJECT_TYPE(IPalette, PaletteId);

} // namespace Addle

Q_DECLARE_METATYPE(Addle::IPalette::ColorArray);

Q_DECLARE_INTERFACE(Addle::IPalette, "org.addle.IPalette")

#endif // ICOLORPALETTE_HPP