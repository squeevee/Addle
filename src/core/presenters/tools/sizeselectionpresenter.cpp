#include "sizeselectionpresenter.hpp"
#include <QtDebug>

void SizeSelectionPresenter::initialize(QSharedPointer<IconProvider> iconProvider)
{
    _initHelper.initializeBegin();

    _iconProvider = iconProvider;

    _initHelper.initializeEnd();
}

QList<double> SizeSelectionPresenter::presets() const { _initHelper.check(); return _presets; }
double SizeSelectionPresenter::get() const { _initHelper.check(); return _size; }
int SizeSelectionPresenter::selectedPreset() const { _initHelper.check(); return _selectedPreset; }

void SizeSelectionPresenter::setPresets(QList<double> presets)
{ 
    _initHelper.check();
    _presets = presets;

    int index = 0;
    _presetsIndex.clear();
    _presetIcons.clear();
    for (double preset : _presets)
    {
        _presetsIndex[preset] = index;
        if (_iconProvider) _presetIcons.append(_iconProvider->icon(preset));
        index++;
    }

    emit presetsChanged(_presets);

    _presetHelper.setPresets(_presets);

    selectPreset(-1);
}

void SizeSelectionPresenter::set(double size)
{  
    _initHelper.check();
    size = qBound(_min, size, _max);
    _selectedPreset = _presetHelper.nearest(size, 0.01);
    _size = size;

    updateIcon();

    emit changed(_size);
    emit selectedPresetChanged(_selectedPreset);
}

void SizeSelectionPresenter::selectPreset(int index)
{
    _initHelper.check();
    _selectedPreset = index;
    if (index == -1)
        _size = qQNaN();
    else 
        _size = _presets[index];
    
    updateIcon();

    emit changed(_size);
    emit selectedPresetChanged(_selectedPreset);
}

void SizeSelectionPresenter::updateIcon()
{
    bool wasNull = _icon.isNull();
    if (_iconProvider && !qIsNaN(_size) && _size > 0)
        _icon = _iconProvider->icon(_size);

    if (_icon.isNull() != wasNull) emit refreshPreviews();
}