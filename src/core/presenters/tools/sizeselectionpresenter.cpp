/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "sizeselectionpresenter.hpp"

#include "utils.hpp"

using namespace Addle;

void SizeSelectionPresenter::initialize(QSharedPointer<ISizeIconProvider> ISizeIconProvider)
{
    const Initializer init(_initHelper);

    _iconProvider = ISizeIconProvider;
}

QList<double> SizeSelectionPresenter::presets() const { ASSERT_INIT(); return _presets; }
double SizeSelectionPresenter::get() const { ASSERT_INIT(); return _size; }
int SizeSelectionPresenter::selectedPreset() const { ASSERT_INIT(); return _selectedPreset; }

void SizeSelectionPresenter::setPresets(QList<double> presets)
{
    try 
    {
        ASSERT_INIT();
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

        _PresetMap.setPresets(_presets);

        selectPreset(-1);
    }
    ADDLE_EVENT_CATCH
}

void SizeSelectionPresenter::set(double size)
{
    try 
    {
        _initHelper.isInitialized();

        size = qBound(_min, size, _max);
        _selectedPreset = _PresetMap.nearest(size, 0.01);
        _size = size;

        updateIcon();

        emit changed(_size);
        emit selectedPresetChanged(_selectedPreset);
    }
    ADDLE_SLOT_CATCH
}

void SizeSelectionPresenter::selectPreset(int index)
{
    try 
    {
        ASSERT_INIT();
        _selectedPreset = index;
        if (index == -1)
            _size = qQNaN();
        else 
            _size = _presets[index];
        
        updateIcon();

        emit changed(_size);
        emit selectedPresetChanged(_selectedPreset);
    }
    ADDLE_SLOT_CATCH
}

void SizeSelectionPresenter::updateIcon()
{
    bool wasNull = _icon.isNull();
    if (_iconProvider && !qIsNaN(_size) && _size > 0)
        _icon = _iconProvider->icon(_size);

    if (_icon.isNull() != wasNull)
        emit refreshPreviews();
}