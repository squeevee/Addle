#include "zoomrotatehelper.h"

#define ZOOM_VALUES_INIT( x ) { zoomValueOf(x), x }

ZoomRotateHelper::ZoomRotateHelper()
    : _zoomValues({
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_5percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_10percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::onesixth),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_25percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::onethird),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_50percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_75percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_100percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_150percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_200percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_300percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_400percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_500percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_800percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_1000percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_1600percent),
        ZOOM_VALUES_INIT(IEditorViewPortPresenter::ZoomPreset::_2000percent)
    }),
    _zoomPresets({
        IEditorViewPortPresenter::ZoomPreset::_5percent,
        IEditorViewPortPresenter::ZoomPreset::_10percent,
        IEditorViewPortPresenter::ZoomPreset::onesixth,
        IEditorViewPortPresenter::ZoomPreset::_25percent,
        IEditorViewPortPresenter::ZoomPreset::onethird,
        IEditorViewPortPresenter::ZoomPreset::_50percent,
        IEditorViewPortPresenter::ZoomPreset::_75percent,
        IEditorViewPortPresenter::ZoomPreset::_100percent,
        IEditorViewPortPresenter::ZoomPreset::_150percent,
        IEditorViewPortPresenter::ZoomPreset::_200percent,
        IEditorViewPortPresenter::ZoomPreset::_300percent,
        IEditorViewPortPresenter::ZoomPreset::_400percent,
        IEditorViewPortPresenter::ZoomPreset::_500percent,
        IEditorViewPortPresenter::ZoomPreset::_800percent,
        IEditorViewPortPresenter::ZoomPreset::_1000percent,
        IEditorViewPortPresenter::ZoomPreset::_1600percent,
        IEditorViewPortPresenter::ZoomPreset::_2000percent
    })
{
}

IEditorViewPortPresenter::ZoomPreset ZoomRotateHelper::nearest(double value, double threshold)
{
    auto iter = _zoomValues.upper_bound(value);
    auto upper = *iter;
    if (iter == _zoomValues.begin())
    {
        //The given value is smaller than all of the presets.

        if (threshold == 0.0 || value + (threshold * value) >= upper.first) {
            return upper.second;
        }
        else
        {
            return IEditorViewPortPresenter::ZoomPreset::nullzoom;
        }
    }
    else if (iter == _zoomValues.end())
    {
        //The given value is larger than all of the presets.

        iter--;
        auto lower = *iter;

        if (threshold == 0.0 || value - (threshold * value) <= lower.first) {
            return lower.second;
        }
        else
        {
            return IEditorViewPortPresenter::ZoomPreset::nullzoom;
        }
    }
    else
    {
        //The given value is between two presets, or equal to one preset.
        iter--;
        auto lower = *iter;

        auto nearer = IEditorViewPortPresenter::ZoomPreset::nullzoom;

        double upperDiff = upper.first - value;
        double lowerDiff = value - lower.first;
        if (upperDiff >= lowerDiff)
            nearer = upper.second;
        else
            nearer = lower.second;
        
        bool withinUpper = value + (threshold * value) >= upper.first;
        bool withinLower = value - (threshold * value) <= lower.first;

        if (threshold == 0.0 || (withinUpper && withinLower))
            return nearer;
        else if (withinUpper)
            return upper.second;
        else if (withinLower)
            return lower.second;
        else
            return IEditorViewPortPresenter::ZoomPreset::nullzoom;
    }
}

IEditorViewPortPresenter::ZoomPreset ZoomRotateHelper::nextUp(double value)
{
    auto iter = _zoomValues.upper_bound(value);
    if (iter == _zoomValues.end())
    {
        //The given value is greater than all of the presets.

        return IEditorViewPortPresenter::ZoomPreset::nullzoom;
    }
    else
    {
        return iter->second;
    }
}

IEditorViewPortPresenter::ZoomPreset ZoomRotateHelper::nextUp(IEditorViewPortPresenter::ZoomPreset preset)
{
    auto iter = _zoomPresets.find(preset);
    iter++;
    if (iter == _zoomPresets.end())
        return IEditorViewPortPresenter::ZoomPreset::nullzoom;
    else 
        return *iter;
}

IEditorViewPortPresenter::ZoomPreset ZoomRotateHelper::nextDown(double value)
{
    auto iter = _zoomValues.upper_bound(value);
    if (iter == _zoomValues.begin())
    {
        //The given value is smaller than all of the presets.

        return IEditorViewPortPresenter::ZoomPreset::nullzoom;
    }
    else
    {
        return (--iter)->second;
    }
}

IEditorViewPortPresenter::ZoomPreset ZoomRotateHelper::nextDown(IEditorViewPortPresenter::ZoomPreset preset)
{
    auto iter = _zoomPresets.find(preset);
    if (iter == _zoomPresets.begin())
        return IEditorViewPortPresenter::ZoomPreset::nullzoom;
    else 
        return *--iter;
}

double ZoomRotateHelper::zoomValueOf(IEditorViewPortPresenter::ZoomPreset preset)
{
    switch(preset)
    {
    case IEditorViewPortPresenter::ZoomPreset::_5percent:
        return 0.05;
    case IEditorViewPortPresenter::ZoomPreset::_10percent:
        return 0.10;
    case IEditorViewPortPresenter::ZoomPreset::onesixth:
        return 0.1667;
    case IEditorViewPortPresenter::ZoomPreset::_25percent:
        return 0.25;
    case IEditorViewPortPresenter::ZoomPreset::onethird:
        return 0.3333;
    case IEditorViewPortPresenter::ZoomPreset::_50percent:
        return 0.50;
    case IEditorViewPortPresenter::ZoomPreset::_75percent:
        return 0.75;
    case IEditorViewPortPresenter::ZoomPreset::_100percent:
        return 1.00;
    case IEditorViewPortPresenter::ZoomPreset::_150percent:
        return 1.50;
    case IEditorViewPortPresenter::ZoomPreset::_200percent:
        return 2.00;
    case IEditorViewPortPresenter::ZoomPreset::_300percent:
        return 3.00;
    case IEditorViewPortPresenter::ZoomPreset::_400percent:
        return 4.00;
    case IEditorViewPortPresenter::ZoomPreset::_500percent:
        return 5.00;
    case IEditorViewPortPresenter::ZoomPreset::_800percent:
        return 8.00;
    case IEditorViewPortPresenter::ZoomPreset::_1000percent:
        return 10.00;
    case IEditorViewPortPresenter::ZoomPreset::_1600percent:
        return 16.00;
    case IEditorViewPortPresenter::ZoomPreset::_2000percent:
        return 20.00;
    default:
        return 0; //throw?
    }
}
