#ifndef COREPALETTEBUILDERS_HPP
#define COREPALETTEBUILDERS_HPP


#include "utilities/model/palettebuilder.hpp"
#include "interfaces/models/ipalette.hpp"

#include "globalconstants.hpp"

namespace CorePaletteBuilders
{

const static PaletteBuilder basic = PaletteBuilder()
    .setId(GlobalConstants::CorePalettes::BasicPalette)
    .setSize(QSize(16, 3))
    .setColor(0,0,QColor::fromRgb(255,255,255),"White")
    .setColor(0,1,QColor::fromRgb(218,218,218),"Light Gray")
    .setColor(0,2,QColor::fromRgb(164,161,168),"Gray")
    .setColor(1,0,QColor::fromRgb(116,116,116),"Dark Gray")
    .setColor(1,1,QColor::fromRgb(74,74,74),"Charcoal")
    .setColor(1,2,QColor::fromRgb(0,0,0),"Black")
    .setColor(2,0,QColor::fromRgb(236,33,74),"Red")
    .setColor(2,1,QColor::fromRgb(174,46,71),"Crimson")
    .setColor(2,2,QColor::fromRgb(111,34,58),"Burgundy")
    .setColor(3,0,QColor::fromRgb(252,219,160),"Peach")
    .setColor(3,1,QColor::fromRgb(255,151,22),"Orange")
    .setColor(3,2,QColor::fromRgb(206,78,4),"Vermilion")
    .setColor(4,0,QColor::fromRgb(255,241,99),"Yellow")
    .setColor(4,1,QColor::fromRgb(252,205,31),"Goldenrod")
    .setColor(4,2,QColor::fromRgb(187,148,52),"Ocher")
    .setColor(5,0,QColor::fromRgb(174,255,62),"Lime")
    .setColor(5,1,QColor::fromRgb(131,197,47),"Grass")
    .setColor(5,2,QColor::fromRgb(113,137,71),"Olive")
    .setColor(6,0,QColor::fromRgb(41,231,88),"Green")
    .setColor(6,1,QColor::fromRgb(17,154,50),"Emerald")
    .setColor(6,2,QColor::fromRgb(43,78,41),"Moss")
    .setColor(7,0,QColor::fromRgb(116,252,206),"Aquamarine")
    .setColor(7,1,QColor::fromRgb(62,191,191),"Spruce")
    .setColor(7,2,QColor::fromRgb(59,118,128),"Teal")
    .setColor(8,0,QColor::fromRgb(199,252,254),"Turquoise")
    .setColor(8,1,QColor::fromRgb(1,229,255),"Cyan")
    .setColor(8,2,QColor::fromRgb(2,157,255),"Cerulean")
    .setColor(9,0,QColor::fromRgb(157,189,255),"Cornflower")
    .setColor(9,1,QColor::fromRgb(0,91,246),"Blue")
    .setColor(9,2,QColor::fromRgb(0,55,149),"Indigo")
    .setColor(10,0,QColor::fromRgb(214,208,255),"Periwinkle")
    .setColor(10,1,QColor::fromRgb(202,138,255),"Lavender")
    .setColor(10,2,QColor::fromRgb(99,59,189),"Violet")
    .setColor(11,0,QColor::fromRgb(255,154,213),"Pink")
    .setColor(11,1,QColor::fromRgb(222,76,132),"Rose")
    .setColor(11,2,QColor::fromRgb(100,34,103),"Purple")
    .setColor(12,0,QColor::fromRgb(226,198,143),"Tan")
    .setColor(12,1,QColor::fromRgb(252,212,190),"Apricot")
    .setColor(12,2,QColor::fromRgb(227,227,211),"Alabaster")
    .setColor(13,0,QColor::fromRgb(197,127,38),"Caramel")
    .setColor(13,1,QColor::fromRgb(217,160,136),"Satin")
    .setColor(13,2,QColor::fromRgb(144,144,124),"Ash")
    .setColor(14,0,QColor::fromRgb(116,55,32),"Sienna")
    .setColor(14,1,QColor::fromRgb(196,108,72),"Bronze")
    .setColor(14,2,QColor::fromRgb(200,190,211),"Marble")
    .setColor(15,0,QColor::fromRgb(80,55,43),"Umber")
    .setColor(15,1,QColor::fromRgb(132,91,75),"Brown")
    .setColor(15,2,QColor::fromRgb(114,108,131),"Slate");

}
#endif // COREPALETTEBUILDERS_HPP