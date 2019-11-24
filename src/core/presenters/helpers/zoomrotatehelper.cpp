#include "zoomrotatehelper.hpp"

#define ZOOM_VALUES_INIT( x ) { zoomValueOf(x), x }

ZoomRotateHelper::ZoomRotateHelper()
    : _zoomValues({
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_5percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_10percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::onesixth),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_25percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::onethird),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_50percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_75percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_100percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_150percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_200percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_300percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_400percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_500percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_800percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_1000percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_1600percent),
        ZOOM_VALUES_INIT(IViewPortPresenter::ZoomPreset::_2000percent)
    }),
    _zoomPresets({
        IViewPortPresenter::ZoomPreset::_5percent,
        IViewPortPresenter::ZoomPreset::_10percent,
        IViewPortPresenter::ZoomPreset::onesixth,
        IViewPortPresenter::ZoomPreset::_25percent,
        IViewPortPresenter::ZoomPreset::onethird,
        IViewPortPresenter::ZoomPreset::_50percent,
        IViewPortPresenter::ZoomPreset::_75percent,
        IViewPortPresenter::ZoomPreset::_100percent,
        IViewPortPresenter::ZoomPreset::_150percent,
        IViewPortPresenter::ZoomPreset::_200percent,
        IViewPortPresenter::ZoomPreset::_300percent,
        IViewPortPresenter::ZoomPreset::_400percent,
        IViewPortPresenter::ZoomPreset::_500percent,
        IViewPortPresenter::ZoomPreset::_800percent,
        IViewPortPresenter::ZoomPreset::_1000percent,
        IViewPortPresenter::ZoomPreset::_1600percent,
        IViewPortPresenter::ZoomPreset::_2000percent
    })
{
}

IViewPortPresenter::ZoomPreset ZoomRotateHelper::nearest(double value, double threshold)
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
            return IViewPortPresenter::ZoomPreset::nullzoom;
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
            return IViewPortPresenter::ZoomPreset::nullzoom;
        }
    }
    else
    {
        //The given value is between two presets, or equal to one preset.
        iter--;
        auto lower = *iter;

        auto nearer = IViewPortPresenter::ZoomPreset::nullzoom;

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
            return IViewPortPresenter::ZoomPreset::nullzoom;
    }
}

IViewPortPresenter::ZoomPreset ZoomRotateHelper::nextUp(double value)
{
    auto iter = _zoomValues.upper_bound(value);
    if (iter == _zoomValues.end())
    {
        //The given value is greater than all of the presets.

        return IViewPortPresenter::ZoomPreset::nullzoom;
    }
    else
    {
        return iter->second;
    }
}

IViewPortPresenter::ZoomPreset ZoomRotateHelper::nextUp(IViewPortPresenter::ZoomPreset preset)
{
    auto iter = _zoomPresets.find(preset);
    iter++;
    if (iter == _zoomPresets.end())
        return IViewPortPresenter::ZoomPreset::nullzoom;
    else 
        return *iter;
}

IViewPortPresenter::ZoomPreset ZoomRotateHelper::nextDown(double value)
{
    auto iter = _zoomValues.upper_bound(value);
    if (iter == _zoomValues.begin())
    {
        //The given value is smaller than all of the presets.

        return IViewPortPresenter::ZoomPreset::nullzoom;
    }
    else
    {
        return (--iter)->second;
    }
}

IViewPortPresenter::ZoomPreset ZoomRotateHelper::nextDown(IViewPortPresenter::ZoomPreset preset)
{
    auto iter = _zoomPresets.find(preset);
    if (iter == _zoomPresets.begin())
        return IViewPortPresenter::ZoomPreset::nullzoom;
    else 
        return *--iter;
}

double ZoomRotateHelper::zoomValueOf(IViewPortPresenter::ZoomPreset preset)
{
    switch(preset)
    {
    case IViewPortPresenter::ZoomPreset::_5percent:
        return 0.05;
    case IViewPortPresenter::ZoomPreset::_10percent:
        return 0.10;
    case IViewPortPresenter::ZoomPreset::onesixth:
        return 0.1667;
    case IViewPortPresenter::ZoomPreset::_25percent:
        return 0.25;
    case IViewPortPresenter::ZoomPreset::onethird:
        return 0.3333;
    case IViewPortPresenter::ZoomPreset::_50percent:
        return 0.50;
    case IViewPortPresenter::ZoomPreset::_75percent:
        return 0.75;
    case IViewPortPresenter::ZoomPreset::_100percent:
        return 1.00;
    case IViewPortPresenter::ZoomPreset::_150percent:
        return 1.50;
    case IViewPortPresenter::ZoomPreset::_200percent:
        return 2.00;
    case IViewPortPresenter::ZoomPreset::_300percent:
        return 3.00;
    case IViewPortPresenter::ZoomPreset::_400percent:
        return 4.00;
    case IViewPortPresenter::ZoomPreset::_500percent:
        return 5.00;
    case IViewPortPresenter::ZoomPreset::_800percent:
        return 8.00;
    case IViewPortPresenter::ZoomPreset::_1000percent:
        return 10.00;
    case IViewPortPresenter::ZoomPreset::_1600percent:
        return 16.00;
    case IViewPortPresenter::ZoomPreset::_2000percent:
        return 20.00;
    default:
        return 0; //throw?
    }
}
