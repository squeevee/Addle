/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "palette.hpp"
using namespace Addle;

void Palette::initialize(const PaletteBuilder& builder)
{
    const Initializer init(_initHelper);

    _id = builder.id();
    _colors = builder.colors();
    buildIndex();
}

void Palette::setColors(MultiArray<ColorInfo, 2> colors)
{
    ASSERT_INIT();
    
    _colors = colors;
    buildIndex();

    emit colorsChanged(_colors);
}

void Palette::buildIndex()
{
    _index.clear();
    int y = 0;
    for (auto row : MutableRowViewer<ColorInfo>(_colors))
    {
        int x = 0;
        for (auto& info : row)
        {
            _index[info.color().rgb()] = info;
            info.setPos(QPoint(x, y));
            ++x;
        }
        ++y;
    }
}