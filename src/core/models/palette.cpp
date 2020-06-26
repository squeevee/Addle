#include "palette.hpp"

void Palette::initialize(const PaletteBuilder& builder)
{
    _initHelper.initializeBegin();

    _id = builder.id();
    _colors = builder.colors();

    _initHelper.initializeEnd();
}

void Palette::setColors(QMultiArray<ColorInfo, 2> colors)
{
    _initHelper.check();
    
    _colors = colors;
    emit colorsChanged();
}