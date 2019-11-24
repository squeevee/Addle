#ifndef ZOOMROTATEHELPER_HPP
#define ZOOMROTATEHELPER_HPP

#include "common/interfaces/presenters/iviewportpresenter.hpp"

#include <map> //todo: QMap
#include <set>

class ZoomRotateHelper
{
public:
    ZoomRotateHelper();

    IViewPortPresenter::ZoomPreset nearest(double value, double threshold = 0);
    IViewPortPresenter::ZoomPreset nextUp(double value);
    IViewPortPresenter::ZoomPreset nextUp(IViewPortPresenter::ZoomPreset preset);
    IViewPortPresenter::ZoomPreset nextDown(double value);
    IViewPortPresenter::ZoomPreset nextDown(IViewPortPresenter::ZoomPreset preset);

    static double zoomValueOf(IViewPortPresenter::ZoomPreset preset);

private:
    const std::map<double, IViewPortPresenter::ZoomPreset> _zoomValues;
    const std::set<IViewPortPresenter::ZoomPreset> _zoomPresets;
};

#endif // ZOOMROTATEHELPER_HPP