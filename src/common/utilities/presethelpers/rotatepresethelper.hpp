#ifndef ROTATEPRESETHELPER_HPP
#define ROTATEPRESETHELPER_HPP

#include "presethelper.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

#define ROTATE_VALUES_INIT(x) { RotatePresetHelper::rotateValueOf(x), x }

class RotatePresetHelper : public PresetHelper<IViewPortPresenter::RotatePreset>
{
    typedef IViewPortPresenter::RotatePreset RotatePreset;
    static const RotatePresetHelper _instance;
    RotatePresetHelper()
        : PresetHelper(
            true,
            RotatePreset::nullrotation,
            RotatePreset::_0deg,
            RotatePreset::_315deg,
            {
                RotatePreset::_0deg,
                RotatePreset::_45deg,
                RotatePreset::_90deg,
                RotatePreset::_135deg,
                RotatePreset::_180deg,
                RotatePreset::_225deg,
                RotatePreset::_270deg,
                RotatePreset::_315deg
            },
            {
                ROTATE_VALUES_INIT(RotatePreset::_0deg),
                ROTATE_VALUES_INIT(RotatePreset::_45deg),
                ROTATE_VALUES_INIT(RotatePreset::_90deg),
                ROTATE_VALUES_INIT(RotatePreset::_135deg),
                ROTATE_VALUES_INIT(RotatePreset::_180deg),
                ROTATE_VALUES_INIT(RotatePreset::_225deg),
                ROTATE_VALUES_INIT(RotatePreset::_270deg),
                ROTATE_VALUES_INIT(RotatePreset::_315deg)
            }
        )
    {
    }

public:
    inline static double rotateValueOf(IViewPortPresenter::RotatePreset preset)
    {
        switch(preset)
        {
        case RotatePreset::_0deg:
            return 0;
        case RotatePreset::_45deg:
            return 45;
        case RotatePreset::_90deg:
            return 90;
        case RotatePreset::_135deg:
            return 135;
        case RotatePreset::_180deg:
            return 180;
        case RotatePreset::_225deg:
            return 225;
        case RotatePreset::_270deg:
            return 270;
        case RotatePreset::_315deg:
            return 315;
        default:
            return qQNaN();
        }
    }

    static RotatePreset nearest(double value, double threshold = 0) { return _instance.nearest_p(value, threshold); }
    static RotatePreset nextUp(double value) { return _instance.nextUp_p(value); }
    static RotatePreset nextUp(RotatePreset preset) { return _instance.nextUp_p(preset); }
    static RotatePreset nextDown(double value) { return _instance.nextDown_p(value); }
    static RotatePreset nextDown(RotatePreset preset) { return _instance.nextDown_p(preset); }

};

#endif // ROTATEPRESETHELPER_HPP