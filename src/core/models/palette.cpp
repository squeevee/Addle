#include "palette.hpp"
using namespace Addle;

void Palette::initialize(const PaletteBuilder& builder)
{
    const Initializer init(_initHelper);

    _id = builder.id();
    _colors = builder.colors();
    buildIndex();
}

void Palette::setColors(QMultiArray<ColorInfo, 2> colors)
{
    _initHelper.check();
    
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