#ifndef BRUSHSIZEPRESETHELPER_HPP
#define BRUSHSIZEPRESETHELPER_HPP

#include <QtGlobal>

#include "presethelper.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"

class BrushSizePresetHelper
{
    typedef IBrushPresenter::SizeOption SizeOption;

    class PxPresetHelper : public PresetHelper<IBrushPresenter::SizeOption, double>
    {
        PxPresetHelper()
            : PresetHelper(
                false,
                SizeOption::custom_px,
                {
                    { SizeOption::_1px,   1.0 },
                    { SizeOption::_2px,   2.0 },
                    { SizeOption::_4px,   4.0 },
                    { SizeOption::_7px,   7.0 },
                    { SizeOption::_13px,  13.0 },
                    { SizeOption::_25px,  25.0 },
                    { SizeOption::_50px,  50.0 },
                    { SizeOption::_100px, 100.0 },
                    { SizeOption::_250px, 250.0 },
                    { SizeOption::_500px, 500.0 }
                }
            )
        {
        }

        friend class BrushSizePresetHelper;
    };

    class PercentPresetHelper : public PresetHelper<IBrushPresenter::SizeOption, double>
    {
        friend class BrushSizePresetHelper;
        PercentPresetHelper()
            : PresetHelper(
                false,
                SizeOption::custom_percent,
                {
                    { SizeOption::_10percent, 0.10 },
                    { SizeOption::_25percent, 0.25 },
                    { SizeOption::onethird,  (1.0 / 3) },
                    { SizeOption::_50percent, 0.50 }
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
        case SizeOption::_1px:
        case SizeOption::_2px:
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

    inline static bool isPercentOption(SizeOption option) { return !isPixelOption(option); }

    inline static double pixelValueOf(SizeOption option)
    {
        if (isPixelOption(option))
            return _instance_px.valueOf_p(option);
        else 
            return qQNaN();
    }

    inline static double percentValueOf(SizeOption option)
    {
        if (isPercentOption(option))
            return _instance_percent.valueOf_p(option) * 100;
        else
            return qQNaN();
    }

    inline static double fractionValueOf(SizeOption option)
    {
        if (isPercentOption(option))
        {
            return percentValueOf(option);
        }
        return qQNaN();
    }

    inline static SizeOption nearestPx(double value, double threshold = 0) { return _instance_px.nearest_p(value, threshold); }
    inline static SizeOption nextUpPx(double value) { return _instance_px.nextUp_p(value); }
    inline static SizeOption nextUpPx(SizeOption preset) { return isPixelOption(preset) ? _instance_px.nextUp_p(preset) : SizeOption::custom_px; }
    inline static SizeOption nextDownPx(double value) { return _instance_px.nextDown_p(value); }
    inline static SizeOption nextDownPx(SizeOption preset) { return isPixelOption(preset) ? _instance_px.nextDown_p(preset) : SizeOption::custom_px; }

    inline static SizeOption nearestPercent(double value, double threshold = 0) { return _instance_percent.nearest_p(value, threshold); }
    inline static SizeOption nextUpPercent(double value) { return _instance_percent.nextUp_p(value); }
    inline static SizeOption nextUpPercent(SizeOption preset) { return isPercentOption(preset) ? _instance_percent.nextUp_p(preset) : SizeOption::custom_percent; }
    inline static SizeOption nextDownPercent(double value) { return _instance_percent.nextDown_p(value); }
    inline static SizeOption nextDownPercent(SizeOption preset) { return isPercentOption(preset) ? _instance_percent.nextDown_p(preset) : SizeOption::custom_percent; }

};

#endif // BRUSHSIZEPRESETHELPER_HPP