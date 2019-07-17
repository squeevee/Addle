#ifndef ZOOMROTATEHELPER_H
#define ZOOMROTATEHELPER_H

#include "interfaces/presenters/ieditorviewportpresenter.h"

#include <map>
#include <set>

class ZoomRotateHelper
{
public:
    ZoomRotateHelper();

    IEditorViewPortPresenter::ZoomPreset nearest(double value, double threshold = 0);
    IEditorViewPortPresenter::ZoomPreset nextUp(double value);
    IEditorViewPortPresenter::ZoomPreset nextUp(IEditorViewPortPresenter::ZoomPreset preset);
    IEditorViewPortPresenter::ZoomPreset nextDown(double value);
    IEditorViewPortPresenter::ZoomPreset nextDown(IEditorViewPortPresenter::ZoomPreset preset);

    static double zoomValueOf(IEditorViewPortPresenter::ZoomPreset preset);

private:
    const std::map<double, IEditorViewPortPresenter::ZoomPreset> _zoomValues;
    const std::set<IEditorViewPortPresenter::ZoomPreset> _zoomPresets;
};

#endif //ZOOMROTATEHELPER_H