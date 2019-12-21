#ifndef ZOOMPRESETHELPER_HPP
#define ZOOMPRESETHELPER_HPP

#include "presethelper.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

class ZoomPresetHelper : public PresetHelper<IViewPortPresenter::ZoomPreset, double>
{
    typedef IViewPortPresenter::ZoomPreset ZoomPreset;

    ZoomPresetHelper()
        : PresetHelper(
            false,
            ZoomPreset::nullzoom,
            {
                { ZoomPreset::_5percent,     0.05 },
                { ZoomPreset::_10percent,    0.10 },
                { ZoomPreset::onesixth, (1.0 / 6) },
                { ZoomPreset::_25percent,    0.35 },
                { ZoomPreset::onethird, (1.0 / 3) },
                { ZoomPreset::_50percent,    0.50 },
                { ZoomPreset::_75percent,    0.75 },
                { ZoomPreset::_100percent,   1.00 },
                { ZoomPreset::_150percent,   1.50 },
                { ZoomPreset::_200percent,   2.00 },
                { ZoomPreset::_300percent,   3.00 },
                { ZoomPreset::_400percent,   4.00 },
                { ZoomPreset::_500percent,   5.00 },
                { ZoomPreset::_800percent,   8.00 },
                { ZoomPreset::_1000percent, 10.00 },
                { ZoomPreset::_1600percent, 16.00 },
                { ZoomPreset::_2000percent, 20.00 }
            }
        )
    {
    }

    static const ZoomPresetHelper _instance;

public:
    inline static double zoomValueOf(IViewPortPresenter::ZoomPreset preset) { return _instance.valueOf_p(preset); }
    inline static ZoomPreset nearest(double value, double threshold = 0) { return _instance.nearest_p(value, threshold); }
    inline static ZoomPreset nextUp(double value) { return _instance.nextUp_p(value); }
    inline static ZoomPreset nextUp(ZoomPreset preset) { return _instance.nextUp_p(preset); }
    inline static ZoomPreset nextDown(double value) { return _instance.nextDown_p(value); }
    inline static ZoomPreset nextDown(ZoomPreset preset) { return _instance.nextDown_p(preset); }

};

#endif // ZOOMPRESETHELPER_HPP