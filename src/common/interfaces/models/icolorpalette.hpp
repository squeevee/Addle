#ifndef ICOLORPALETTE_HPP
#define ICOLORPALETTE_HPP

#include "utilities/qt_extensions/rectangulararray.hpp"
#include <QColor>

class IColorPalette
{
public:
    virtual ~IColorPalette() = default;
    RectangularArray<QColor> getColors();

};

#endif // ICOLORPALETTE_HPP