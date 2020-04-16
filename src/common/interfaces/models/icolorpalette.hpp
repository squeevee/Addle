#ifndef ICOLORPALETTE_HPP
#define ICOLORPALETTE_HPP

#include "utilities/rectangulararray.hpp"
#include "interfaces/traits/compat.hpp"
#include <QColor>

class ADDLE_COMMON_EXPORT IColorPalette
{
public:
    virtual ~IColorPalette() = default;
    virtual RectangularArray<QColor> getColors() = 0;

};

#endif // ICOLORPALETTE_HPP