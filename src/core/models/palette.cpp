#include "palette.hpp"

void Palette::initialize(const PaletteBuilder& builder)
{
    _initHelper.initializeBegin();

    _id = builder.id();
    _colors = builder.colors();
    buildIndex();

    _initHelper.initializeEnd();
}

void Palette::setColors(QMultiArray<ColorInfo, 2> colors)
{
    _initHelper.check();
    
    _colors = colors;
    buildIndex();

    emit colorsChanged();
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