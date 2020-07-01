#include "colorselectionpresenter.hpp"

void ColorSelectionPresenter::initialize(QList<QSharedPointer<IPalettePresenter>> palettes)
{
    _initHelper.initializeBegin();

    _palettes = palettes;

    _initHelper.initializeEnd();
}

void ColorSelectionPresenter::setPalettes(QList<QSharedPointer<IPalettePresenter>> palettes)
{
    _initHelper.check();

    _palettes = palettes;
    emit palettesChanged(_palettes);
}

void ColorSelectionPresenter::setPalette(QSharedPointer<IPalettePresenter> palette)
{
    _initHelper.check();
    
    if (palette != _palette)
    {
        _palette = palette;
        emit paletteChanged(_palette);
    }
}

ColorInfo ColorSelectionPresenter::color(int which) const
{
    //warn if invalid which
    if (which == 1) return color1();
    else return color2();
}

void ColorSelectionPresenter::setColor(int which, ColorInfo color)
{
    //warn if invalid which
    if (which == 1) setColor1(color);
    else setColor2(color);
}

void ColorSelectionPresenter::setColor1(ColorInfo color)
{
    _initHelper.check();

    _color1 = color;
    emit colorChanged(1, _color1);
    emit color1Changed(_color1);
}

void ColorSelectionPresenter::setColor2(ColorInfo color)
{
    _initHelper.check();

    _color2 = color;
    emit colorChanged(2, _color2);
    emit color2Changed(_color2); 
}

void ColorSelectionPresenter::setActive(int active)
{
    //warn if invalid which
    if (active != _active)
    {
        _active = qBound(1, active, 2);
        emit activeChanged(_active);
    }
}