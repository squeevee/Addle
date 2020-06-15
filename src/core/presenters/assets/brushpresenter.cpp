#include "brushpresenter.hpp"
#include "servicelocator.hpp"

#include "interfaces/models/ibrushmodel.hpp"

#include "utilities/qtextensions/qlist.hpp"
#include "globalconstants.hpp"

void BrushPresenter::initialize(IBrushModel& model)
{
    _initHelper.initializeBegin();

    _model = &model;
    _id = _model->id();

    _sizeSelection = ServiceLocator::makeUnique<ISizeSelectionPresenter>();

    {
        _iconHelper.setBackground(Qt::white);

        QList<double> presets = model.info().getPreferredSizes();
        if (presets.isEmpty())
            presets = qListFromArray(IBrushPresenterAux::DEFAULT_SIZES);

        _sizeSelection->setPresets(presets);

        QList<QIcon> icons;
        for(double preset : presets)
        {
            icons.append(_iconHelper.icon(_id, Qt::blue, preset));
        }
        _sizeSelection->setPresetIcons(icons);
    }

    _initHelper.initializeEnd();
}

void BrushPresenter::initialize(BrushId id)
{
    _initHelper.initializeBegin();

    initialize(ServiceLocator::get<IBrushModel>(id));

    _initHelper.initializeEnd();
}
