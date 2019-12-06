#ifndef BRUSHSIZEPRESETHELPER_HPP
#define BRUSHSIZEPRESETHELPER_HPP

#include <QtGlobal>

#include "presethelper.hpp"
#include "interfaces/presenters/tools/ibrushliketoolpresenter.hpp"

#define BRUSH_PX_SIZE_VALUES_INIT(x) { BrushSizePresetHelper::pixelValueOf(x), x }
#define BRUSH_PERCENT_SIZE_VALUES_INIT(x) { BrushSizePresetHelper::percentValueOf(x), x }

class BrushSizePresetHelper
{
    typedef IBrushLikeToolPresenter::SizeOption SizeOption;

    class PxPresetHelper : public PresetHelper<IBrushLikeToolPresenter::SizeOption>
    {
        PxPresetHelper()
            : PresetHelper(
                false,
                SizeOption::custom_px,
                SizeOption::_4px,
                SizeOption::_500px,
                {
                    SizeOption::_4px,
                    SizeOption::_7px,
                    SizeOption::_13px,
                    SizeOption::_25px,
                    SizeOption::_50px,
                    SizeOption::_100px,
                    SizeOption::_250px,
                    SizeOption::_500px
                },
                {
                    BRUSH_PX_SIZE_VALUES_INIT(SizeOption::_4px),
                    BRUSH_PX_SIZE_VALUES_INIT(SizeOption::_7px),
                    BRUSH_PX_SIZE_VALUES_INIT(SizeOption::_13px),
                    BRUSH_PX_SIZE_VALUES_INIT(SizeOption::_25px),
                    BRUSH_PX_SIZE_VALUES_INIT(SizeOption::_50px),
                    BRUSH_PX_SIZE_VALUES_INIT(SizeOption::_100px),
                    BRUSH_PX_SIZE_VALUES_INIT(SizeOption::_250px),
                    BRUSH_PX_SIZE_VALUES_INIT(SizeOption::_500px)
                }
            )
        {
        }

        friend class BrushSizePresetHelper;
    };

    class PercentPresetHelper : public PresetHelper<IBrushLikeToolPresenter::SizeOption>
    {
        friend class BrushSizePresetHelper;
        PercentPresetHelper()
            : PresetHelper(
                false,
                SizeOption::custom_percent,
                SizeOption::_10percent,
                SizeOption::_50percent,
                {
                    SizeOption::_10percent,
                    SizeOption::_25percent,
                    SizeOption::_33percent,
                    SizeOption::_50percent,
                },
                {
                    BRUSH_PX_SIZE_VALUES_INIT(SizeOption::_10percent),
                    BRUSH_PX_SIZE_VALUES_INIT(SizeOption::_25percent),
                    BRUSH_PX_SIZE_VALUES_INIT(SizeOption::_33percent),
                    BRUSH_PX_SIZE_VALUES_INIT(SizeOption::_50percent),
                }
            )
        {
        }
    };

    static const PxPresetHelper _instance_px;
    static const PercentPresetHelper _instance_percent;

public:

    inline static bool isPixelOption(SizeOption option)
    {
        switch (option)
        {
        case SizeOption::_4px:
        case SizeOption::_7px:
        case SizeOption::_13px:
        case SizeOption::_25px:
        case SizeOption::_50px:
        case SizeOption::_100px:
        case SizeOption::_250px:
        case SizeOption::_500px:
        case SizeOption::custom_px:
            return true;
        default:
            return false;
        }
    }

    inline static bool isPercentOption(SizeOption option)
    {
        return !isPixelOption(option);
    }

    inline static int pixelValueOf(SizeOption option)
    {
        if (isPixelOption(option))
        {
            switch (option)
            {
                case SizeOption::_4px:
                return 4;
                case SizeOption::_7px:
                return 7;
                case SizeOption::_13px:
                return 13;
                case SizeOption::_25px:
                return 25;
                case SizeOption::_50px:
                return 50;
                case SizeOption::_100px:
                return 100;
                case SizeOption::_250px:
                return 250;
                case SizeOption::_500px:
                return 500;
            }
        }
        return -1;
    }

    inline static int percentValueOf(SizeOption option)
    {
        if (isPercentOption(option))
        {
            switch (option)
            {
                case SizeOption::_10percent:
                return 10;
                case SizeOption::_25percent:
                return 25;
                case SizeOption::_33percent:
                return 33;
                case SizeOption::_50percent:
                return 50;
            }
        }
        return -1;
    }

    inline static double fractionValueOf(SizeOption option)
    {
        if (isPercentOption(option))
        {
            return (double)(percentValueOf(option)) / 100;
        }
        return qQNaN();
    }

    static SizeOption nearestPx(double value, double threshold = 0) { return _instance_px.nearest_p(value, threshold); }
    static SizeOption nextUpPx(double value) { return _instance_px.nextUp_p(value); }
    static SizeOption nextUpPx(SizeOption preset) { return isPixelOption(preset) ? _instance_px.nextUp_p(preset) : SizeOption::custom_px; }
    static SizeOption nextDownPx(double value) { return _instance_px.nextDown_p(value); }
    static SizeOption nextDownPx(SizeOption preset) { return isPixelOption(preset) ? _instance_px.nextDown_p(preset) : SizeOption::custom_px; }

    static SizeOption nearestPercent(double value, double threshold = 0) { return _instance_percent.nearest_p(value, threshold); }
    static SizeOption nextUpPercent(double value) { return _instance_percent.nextUp_p(value); }
    static SizeOption nextUpPercent(SizeOption preset) { return isPercentOption(preset) ? _instance_percent.nextUp_p(preset) : SizeOption::custom_percent; }
    static SizeOption nextDownPercent(double value) { return _instance_percent.nextDown_p(value); }
    static SizeOption nextDownPercent(SizeOption preset) { return isPercentOption(preset) ? _instance_percent.nextDown_p(preset) : SizeOption::custom_percent; }

};

#undef BRUSH_PX_SIZE_VALUES_INIT
#undef BRUSH_PERCENT_SIZE_VALUES_INIT

#endif // BRUSHSIZEPRESETHELPER_HPP