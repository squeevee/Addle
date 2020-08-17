#include "colorselectionpresenter.hpp"
using namespace Addle;

void ColorSelectionPresenter::initialize(QList<QSharedPointer<IPalettePresenter>> palettes)
{
    const Initializer init(_initHelper);

    _palettes = palettes;
}

void ColorSelectionPresenter::setPalettes(QList<QSharedPointer<IPalettePresenter>> palettes)
{
    try 
    {
        _initHelper.check();

        _palettes = palettes;
        emit palettesChanged(_palettes);
    }
    ADDLE_SLOT_CATCH
}

void ColorSelectionPresenter::setPalette(QSharedPointer<IPalettePresenter> palette)
{
    try 
    {
        _initHelper.check();
        
        if (palette != _palette)
        {
            _palette = palette;
            emit paletteChanged(_palette);
        }
    }
    ADDLE_SLOT_CATCH
}

ColorInfo ColorSelectionPresenter::color(int which) const
{
    switch (which)
    {
    case 1: 
        return _color1;
    case 2:
        return _color2;
    default:
        ADDLE_LOGIC_ERROR_M("Invalid `which`");
    }
}

void ColorSelectionPresenter::setColor(int which, ColorInfo color)
{
    try 
    {
        switch(which)
        {
        case 1:
            setColor1(color);
            break;
        case 2:
            setColor2(color);
            break;
        default:
            ADDLE_LOGIC_ERROR_M("Invalid `which`");
        }
    }
    ADDLE_SLOT_CATCH
}

void ColorSelectionPresenter::setColor1(ColorInfo color)
{
    try 
    {
        _initHelper.check();

        _color1 = color;
        emit colorChanged(1, _color1);
        emit color1Changed(_color1);
    }
    ADDLE_SLOT_CATCH
}

void ColorSelectionPresenter::setColor2(ColorInfo color)
{
    try 
    {
        _initHelper.check();

        _color2 = color;
        emit colorChanged(2, _color2);
        emit color2Changed(_color2); 
    }
    ADDLE_SLOT_CATCH
}

void ColorSelectionPresenter::setActive(int active)
{
    try 
    {
        //warn if invalid which
        if (active != _active)
        {
            _active = qBound(1, active, 2);
            emit activeChanged(_active);
        }
    }
    ADDLE_SLOT_CATCH
}