#ifndef COREPALETTEBUILDERS_HPP
#define COREPALETTEBUILDERS_HPP


#include "utilities/model/palettebuilder.hpp"
#include "interfaces/models/ipalette.hpp"

#include "globalconstants.hpp"

namespace CorePaletteBuilders
{

const static PaletteBuilder test = PaletteBuilder()
    .setId(GlobalConstants::CorePalettes::TestPalette)
    .setSize(QSize(4, 1))
    .setColor(0, 0, Qt::black, "black")
    .setColor(1, 0, Qt::white, "white")
    .setColor(2, 0, Qt::red, "red")
    .setColor(3, 0, Qt::blue, "blue");

}
#endif // COREPALETTEBUILDERS_HPP