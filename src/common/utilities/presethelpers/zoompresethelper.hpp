#ifndef ZOOMPRESETHELPER_HPP
#define ZOOMPRESETHELPER_HPP

#include "presethelper.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

#define ZOOM_VALUES_INIT( x ) { ZoomPresetHelper::zoomValueOf(x), x }

class ZoomPresetHelper : public PresetHelper<IViewPortPresenter::ZoomPreset>
{
    typedef IViewPortPresenter::ZoomPreset ZoomPreset;

    ZoomPresetHelper()
        : PresetHelper(
            false,
            ZoomPreset::nullzoom,
            ZoomPreset::_5percent,
            ZoomPreset::_2000percent,
            {
                ZoomPreset::_5percent,
                ZoomPreset::_10percent,
                ZoomPreset::onesixth,
                ZoomPreset::_25percent,
                ZoomPreset::onethird,
                ZoomPreset::_50percent,
                ZoomPreset::_75percent,
                ZoomPreset::_100percent,
                ZoomPreset::_150percent,
                ZoomPreset::_200percent,
                ZoomPreset::_300percent,
                ZoomPreset::_400percent,
                ZoomPreset::_500percent,
                ZoomPreset::_800percent,
                ZoomPreset::_1000percent,
                ZoomPreset::_1600percent,
                ZoomPreset::_2000percent
            },
            {
                ZOOM_VALUES_INIT(ZoomPreset::_5percent),
                ZOOM_VALUES_INIT(ZoomPreset::_10percent),
                ZOOM_VALUES_INIT(ZoomPreset::onesixth),
                ZOOM_VALUES_INIT(ZoomPreset::_25percent),
                ZOOM_VALUES_INIT(ZoomPreset::onethird),
                ZOOM_VALUES_INIT(ZoomPreset::_50percent),
                ZOOM_VALUES_INIT(ZoomPreset::_75percent),
                ZOOM_VALUES_INIT(ZoomPreset::_100percent),
                ZOOM_VALUES_INIT(ZoomPreset::_150percent),
                ZOOM_VALUES_INIT(ZoomPreset::_200percent),
                ZOOM_VALUES_INIT(ZoomPreset::_300percent),
                ZOOM_VALUES_INIT(ZoomPreset::_400percent),
                ZOOM_VALUES_INIT(ZoomPreset::_500percent),
                ZOOM_VALUES_INIT(ZoomPreset::_800percent),
                ZOOM_VALUES_INIT(ZoomPreset::_1000percent),
                ZOOM_VALUES_INIT(ZoomPreset::_1600percent),
                ZOOM_VALUES_INIT(ZoomPreset::_2000percent)
            }
        )
    {
    }

    static const ZoomPresetHelper _instance;

public:
    inline static double zoomValueOf(IViewPortPresenter::ZoomPreset preset)
    {
        switch(preset)
        {
        case ZoomPreset::_5percent:
            return 0.05;
        case ZoomPreset::_10percent:
            return 0.10;
        case ZoomPreset::onesixth:
            return 0.1667;
        case ZoomPreset::_25percent:
            return 0.25;
        case ZoomPreset::onethird:
            return 0.3333;
        case ZoomPreset::_50percent:
            return 0.50;
        case ZoomPreset::_75percent:
            return 0.75;
        case ZoomPreset::_100percent:
            return 1.00;
        case ZoomPreset::_150percent:
            return 1.50;
        case ZoomPreset::_200percent:
            return 2.00;
        case ZoomPreset::_300percent:
            return 3.00;
        case ZoomPreset::_400percent:
            return 4.00;
        case ZoomPreset::_500percent:
            return 5.00;
        case ZoomPreset::_800percent:
            return 8.00;
        case ZoomPreset::_1000percent:
            return 10.00;
        case ZoomPreset::_1600percent:
            return 16.00;
        case ZoomPreset::_2000percent:
            return 20.00;
        default:
            return qQNaN();
        }
    }

    static ZoomPreset nearest(double value, double threshold = 0) { return _instance.nearest_p(value, threshold); }
    static ZoomPreset nextUp(double value) { return _instance.nextUp_p(value); }
    static ZoomPreset nextUp(ZoomPreset preset) { return _instance.nextUp_p(preset); }
    static ZoomPreset nextDown(double value) { return _instance.nextDown_p(value); }
    static ZoomPreset nextDown(ZoomPreset preset) { return _instance.nextDown_p(preset); }

};

#undef ZOOM_VALUES_INIT

#endif // ZOOMPRESETHELPER_HPP