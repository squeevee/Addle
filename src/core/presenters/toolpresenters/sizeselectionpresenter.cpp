#include "sizeselectionpresenter.hpp"
#include <QtDebug>

QList<double> SizeSelectionPresenter::presets() const { return _presets; }
double SizeSelectionPresenter::get() const { return _size; }
int SizeSelectionPresenter::selectedPreset() const { return _selectedPreset; }

void SizeSelectionPresenter::setIcon(QIcon icon)
{
    _icon = icon;
    emit iconChanged(_icon);
}

void SizeSelectionPresenter::setPresetIcons(QList<QIcon> icons)
{
    _presetIcons = icons;
    emit presetIconsChanged(_presetIcons);
}

void SizeSelectionPresenter::setPresets(QList<double> presets)
{
    _presets = presets;
    

    int index = 0;
    _presetsIndex.clear();
    for (double preset : _presets)
    {
        _presetsIndex[preset] = index;
        index++;
    }

    emit presetsChanged(_presets);

    _presetHelper.setPresets(_presets);

    selectPreset(-1);
}

void SizeSelectionPresenter::set(double size)
{
    _selectedPreset = _presetHelper.nearest(size, 0.01);
    _size = size;

    emit changed(_size);
    emit selectedPresetChanged(_selectedPreset);
}

void SizeSelectionPresenter::selectPreset(int index)
{
    _selectedPreset = index;
    if (index == -1)
        _size = qQNaN();
    else 
        _size = _presets[index];

    emit changed(_size);
    emit selectedPresetChanged(_selectedPreset);
}