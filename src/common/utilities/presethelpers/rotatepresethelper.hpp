#ifndef ROTATEPRESETHELPER_HPP
#define ROTATEPRESETHELPER_HPP

#include "presethelper.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

class RotatePresetHelper : public PresetHelper<IViewPortPresenter::RotatePreset, double>
{
    typedef IViewPortPresenter::RotatePreset RotatePreset;
    static const RotatePresetHelper _instance;
    RotatePresetHelper()
        : PresetHelper(
            true,
            RotatePreset::nullrotation,
            {
                { RotatePreset::_0deg,   0.0 },
                { RotatePreset::_45deg,  45.0 },
                { RotatePreset::_90deg,  90.0 },
                { RotatePreset::_135deg, 135.0 },
                { RotatePreset::_180deg, 180.0 },
                { RotatePreset::_225deg, 225.0 },
                { RotatePreset::_270deg, 270.0 },
                { RotatePreset::_315deg, 315.0 }
            }
        )
    {
    }

public:
    inline static double rotateValueOf(RotatePreset preset) { return _instance.valueOf_p(preset); }
    inline static RotatePreset nearest(double value, double threshold = 0) { return _instance.nearest_p(value, threshold); }
    inline static RotatePreset nextUp(double value) { return _instance.nextUp_p(value); }
    inline static RotatePreset nextUp(RotatePreset preset) { return _instance.nextUp_p(preset); }
    inline static RotatePreset nextDown(double value) { return _instance.nextDown_p(value); }
    inline static RotatePreset nextDown(RotatePreset preset) { return _instance.nextDown_p(preset); }

};

#endif // ROTATEPRESETHELPER_HPP